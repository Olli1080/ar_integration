using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using UnrealBuildTool;

public class Grpc : ModuleRules
{
    class VcpkgBasePaths
    {
        public VcpkgBasePaths(string root)
        {
            this.root = root;
        }

        public string root;
        public string exe => Path.Combine(root, "vcpkg.exe");
        public string installed => Path.Combine(root, "installed");
        public string triplets => Path.Combine(root, "triplets");
    }

    class VcpkgPaths
    {
        public VcpkgPaths(string tripletName, VcpkgBasePaths basePaths)
        {
            this.tripletName = tripletName;
            this.basePaths = basePaths;
        }

        public VcpkgBasePaths basePaths;
        public string tripletName;

        public string info => Path.Combine(basePaths.installed, "vcpkg", "info");
        public string tripletInstalled => Path.Combine(basePaths.installed, tripletName);

        public string lib => Path.Combine(tripletInstalled, "lib");
        public string bin => Path.Combine(tripletInstalled, "bin");
        public string include => Path.Combine(tripletInstalled, "include");
    }

    private string mTargetTriplet;
    private string mProjectPath => Path.Combine(PluginDirectory, "..", "..");

    class LibFiles
    {
        public List<string> libs = new List<string>();
        public List<string> dlls = new List<string>();
    }

    private int getLibRank(string libPath)
    {
        string name = Path.GetFileName(libPath);
        if (name.StartsWith("libopencv")) return 1;
        if (name.StartsWith("libgrpc") || name.StartsWith("libgpr") || name.StartsWith("libaddress_sorting")) return 2;
        if (name.StartsWith("libssl")) return 3;
        if (name.StartsWith("libcrypto")) return 4;
        if (name.StartsWith("libprotobuf")) return 5;
        if (name.StartsWith("libabsl")) return 6;
        return 7;
    }

    private List<string> getInstalledFiles(VcpkgPaths vcpkgPaths)
    {
        List<string> InstalledFiles = new List<string>();
        if (!Directory.Exists(vcpkgPaths.info))
        {
            return InstalledFiles;
        }

        string[] files = Directory.GetFiles(vcpkgPaths.info);
        foreach (string file in files)
        {
            if (file.EndsWith("_" + vcpkgPaths.tripletName + ".list"))
            {
                InstalledFiles.Add(file);
            }
        }
        return InstalledFiles;
    }

    private LibFiles getRessources(List<string> installedFiles, string tripletName)
    {
        LibFiles result = new();

        Regex regLib = new Regex($@"^{tripletName}\/(?:lib|sdk\/native\/staticlibs\/arm64-v8a)\/.*?\.(?:lib|a)$");
        Regex regBin = new Regex($@"^{tripletName}\/bin\/.*?\.dll$");

        foreach (string InstalledFile in installedFiles)
        {
            string fileName = Path.GetFileName(InstalledFile);
            // Exclude packages that are problematic on specific targets
            if (tripletName.Contains("android") && fileName.StartsWith("c-ares_"))
            {
                continue;
            }

            foreach (string Line in File.ReadAllLines(InstalledFile))
            {
                if (regLib.IsMatch(Line))
                {
                    string relPath = Line.Substring(tripletName.Length + 1);
                    if (tripletName.Contains("android"))
                    {
                        if (relPath == "lib/libssl.a")
                            relPath = "lib/libssl_vcpkg.a";
                        else if (relPath == "lib/libcrypto.a")
                            relPath = "lib/libcrypto_vcpkg.a";
                    }
                    result.libs.Add(relPath);
                }
                else if (regBin.IsMatch(Line))
                {
                    string relPath = Line.Substring(tripletName.Length + 1);
                    result.dlls.Add(relPath);
                }
            }
        }
        return result;
    }

    private string getTarget(ReadOnlyTargetRules Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
            return "x64-windows";
        else if (Target.Platform == UnrealTargetPlatform.Android)
            return "arm64-android";

        return "";
    }

    public Grpc(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableExceptions = true;
        PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI");
        PublicDefinitions.Add("GPR_FORBID_UNREACHABLE_CODE");
        PublicDefinitions.Add("GRPC_ALLOW_EXCEPTIONS=0");

        VcpkgBasePaths basePaths = new VcpkgBasePaths(
            Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "vcpkg"))
        );

        mTargetTriplet = getTarget(Target) + "-static-md";
        VcpkgPaths TargetPaths = new VcpkgPaths(mTargetTriplet, basePaths);

        if (!Directory.Exists(TargetPaths.tripletInstalled))
        {
            throw new BuildException($"Dependencies for triplet {mTargetTriplet} are not installed. " +
                                     "Please run 'python setup_dependencies.py' first to install them.");
        }

        // Copy libssl.a and libcrypto.a to non-lib-prefixed files on Android
        // to prevent UBT from converting them to -lssl and -lcrypto and linking engine legacy libs.
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string libDir = Path.Combine(TargetPaths.tripletInstalled, "lib");
            string oldSsl = Path.Combine(libDir, "libssl.a");
            string newSsl = Path.Combine(libDir, "libssl_vcpkg.a");
            if (File.Exists(oldSsl) && !File.Exists(newSsl))
            {
                File.Copy(oldSsl, newSsl);
            }

            string oldCrypto = Path.Combine(libDir, "libcrypto.a");
            string newCrypto = Path.Combine(libDir, "libcrypto_vcpkg.a");
            if (File.Exists(oldCrypto) && !File.Exists(newCrypto))
            {
                File.Copy(oldCrypto, newCrypto);
            }
        }

        List<string> InstalledFiles = getInstalledFiles(TargetPaths);
        var Ressources = getRessources(InstalledFiles, mTargetTriplet);

        // Sort libraries to satisfy strict linker ordering on platforms like Android
        Ressources.libs.Sort((a, b) => {
            int rankA = getLibRank(a);
            int rankB = getLibRank(b);
            if (rankA != rankB)
                return rankA.CompareTo(rankB);
            return string.Compare(a, b, StringComparison.OrdinalIgnoreCase);
        });

        PublicIncludePaths.Add(TargetPaths.include);
        PublicIncludePaths.Add(Path.Combine(TargetPaths.include, "opencv4"));
        PublicSystemLibraryPaths.Add(Path.Combine(TargetPaths.tripletInstalled, "lib"));

        string generatedDir = Path.Combine(mProjectPath, "Source", "ar_integration", "Generated");
        PublicIncludePaths.Add(generatedDir);

        // Add libraries twice to resolve cyclic dependencies (e.g. within abseil or protobuf)
        foreach (string Lib in Ressources.libs)
        {
            string LibFilePath = Path.Combine(TargetPaths.tripletInstalled, Lib);
            PublicAdditionalLibraries.Add(LibFilePath);
        }
        foreach (string Lib in Ressources.libs)
        {
            string LibFilePath = Path.Combine(TargetPaths.tripletInstalled, Lib);
            PublicAdditionalLibraries.Add(LibFilePath);
        }

        foreach (string Dll in Ressources.dlls)
        {
            string DllFilePath = Path.Combine(TargetPaths.tripletInstalled, Dll);
            RuntimeDependencies.Add("$(TargetOutputDir)/" + Path.GetFileName(Dll), DllFilePath);
        }

        PublicDependencyModuleNames.AddRange(new string[] { "Core" });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicSystemLibraries.Add("crypt32.lib");
        }

        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine" });
    }
}
