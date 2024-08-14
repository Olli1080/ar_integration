using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Text;
using System.Linq;
using System.Text.RegularExpressions;
using EpicGames.Core;
using UnrealBuildTool;

public class Research : ModuleRules
{
    private void SafeCopy(string source, string destination)
	{
		if(!File.Exists(source))
		{
			Log.TraceError("Class {0} can't find {1} file for copying", this.GetType().Name, source);
			return;
		}

		try
		{
			File.Copy(source, destination, true);
		}
		catch(IOException ex)
		{
			Log.TraceWarning("Failed to copy {0} to {1} with exception: {2}", source, destination, ex.Message);
			if (!File.Exists(destination))
			{
				Log.TraceError("Destination file {0} does not exist", destination);
				return;
			}

			Log.TraceWarning("Destination file {0} already existed and is probably in use.  The old file will be used for the runtime dependency.  This may happen when packaging a Win64 exe from the editor.", destination);
		}
	}

    public Research(ReadOnlyTargetRules Target) : base(Target)
    {
        CppStandard = CppStandardVersion.Cpp20;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "GeometryCore", "Engine", "AugmentedReality"
        });

        PrivateDependencyModuleNames.Add("MicrosoftOpenXR");

        PublicIncludePathModuleNames.Add("OpenXR");
        //PrivateIncludePaths.Add(Path.Combine(PluginDirectory, "..", "MicrosoftOpenXR/Source/MicrosoftOpenXR/Private/External"));

        // WinRT with Nuget support
        if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.HoloLens)
        {
            // these parameters mandatory for winrt support
            bEnableExceptions = true;
            bUseUnity = false;
            CppStandard = CppStandardVersion.Cpp20;
            PublicSystemLibraries.AddRange(new string[] { "shlwapi.lib", "runtimeobject.lib" });

            // prepare everything for nuget
            string MyModuleName = GetType().Name;
            string NugetFolder = Path.Combine(PluginDirectory, "Intermediate", "Nuget", MyModuleName);
            Directory.CreateDirectory(NugetFolder);

            string BinariesSubFolder = Path.Combine("Binaries", "ThirdParty", Target.Type.ToString(), Target.Platform.ToString(), Target.Architecture.ToString());

            PrivateDefinitions.Add($"THIRDPARTY_BINARY_SUBFOLDER=\"{BinariesSubFolder.Replace(@"\", @"\\")}\"");

            string BinariesFolder = Path.Combine(PluginDirectory, BinariesSubFolder);
            Directory.CreateDirectory(BinariesFolder);

            ExternalDependencies.Add("packages.config");

            // download nuget
			string NugetExe = Path.Combine(NugetFolder, "nuget.exe");
			if (!File.Exists(NugetExe))
			{
				// The System.Net assembly is not referenced by the build tool so it must be loaded dynamically.
				var assembly = System.Reflection.Assembly.Load("System.Net");
				var webClient = assembly.CreateInstance("System.Net.WebClient");
				using ((IDisposable)webClient)
				{
					// we aren't focusing on a specific nuget version, we can use any of them but the latest one is preferable
					var downloadFileMethod = webClient.GetType().GetMethod("DownloadFile", new Type[] { typeof(string), typeof(string) });
					downloadFileMethod.Invoke(webClient, new object[] { @"https://dist.nuget.org/win-x86-commandline/latest/nuget.exe", NugetExe } );
				}
			}
            // run nuget to update the packages
			{
                Utils.RunLocalProcessAndReturnStdOut(NugetExe,
                    $"install \"{Path.Combine(ModuleDirectory, "packages.config")}\" -OutputDirectory \"{NugetFolder}\"", out var ExitCode, true);
				if (ExitCode < 0)
				{
					throw new BuildException("Failed to get nuget packages.  See log for details.");
				}
			}

            // get list of the installed packages, that's needed because the code should get particular versions of the installed packages
            string[] InstalledPackages = Utils.RunLocalProcessAndReturnStdOut(NugetExe,
                $"list -Source \"{NugetFolder}\"").Split(new char[] { '\r', '\n' });

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                // Microsoft.VCRTForwarders.140 is needed to run WinRT dlls in Win64 platforms
                string VCRTForwardersPackage = InstalledPackages.FirstOrDefault(x => x.StartsWith("Microsoft.VCRTForwarders.140"));
                if (!string.IsNullOrEmpty(VCRTForwardersPackage))
                {
                    string VCRTForwardersName = VCRTForwardersPackage.Replace(" ", ".");
                    foreach (var Dll in Directory.EnumerateFiles(Path.Combine(NugetFolder, VCRTForwardersName, "runtimes/win10-x64/native/release"), "*_app.dll"))
                    {
                        string newDll = Path.Combine(BinariesFolder, Path.GetFileName(Dll));
                        SafeCopy(Dll, newDll);
                        RuntimeDependencies.Add(newDll);
                    }
                }
            }
            // get WinRT package 
            string CppWinRTPackage = InstalledPackages.FirstOrDefault(x => x.StartsWith("Microsoft.Windows.CppWinRT"));
            if (!string.IsNullOrEmpty(CppWinRTPackage))
            {
                string CppWinRTName = CppWinRTPackage.Replace(" ", ".");
                string CppWinRTExe = Path.Combine(NugetFolder, CppWinRTName, "bin", "cppwinrt.exe");
                string CppWinRTFolder = Path.Combine(PluginDirectory, "Intermediate", CppWinRTName, MyModuleName);
                Directory.CreateDirectory(CppWinRTFolder);

                /*// generate winrt headers and add them into include paths
                var StartInfo = new System.Diagnostics.ProcessStartInfo(CppWinRTExe, string.Format("-input \"{0}\" -output \"{1}\"", Target.WindowsPlatform.WindowsSdkVersion, CppWinRTFolder));
                StartInfo.UseShellExecute = false;
                StartInfo.CreateNoWindow = true;
                var ExitCode = Utils.RunLocalProcessAndPrintfOutput(StartInfo);
                if (ExitCode < 0)
                {
                    throw new BuildException("Failed to get generate WinRT headers.  See log for details.");
                }*/

// generate winrt headers and add them into include paths
                Utils.RunLocalProcessAndReturnStdOut(CppWinRTExe,
                    $"-input \"{Target.WindowsPlatform.WindowsSdkVersion}\" -output \"{CppWinRTFolder}\"", out var ExitCode, true);   
				if (ExitCode < 0)
				{
					throw new BuildException("Failed to get generate WinRT headers.  See log for details.");
				}			

                PublicIncludePaths.Add(CppWinRTFolder);
            }
            else
            {
                // fall back to default WinSDK headers if no winrt package in our list
                PublicIncludePaths.Add(Path.Combine(Target.WindowsPlatform.WindowsSdkDir, "Include", Target.WindowsPlatform.WindowsSdkVersion, "cppwinrt"));
            }
        }
    }
}
