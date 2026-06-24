# Migration Plan: HoloLens 2 to Meta Quest 3

This document outlines the steps to migrate the `ar_integration` project to the Meta Quest 3, maintaining compatibility with the existing gRPC host server.

## 1. Core Architecture Changes

### A. Networking & gRPC (The "Heavy Lift")
The current gRPC setup is tied to Windows/UWP.
- **Action:** Update `Grpc.Build.cs` to support `UnrealTargetPlatform.Android`.
- **Toolchain:** Configure `vcpkg` to use the **Android NDK** (included with UE) for cross-compilation. Target triplet: `arm64-android`.
- **Dependency:** Ensure `asio-grpc` and `grpc` are built with OpenSSL for Android.

### B. Registration & Point Clouds
Since the server expects a Point Cloud format:
- **Approach 1 (Raw PCL):** Use Meta's **Experimental Camera Access** (Quest 3 Research Mode). This requires the `com.oculus.permission.USE_SCENE` and potentially a custom OVR camera wrapper.
- **Approach 2 (Synthetic PCL):** Sample points from the Quest 3 **Spatial Mapping Mesh**. This provides a "cleaner" environment PCL that is already in the server's expected format.
- **Aruco Integration:** Use the **OpenCV for Unreal** plugin or a custom OpenCV build to detect markers. This provides the "Ground Truth" for the Table/Kinect/Quest synchronization.

### C. UI & Interaction
- **Replacement:** Swap `UXTools` (MRTK) for **Meta XR Interaction SDK**.
- **Visuals:** Convert HoloLens shaders to **Mobile-friendly (Vulkan)** shaders. Avoid complex transparency; use Meta's Passthrough API for the background.

## 2. Updated gRPC Service Plan

To support Aruco-based registration while keeping the server happy:
1.  **Client-Side Transformation:** The Quest 3 detects the Aruco marker, calculates the transform to the "Table Space," and then transforms all Point Clouds (synthetic or raw) into that space *before* sending them via `transmit_pcl_data`.
2.  **New RPC (Optional):** Add `rpc transmit_marker_pose (MarkerPose) returns (google.protobuf.Empty)` to `services.proto` to explicitly inform the server of the marker location.

## 3. Implementation Phases

### Phase 1: Android Build System (Target: 1-2 weeks)
- Modify `Grpc.Build.cs` to detect Android.
- Download and integrate Android-specific binaries for gRPC/Protobuf.
- Validate a "Ping" gRPC call from a Quest 3 build to the host.

### Phase 2: Passthrough & Interaction (Target: 1 week)
- Set up `Meta XR Plugin`.
- Implement basic "Passthrough" background.
- Replace current Blueprint UI with Interaction SDK Canvas/Buttons.

### Phase 3: Registration & PCL (Target: 2 weeks)
- Implement `A_quest_camera_client`.
- Integrate OpenCV for Aruco detection.
- Implement the "Synthetic PCL" generator that samples the Scene Mesh.

### Phase 4: 3-Space Sync Validation
- Use the Aruco marker on the table to align the Quest 3 world with the Kinect/Robot world.
- Verify that voxels/robots received from gRPC align with physical reality.

## 4. Risks & Mitigations
- **Risk:** gRPC compilation for Android is notoriously difficult.
- **Mitigation:** Use a pre-built gRPC-Android library if `vcpkg` integration becomes too complex.
- **Risk:** Raw camera access latency.
- **Mitigation:** Use the Aruco marker for the initial "static" transform and the Scene Mesh for the environment PCL, rather than high-frequency raw frame processing.
