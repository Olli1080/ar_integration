# Setup Guide: Quest 3 AR Integration (UE 5.7.4)

This guide explains how to set up the development environment and deploy the AR Integration project to a Meta Quest 3 after the migration to Unreal Engine 5.7.4.

## 1. Prerequisites

### Hardware
- **Meta Quest 3** (or Quest 3S/Pro).
- **Development PC** (Windows 10/11).
- **USB-C Cable** (for initial deployment).

### Software
- **Unreal Engine 5.7.4** (installed via Epic Games Launcher):
  - **IMPORTANT:** You MUST run Unreal Engine at least once after installation. This registers the engine version in the Windows Registry, which is required for project file generation.
- **Android Studio** (Electric Eel or newer):
  - **Important:** After installing, you MUST run Android Studio at least once to complete the initial SDK setup.
  - **SDK Tools:** In Android Studio, go to **Settings > Languages & Frameworks > Android SDK > SDK Tools**. Ensure **"Android SDK Command-line Tools (latest)"** is checked and installed.
- **Visual Studio 2022 or 2026** with:
  - Desktop development with C++
  - Windows 10/11 SDK
  - **Note for VS 2026:** You MUST install the "MSVC v143 - VS 2022 C++ x64/x86 build tools" component in the VS Installer.
- **Git** (with LFS support).
- **Python 3.x** (for setup scripts).

## 2. Android Toolchain Configuration

Unreal Engine 5.7.4 requires a specific Android NDK and SDK.

1.  Navigate to your UE installation: `.../UE_5.7/Engine/Extras/Android`.
2.  Run `SetupAndroid.bat`.
3.  Restart your PC to ensure environment variables (`ANDROID_NDK_HOME`, `JAVA_HOME`) are correctly set.
4.  In Unreal Engine: **Project Settings > Platforms > Android SDK**.
    - Ensure the paths point to the locations created by the script (usually in `AppData/Local/Android/Sdk`).

## 3. Repository Initialization

1.  **Clone the Repo:**
    ```bash
    git clone https://github.com/Olli1080/ar_integration.git -b quest3-migration
    cd ar_integration
    ```
2.  **Initialize Submodules:**
    ```bash
    git submodule update --init --recursive
    ```
3.  **Run Setup Script:**
    ```bash
    python setup_dependencies.py
    ```
    *This script performs the following:*
    - **NDK Auto-Discovery:** Attempts to locate your Android NDK automatically (checking env vars, `local.properties`, and default system paths).
    - **Plugin Download:** Fetches Meta-specific plugins and OpenCV.
    - **vcpkg Bootstrapping:** Initializes the local vcpkg environment for gRPC.

## 4. Building the Project

### A. Initial gRPC Build (vcpkg)
The first time you build the project for a new platform, `vcpkg` will compile the gRPC dependencies.

1.  Right-click `ar_integration.uproject` and select **Generate Visual Studio project files**.
2.  Open `ar_integration.sln` in Visual Studio.
3.  Set the solution configuration to **Development Editor** and the platform to **Win64**.
4.  **Build Solution.**
    *   *Note: This will trigger the `vcpkg` build for Win64. The Android build happens during the Packaging phase.*

### B. Packaging for Quest 3 (Android)
1.  Open the project in **Unreal Editor**.
2.  Go to **Platforms > Android**.
3.  Select **Package Project**.
    *   *Note: On the first run, the `Grpc.Build.cs` will trigger `vcpkg` to build the `arm64-android` libraries. This may take 20-40 minutes depending on your CPU. Subsequent builds will be near-instant.*

## 5. Deployment & Registration

1.  Enable **Developer Mode** on your Quest 3 via the Meta Quest mobile app.
2.  Connect the headset to your PC.
3.  In Unreal Editor, click the **Platforms** dropdown, select your Quest 3, and click **Launch**.
4.  **Registration Procedure:**
    - Place the Aruco marker on the physical table.
    - Start the app on the Quest.
    - Look at the Aruco marker. The app will automatically create a Spatial Anchor and align the virtual world (Robot/Kinect) with the physical table.

## 6. Troubleshooting

- **gRPC Connection Failed:** Ensure your Quest 3 and Host PC are on the same Wi-Fi network. Check the Host IP in the in-game menu.
- **Camera Access Denied:** Ensure you have accepted the "Camera Access" prompt on the headset.
- **vcpkg Build Errors:** Check the `Binaries/vcpkg_logs` folder. Ensure `ANDROID_NDK_HOME` is set and points to NDK r27 (or the version required by UE 5.7).
- **VS 2026 Issues:** If Unreal Build Tool (UBT) fails to find the compiler, verify that the **v143 build tools** are installed via the VS 2026 Installer.
