using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Text.RegularExpressions;
using UnrealBuildTool;

public class Grpc : ModuleRules
{
    //private List<string> mLibs = new List<string>();
    //private List<string> mDlls = new List<string>();

    private string mPluginPath;
    private string mCorePath;
    private string mVcpkgPath;
    private string mVcpkgExe;
    private string mInstalledPath;
    private string mInfoPath;
    private string mTripletPath;
    private string mLibPath;
    private string mBinPath;
    private string mIncludePath;
    private string mLibPathLinked;
    private string mBinPathLinked;
    private string mIncludePathLinked;
    private string mTarget;
    private string mTriplet;
    private string mHostTriplet;
    private string mProjectPath;

    private static string runProgram(string program, string arguments, bool redirect = false)
    {
        ProcessStartInfo startInfo = new()
        {
            CreateNoWindow = false,
            UseShellExecute = false,
            FileName = program,
            Arguments = arguments,
            RedirectStandardOutput = redirect
        };
        startInfo.UseShellExecute = false;

        string maybeOut = null;

        try
        {
            using (Process exeProcess = Process.Start(startInfo))
            {
                if (redirect)
                    maybeOut = exeProcess.StandardOutput.ReadToEnd();
                exeProcess.WaitForExit();

                return maybeOut;
            }
        }
        catch (Exception e)
        {
            throw new BuildException(e.Message);
        }
    }

    private HashSet<string> parseDepencies(string package)
    {
        //Regex reg = new Regex(@"^([^:\[]*?)(?:\[[^\]]*?\])??:(?:(?: ([^ ,]*?),)*? ([^ ]*?))??\r$", RegexOptions.Multiline);
        Regex reg = new Regex(@"^([^* :\[\]]*)(?:\[[^\]]*\])?:(.*)", RegexOptions.Multiline);
        HashSet<string> Packages = new HashSet<string>();

        string unparsed = runProgram(mVcpkgExe, "depend-info " + package + ":" + mTriplet, true);
        Console.WriteLine(unparsed);
        MatchCollection MatchCollection = reg.Matches(unparsed);

        foreach (Match match in MatchCollection)
        {
            Packages.Add(match.Groups[1].Value);
            //Console.Write(match.Groups[1]);
            //Console.Write("    -_-    ");

            foreach (string SubPackage in match.Groups[2].Value.Split(','))
            {
                Packages.Add(SubPackage.Trim());
                //Console.Write($"\"{VARIABLE.Trim()}\"");
            }
            //Console.WriteLine("---------meh");
            for (int i = 1; i < match.Groups.Count; ++i)
            {
                
                /*foreach (Capture capture in match.Groups[i].Captures)
                {
                    packages.Add(capture.Value);
                }*/
            }
        }
        Packages.Remove("");
        if (mTarget == "arm64-uwp")
            Packages.Remove("c-ares");
        
        return Packages;
    }

    private List<string> getInstalledFiles(HashSet<string> packages)
    {
        string[] files = Directory.GetFiles(mInfoPath);
        List<string> InstalledFiles = new List<string>();

        foreach (string Package in packages)
        {
            Regex regex = new Regex("^" + Package + @"_[^_]*?_" + mTriplet + @"\.list$");

            bool found = false;
            foreach (string file in files)
            {
                if (!regex.IsMatch(Path.GetFileName(file))) continue;

                Console.WriteLine(file + " is installed");
                InstalledFiles.Add(file);
                found = true;
                break;
            }

            if (!found)
            {
                if (!Package.Contains("vcpkg")) 
                    throw new BuildException(Package + " is not installed");
            }
        }
        return InstalledFiles;
    }
    /*
    private static void linkFolder(string target, string src)
    {
        if (Directory.Exists(target)) return;
		Directory.CreateDirectory(Path.Combine(target, ".."));
        runProgram("cmd.exe", "/C mklink /J " + "\"" + target + "\"" + " " + "\"" + src + "\"");
        if (!Directory.Exists(target))
            throw new BuildException("Linking directory failed: " + src + " -> " + target);
    }
    */
    private static void checkInstalled(string path, Action install)
    {
        if (File.Exists(path)) return;

        install();

        if (!File.Exists(path))
            throw new BuildException("Failed at install of: " + Path.GetFileName(path));
    }

    class LibFiles
    {
        public LibFiles() {}

        public List<string> libs = new List<string>();
        public List<string> dlls = new List<string>();
    }

    private LibFiles getRessources(List<string> installedFiles)
    {
        LibFiles result = new();

        Regex regLib = new Regex("^" + mTriplet + @"\/lib\/.*?\.lib");
        Regex regBin = new Regex("^" + mTriplet + @"\/bin\/.*?\.dll");

        foreach (string InstalledFile in installedFiles)
        {
            foreach (string Line in File.ReadAllLines(InstalledFile))
            {
                if (regLib.IsMatch(Line))
                    result.libs.Add(Path.GetFileName(Line));
                else if (regBin.IsMatch(Line))
                    result.dlls.Add(Path.GetFileName(Line));
            }
        }
        return result;
    }
    /*
    private void linkRessources()
    {
        linkFolder(mIncludePathLinked, mIncludePath);
        linkFolder(mLibPathLinked, mLibPath);
        if (mDlls.Count > 0)
            linkFolder(mBinPathLinked, mBinPath);
    }
    */
    private string getTarget(ReadOnlyTargetRules Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
            return "x64-windows";
        else if (Target.Platform == UnrealTargetPlatform.HoloLens)
        {
            if (Target.Architecture == UnrealArch.Arm64)
                return "arm64-uwp";
            else
            {
                //TODO:: not supported
            }
        }

        return "";
    }

    private void generateProtoFiles()
    {
        string toolsPath = Path.Combine(mInstalledPath, mHostTriplet, "tools");
        string gprc_plugin = Path.Combine(toolsPath, "grpc", "grpc_cpp_plugin.exe");
        string protoc = Path.Combine(toolsPath, "protobuf", "protoc.exe");
        string protoPath = Path.Combine(mProjectPath, "Proto");//Path.Combine(mCorePath, "assets", "grpc", "proto");

        string generatedDir = Path.Combine(mProjectPath, "Generated");
        string outSource = Path.Combine(mProjectPath, "Source", new DirectoryInfo(mProjectPath).Name);

        
        Console.WriteLine(Path.GetFullPath(generatedDir));

        Directory.CreateDirectory(generatedDir);
        foreach (FileInfo file in new DirectoryInfo(generatedDir).GetFiles()) 
            file.Delete();

        runProgram(protoc, "--proto_path=" + protoPath + " --grpc_out=" + generatedDir + " --cpp_out=" + generatedDir + " --plugin=protoc-gen-grpc=" + gprc_plugin + " " + protoPath + "/*.proto");

        foreach (FileInfo file in new DirectoryInfo(generatedDir).GetFiles())
        {
            if (file.Extension != ".cc")
            {
                Console.WriteLine("Gen header: \t" + file.Name);
                file.CopyTo(Path.Combine(outSource, file.Name), true);
                continue;
            }

            string oldName = file.Name;
            string oldFullName = file.FullName;
            file.MoveTo(file.FullName + ".temp");
            
            StreamReader oldFile = file.OpenText();
            StreamWriter newFile = File.CreateText(oldFullName);
            newFile.WriteLine("#include \"grpc_include_begin.h\"");

            string line = null;
            while ((line = oldFile.ReadLine()) != null)
                newFile.WriteLine(line);
            newFile.Write("#include \"grpc_include_end.h\"");
            newFile.Close();

            Console.WriteLine("Gen cc: \t" + oldName);
            File.Copy(oldFullName, Path.Combine(outSource, oldName), true);
        }
    }

    private string getHostTarget()
    {
        if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.Win64)
            return "x64-windows";
        else if (BuildHostPlatform.Current.Platform == UnrealTargetPlatform.HoloLens)
        {
            return "arm64-uwp";
        }
        return "";

        //Console.WriteLine(BuildHostPlatform.Current.Platform);
    }

    private void makeReleaseOnly(string triplet)
    { 
        string tripletPath = Path.Combine(mVcpkgPath, "triplets", "community", triplet + ".cmake");

        if (File.ReadLines(tripletPath).Contains("VCPKG_BUILD_TYPE release"))
            return;

        var tempFile = Path.GetTempFileName();
        var linesToKeep = File.ReadLines(tripletPath).Where(line => !line.Contains("VCPKG_BUILD_TYPE"));

        File.WriteAllLines(tempFile, linesToKeep);
        File.AppendAllLines(tempFile, new string[] { "set(VCPKG_BUILD_TYPE release)" });
        
        File.Move(tempFile, tripletPath, true);
    }

    public Grpc(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDefinitions.Add("GOOGLE_PROTOBUF_NO_RTTI");
        PublicDefinitions.Add("GPR_FORBID_UNREACHABLE_CODE");
        PublicDefinitions.Add("GRPC_ALLOW_EXCEPTIONS=0");
	
	    //Console.WriteLine(PluginDirectory);
	    //Console.WriteLine(ModuleDirectory);

        //mCorePath = Path.GetFullPath(Path.Combine(mPluginPath, "..", "..", "..", ".."));
        mVcpkgPath = Path.GetFullPath(Path.Combine(PluginDirectory, "Source", "vcpkg"));
        mVcpkgExe = Path.Combine(mVcpkgPath, "vcpkg.exe");
        mInstalledPath = Path.Combine(mVcpkgPath, "installed");
        mInfoPath = Path.Combine(mInstalledPath, "vcpkg", "info");
        mProjectPath = Path.Combine(PluginDirectory, "..", "..");

        List<string> Packages = new List<string>();
        Packages.AddRange(new string[] { "grpc", "draco", "asio-grpc" });

        mTarget = getTarget(Target);

        mTriplet = mTarget + "-static-md";
        mHostTriplet = getHostTarget() + "-static-md";
        mTripletPath = Path.Combine(mInstalledPath, mTriplet);
        mLibPath = Path.Combine(mTripletPath, "lib");
        mBinPath = Path.Combine(mTripletPath, "bin");
        mIncludePath = Path.Combine(mTripletPath, "include");

        //mLibPathLinked = Path.Combine(mPluginPath, "lib", mTarget);
        //mBinPathLinked = Path.Combine(mPluginPath, "bin", mTarget);
        //mIncludePathLinked = Path.Combine(mPluginPath, "include", mTarget);

        makeReleaseOnly(mHostTriplet);
        if (mHostTriplet != mTriplet)
            makeReleaseOnly(mTriplet);

        checkInstalled(mVcpkgExe, () =>
        {
            Console.WriteLine("Bootstrapping vcpkg");
            runProgram(Path.Combine(mVcpkgPath, "bootstrap-vcpkg.bat"), "");
        });

        string VcpkgCmd = "install --host-triplet=" + mHostTriplet;
        string InstallMessage = "Installing [";
        HashSet<string> SubPackages = new HashSet<string>();
        foreach (var Package in Packages)
        {
            SubPackages.UnionWith(parseDepencies(Package));
            VcpkgCmd += " " + Package + ":" + mTriplet;
            InstallMessage += " " + Package;
        }
        InstallMessage += " ] for " + mTriplet + " with host " + mHostTriplet;

        Console.WriteLine(InstallMessage);
        runProgram(mVcpkgExe, VcpkgCmd);

        foreach (string Package in SubPackages)
            Console.WriteLine(Package);

        List<string> InstalledFiles = getInstalledFiles(SubPackages);
        var Ressources = getRessources(InstalledFiles);

        foreach (string InstalledFile in InstalledFiles)
            Console.WriteLine(InstalledFile);
        
        //linkRessources();

        Console.WriteLine("Added header root: " + mIncludePath);
        PublicIncludePaths.Add(mIncludePath);

        Console.WriteLine("Adding lib files: ");
        foreach (string Lib in Ressources.libs)
        {
            string LibFilePath = Path.Combine(mLibPath, Lib);
            Console.WriteLine(LibFilePath);
            PublicAdditionalLibraries.Add(LibFilePath);
        }
        foreach (string Dll in Ressources.dlls)
        {
            string DllFilePath = Path.Combine(mBinPath, Dll);
            Console.WriteLine(DllFilePath);
            RuntimeDependencies.Add("$(TargetOutputDir)/" + Dll, DllFilePath);
        }

        string ProtoDir = Path.Combine(mProjectPath, "Proto");
        if (Directory.Exists(ProtoDir))
            generateProtoFiles();

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
            PublicSystemLibraries.Add("crypt32.lib");

        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine" });
    }
}
