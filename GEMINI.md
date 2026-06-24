# AR Integration Project (HoloLens & Quest 3)

An Unreal Engine 5.7.4 project designed for Microsoft HoloLens 2 and Meta Quest 3, facilitating real-time integration between Augmented Reality and remote services via gRPC.

## Project Overview

- **Core Technology:** Unreal Engine 5.7.4, gRPC, OpenXR, Meta XR SDK (Quest), UXTools (Legacy HoloLens).
- **Primary Goal:** Real-time visualization and interaction with remote robotics and geometry data.
- **Architecture:**
    - **Central Hub:** `A_integration_game_state` manages gRPC clients and Quest/HoloLens specific managers.
    - **Quest Support:** 
        - `A_QuestManager`: Handles Passthrough and Performance settings.
        - `A_QuestRegistrationManager`: Manages Aruco marker detection and Spatial Anchors.
        - `A_QuestCamera`: Generates synthetic Point Clouds from the Scene Mesh.
    - **Networking:** Custom `Grpc` plugin with support for `arm64-android` and `arm64-uwp`.

## Building and Running

### Prerequisites
- **Unreal Engine 5.4** (with HoloLens support).
- **Visual Studio 2022** with `Desktop development with C++` and UWP/ARM64 build tools (see `README.md` for specific versions).
- **HoloLens 2** device.

### Initial Setup
1.  **Clone Submodules:** `git submodule update --init --recursive`
2.  **Setup Dependencies:** `python setup_dependencies.py` (downloads necessary plugins).
3.  **Generate Project Files:** Right-click `ar_integration.uproject` -> "Generate Visual Studio project files".

### Build & Deploy
1.  **Compile:** Open `ar_integration.sln` in VS 2022.
    - Build for `Development Editor` (x64) to run in the UE Editor.
    - Build for `Shipping` or `Development` (ARM64) for HoloLens deployment.
2.  **Cook & Package:** In the UE Editor:
    - `Platforms` -> `HoloLens` -> `Cook Content`.
    - `Platforms` -> `HoloLens` -> `Package Project`.
3.  **Install:** Deploy the generated `.appxbundle` to the HoloLens via the Device Portal (Web Interface).

## Key Components & Paths

- **Source Code:** `Source/ar_integration/`
    - `integration_game_state.h/cpp`: Main application controller.
    - `grpc_channel.h/cpp`: gRPC connection management.
    - `*client.h/cpp`: Specialized gRPC client implementations.
- **Protobuf Definitions:** `Proto/`
    - `robot.proto`: Robot joints, voxels, and TCP data.
    - `object.proto`: Dynamic object instances and boxes.
    - `services.proto`: General service definitions.
- **Plugins:** `Plugins/`
    - `grpc_plugin`: Custom gRPC integration for Unreal.
    - `UXTools`: MRTK for UI and interaction.
    - `MicrosoftOpenXR`: HoloLens runtime support.
- **Content:** `Content/`
    - Contains materials, blueprinted actors, and assets for the AR experience.

## Development Conventions

- **gRPC Usage:** Most data is streamed from a host application. Ensure the host IP is correctly configured in the UE Project Settings or via the in-game UI.
- **Thread Safety:** gRPC callbacks often run on background threads. Use `std::mutex` and carefully marshal data to the Game Thread (e.g., via `set_list` in `A_integration_game_state`).
- **Spatial Anchors:** The project relies on `UARPin` for persistent workspace alignment. The anchor name is typically `ROBOT_AR_PIN`.
- **C++ Version:** Uses **C++20** (Note: UWP support for C++20 may have limitations; see comments in `ar_integration.Build.cs`).
- **Procedural Geometry:** Use `A_procedural_mesh_actor` for any geometry that needs to update frequently based on remote data.
