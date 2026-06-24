# PLAN: Quest 3 Migration for AR Integration (UE 5.7.4)

This document outlines the execution strategy for migrating the HoloLens 2 project to Meta Quest 3 using **Unreal Engine 5.7.4**.

## 1. Core Objectives
- **Maintain Server Compatibility:** Keep gRPC point cloud streams identical to current server expectations.
- **Platform Transition:** Move from Win64/UWP (HoloLens) to Android/Vulkan (Quest 3).
- **Engine Upgrade:** Migrate to **Unreal Engine 5.7.4** for enhanced OpenXR stability and Nanite mobile support.
- **Registration Upgrade:** Implement 3-space synchronization using Aruco markers and Meta Spatial Anchors.
- **UI Modernization:** Swap UXTools for Meta XR Interaction SDK.

## 2. Technical Infrastructure (gRPC & vcpkg)
We will preserve the "compile-once" logic in `Grpc.Build.cs` but expand it for Android.

- **Action:** Update `Grpc.Build.cs` to handle `UnrealTargetPlatform.Android`.
- **Target Triplet:** `arm64-android-static-md` (matching Unreal's STL).
- **Compiler:** Support **Visual Studio 2026** while enforcing the **v143 (2022) build tools** for stable UE 5.7.4 cross-compilation.
- **Optimization:** Use the existing `checkInstalled` logic to ensure `vcpkg` only builds dependencies (gRPC, asio-grpc, etc.) if they are missing for the Android triplet.
- **NDK Integration:** Dynamically resolve the Android NDK path from Unreal's environment variables to ensure `vcpkg` uses the same toolchain as UBT.

## 3. Registration & Coordinate Systems
The "Table Space" is the master reference.

- **Workflow:**
    1. **Detection:** Quest 3 RGB cameras (via Camera2 API) detect the Aruco marker on the table.
    2. **Locking:** Upon detection, create a **Meta Spatial Anchor** at the marker's pose.
    3. **Persistence:** Use the Spatial Anchor to maintain the "Table Space" even if the marker is occluded.
    4. **Transformation:** All local Quest coordinates (hands, robot, mesh) are transformed relative to this anchor before being sent to the gRPC server.

## 4. Feature Replication Mapping

| HoloLens Feature | Quest 3 Implementation | Notes |
| :--- | :--- | :--- |
| **PCL Generation** | **Scene Mesh Sampling** | Sample vertices from Meta's Scene Mesh to create the environment Point Cloud. |
| **Research Mode** | **Public Camera API** | Use `horizonos.permission.HEADSET_CAMERA` for Aruco detection. |
| **UXTools Buttons** | **ISDK Poke Interactable** | Rebuild simple menus using Meta's Interaction SDK. | [DONE] |
 **Hand Tracking** | **Meta Hand Tracking v2** | Superior fidelity; fully compatible with Interaction SDK. |
| **Spatial Anchors** | **Meta Spatial Anchors** | Replaces `UARPin` with more robust persistence. |

## 5. Implementation Phases

### Phase 1: Android Build System [DONE]
- Update `Grpc.Build.cs` with Android triplet support.
- Configure `AndroidManifest.xml` with required permissions (`CAMERA`, `HEADSET_CAMERA`).
- Verify successful APK build with linked gRPC libraries.

### Phase 2: Passthrough & Interaction [DONE]
- Install Meta XR Plugin and Interaction SDK.
- Configure `MainMap` for Passthrough rendering.
- Re-implement menus using Widget Interaction components.

### Phase 3: Aruco & Registration [DONE]
- Integrate OpenCV (Android version) for marker detection.
- Implement the `A_QuestRegistrationManager` to handle Marker -> Spatial Anchor logic.
- Update `integration_game_state` to use the new anchor-based coordinate system.

### Phase 4: Data Streaming & Validation [DONE]
- Implement the "Synthetic PCL" generator.
- Verify 3-space sync: Kinect (Server), Robot (Server), and Quest (Client) should align on the physical table.

## 6. Critical Dependencies
- **Meta XR Plugin:** Core runtime.
- **Meta XR Interaction SDK:** UI/Hands.
- **OpenCV for Unreal:** Aruco detection.
- **NDK r25b+:** Required for modern Android builds in UE 5.4.
