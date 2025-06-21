# Preparation
1. Register as developer for the unreal engine to get access to the source code
2. Install Visual Studio 2022: select the "Desktop development with C++" workload and additionally install:
    * C++ Universal Windows Platform support for v 143 build tools (ARM64/ARM64EC)
    * MSVC v143 - VS 2022 C++ ARM64/ARM64EC build tools (v14.38-17.8)
    * MSVC v143 - VS 2022 + x64/x86 build tools (v14.38-17.8)
    * C++ v14.38 (17.8) MFC for v143 build tools (ARM)
    * C++ v14.38 (17.8) MFC for v143 build tools (ARM64)
    * C++ v14.38 (17.8) MFC for v143 build tools (x86 & x64)
    * C++ v14.38 (17.8) ATL forv143 build tools (ARM)
    * C++ v14.38 (17.8) ATL forv143 build tools (ARM64)
    * C++ v14.38 (17.8) ATL for v143 build tools (x86 & x64)
3. Clone Repository: `git clone https://github.com/Olli1080/UnrealEngine.git -b 5.4.3` (you may wanna use `--single-branch` to avoid overhead)
4. Follow the Instructions in the readme or use the Unreal Engine builder `https://github.com/Olli1080/Unreal-Binary-Builder.git`

# Getting Started
1. Clone Repository: `git clone https://github.com/Olli1080/ar_integration.git -b 5.4.3` .
2. Clone Sub-Repositories `git submodule update --init --recursive`.
3. Run in the checked-out repository: `python setup_dependencies.py`.
4. Install Epic Games Launcher: https://launcher-public-service-prod06.ol.epicgames.com/launcher/api/installer/download/EpicGamesLauncherInstaller.msi
5. Right click on `ar_integration.uproject` and select Generate Visual Studio project files (requires Unreal Engine with support for HoloLens). 
6. Open the generated ar_integration.sln with Visual Studio 2022.
7. Right click on `ar_integration` in the solution explorer and select "Build". If you encounter build errors, examine the vcpkg logs and fix the ports. Use, for instance overlay ports as done in `Plugins\grpc_plugin\Source\overlay`
8. Connect the HoloLens to the same network as the host and note the IP addresses of both devices (will be needed in later steps)
9. Open `ar_integration.uproject` in Unreal Engine.
    1. Edit -> Project Settings -> [On the left of the dialog window] HoloLens under Platforms -> Signing Certificate under Packaging -> Click Generate. Under Toolchain select the installed Windows 10 SDK Version.
    2. Tools -> Find Blueprints -> Find Blueprints 1 -> Enter "192.168.100.100" and search -> double click the first match -> change the IP address to the one of the host
    3. Platforms (dropdown below navigation bar) -> Hololens -> Select "Shipping" and click "Cook Content" (redo this step if it fails)
    4. Platforms -> Hololens -> Click "Package Project". Select or create "Binaries" as destination folder in the ar_integration repository.
10. Enter the IP address of the HoloLens in Edge browser to get the web interface: Views -> Apps -> Click "Allow me to select framework packages" and "Choose File" -> Select the files from step 10.4 (in the first step "ar_integration.appxbundle" and in the second step "Microsoft.VCLibs.arm64.14.00.appx")
11. Start the main application on the host
12. Start ArSurvey on the HoloLens. 

# Development (Advanced)
- If you want to see something in the editor you will want to click the following menu `Create -> Only Create Lighting`
- Every time you make changes to the code you have to
    1. Compile the changes for arm64 HoloLens in Visual Studio
    2. Package for HoloLens in Editor
    3. Upload the new package via device portal
    4. Compile or Hot-Reload the changes for development editor x64, to keep the environments behaviour in sync (optional)
