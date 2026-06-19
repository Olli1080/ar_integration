import os
import sys
import subprocess
import shutil
import glob
import platform
from pathlib import Path

# --- Path Resolution ---
plugin_dir = Path(__file__).parent.resolve()
project_root = plugin_dir.parent.parent.resolve()
proto_dir = project_root / "Proto"
generated_dir = project_root / "Source" / "ar_integration" / "Generated"
vcpkg_root = plugin_dir / "Source" / "vcpkg"
vcpkg_exe = vcpkg_root / "vcpkg.exe"
bootstrap_bat = vcpkg_root / "bootstrap-vcpkg.bat"
overlay_dir = plugin_dir / "Source" / "overlay"

def find_ndk_path(project_root_path):
    """
    Attempts to automatically find the Android NDK path.
    """
    env_ndk = os.environ.get("ANDROID_NDK_HOME")
    if env_ndk and os.path.exists(env_ndk):
        return env_ndk

    local_props_path = os.path.join(project_root_path, "local.properties")
    if os.path.exists(local_props_path):
        with open(local_props_path, "r") as f:
            for line in f:
                if line.startswith("ndk.dir="):
                    ndk_dir = line.split("=", 1)[1].strip()
                    ndk_dir = ndk_dir.replace("\\:", ":")
                    if os.path.exists(ndk_dir):
                        return ndk_dir

    system = platform.system()
    ndk_base_path = ""
    if system == "Windows":
        local_app_data = os.environ.get("LOCALAPPDATA", "")
        ndk_base_path = os.path.join(local_app_data, "Android", "Sdk", "ndk")
    elif system == "Darwin":
        ndk_base_path = os.path.expanduser("~/Library/Android/sdk/ndk")
    elif system == "Linux":
        ndk_base_path = os.path.expanduser("~/Android/Sdk/ndk")

    if os.path.exists(ndk_base_path):
        versions = os.listdir(ndk_base_path)
        versions = [v for v in versions if os.path.isdir(os.path.join(ndk_base_path, v))]
        if versions:
            versions.sort()
            latest_version = versions[-1]
            return os.path.join(ndk_base_path, latest_version)
    return None


def run_cmd(cmd, cwd=None, shell=False):
    print(f"Executing: {' '.join(cmd) if isinstance(cmd, list) else cmd}")
    result = subprocess.run(cmd, cwd=cwd, shell=shell, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        print(f"Stdout:\n{result.stdout}")
        print(f"Stderr:\n{result.stderr}")
        raise RuntimeError(f"Command execution failed: {cmd}")
    return result.stdout

def bootstrap_vcpkg():
    if not vcpkg_exe.exists():
        print("Bootstrapping vcpkg...")
        if bootstrap_bat.exists():
            run_cmd([str(bootstrap_bat), "-disableMetrics"], cwd=str(vcpkg_root))
        else:
            raise RuntimeError(f"vcpkg bootstrap script not found at {bootstrap_bat}")
    else:
        print("vcpkg is already bootstrapped.")

def setup_triplet(triplet_name):
    triplet_paths = [
        vcpkg_root / "triplets" / f"{triplet_name}.cmake",
        vcpkg_root / "triplets" / "community" / f"{triplet_name}.cmake"
    ]
    
    triplet_file = None
    for path in triplet_paths:
        if path.exists():
            triplet_file = path
            break
            
    if not triplet_file:
        # If not found, default to creating it in triplets directory
        triplet_file = vcpkg_root / "triplets" / f"{triplet_name}.cmake"
        print(f"Creating new triplet file: {triplet_file}")
        with open(triplet_file, "w") as f:
            f.write(f"set(VCPKG_TARGET_ARCHITECTURE arm64)\n")
            f.write(f"set(VCPKG_CRT_LINKAGE dynamic)\n")
            f.write(f"set(VCPKG_LIBRARY_LINKAGE static)\n")
            if "android" in triplet_name:
                f.write(f"set(VCPKG_CMAKE_SYSTEM_NAME Android)\n")
                f.write(f"set(VCPKG_CMAKE_SYSTEM_VERSION 28)\n")
                f.write(f"set(VCPKG_MAKE_BUILD_TRIPLET \"--host=aarch64-linux-android\")\n")
                f.write(f"set(VCPKG_CMAKE_CONFIGURE_OPTIONS -DANDROID_ABI=arm64-v8a)\n")
    
    print(f"Configuring triplet: {triplet_file}")
    with open(triplet_file, "r") as f:
        lines = f.readlines()
        
    toolset_version = "14.44"  # Default toolset configuration
    
    # Filter out existing VCPKG_BUILD_TYPE and VCPKG_PLATFORM_TOOLSET_VERSION
    new_lines = [l for l in lines if "VCPKG_BUILD_TYPE" not in l and "VCPKG_PLATFORM_TOOLSET_VERSION" not in l]
    new_lines.append("\nset(VCPKG_BUILD_TYPE release)\n")
    if "windows" in triplet_name:
        new_lines.append(f"set(VCPKG_PLATFORM_TOOLSET_VERSION \"{toolset_version}\")\n")
    
    with open(triplet_file, "w") as f:
        f.writelines(new_lines)

def install_dependencies():
    host_triplet = "x64-windows-static-md"
    target_triplets = ["x64-windows-static-md", "arm64-android-static-md"]
    
    # 1. Setup triplets first
    for triplet in target_triplets:
        setup_triplet(triplet)
    setup_triplet(host_triplet)
    
    # 2. Build host helper packages
    print("Installing vcpkg-cmake...")
    vcpkg_cmd = [
        str(vcpkg_exe),
        "install",
        "--recurse",
        f"--overlay-ports={overlay_dir}",
        f"--host-triplet={host_triplet}",
        f"--vcpkg-root={vcpkg_root}",
        "vcpkg-cmake"
    ]
    run_cmd(vcpkg_cmd)
    
    # 3. Build target packages
    packages = ["grpc", "asio-grpc", "base-transformation", "opencv4[contrib,png,jpeg]"]
    
    for triplet in target_triplets:
        print(f"Installing dependencies for triplet {triplet}...")
        triplet_cmd = [
            str(vcpkg_exe),
            "install",
            "--recurse",
            f"--overlay-ports={overlay_dir}",
            f"--host-triplet={host_triplet}",
            f"--vcpkg-root={vcpkg_root}"
        ]
        # Append packages with triplet suffix
        for pkg in packages:
            triplet_cmd.append(f"{pkg}:{triplet}")
            
        run_cmd(triplet_cmd)

def generate_protobufs():
    if not proto_dir.exists():
        print(f"Proto directory not found at {proto_dir}. Skipping generation.")
        return
        
    print("Generating Protobuf & gRPC files...")
    
    host_triplet = "x64-windows-static-md"
    # Locate protoc.exe and grpc_cpp_plugin.exe inside the host vcpkg tools folder
    installed_tools_dir = vcpkg_root / "installed" / host_triplet / "tools"
    gprc_plugin = installed_tools_dir / "grpc" / "grpc_cpp_plugin.exe"
    protoc = installed_tools_dir / "protobuf" / "protoc.exe"
    
    if not protoc.exists() or not gprc_plugin.exists():
        raise RuntimeError(f"Required tools not found. Protoc: {protoc}, Plugin: {gprc_plugin}")
        
    # Recreate the target generated directory
    if generated_dir.exists():
        shutil.rmtree(generated_dir)
    generated_dir.mkdir(parents=True, exist_ok=True)
    
    # Get all proto files
    proto_files = glob.glob(str(proto_dir / "*.proto"))
    if not proto_files:
        print("No proto files found.")
        return
        
    # Run protoc
    cmd = [
        str(protoc),
        f"--proto_path={proto_dir}",
        f"--grpc_out={generated_dir}",
        f"--cpp_out={generated_dir}",
        f"--plugin=protoc-gen-grpc={gprc_plugin}"
    ] + proto_files
    
    run_cmd(cmd)
    
    # Post-process generated .cc files to wrap them in gRPC begin/end headers for UE compatibility
    for cc_file_path in glob.glob(str(generated_dir / "*.cc")):
        print(f"Wrapping generated file: {os.path.basename(cc_file_path)}")
        with open(cc_file_path, "r") as f:
            lines = f.readlines()
            
        lines.insert(0, '#include "grpc_include_begin.h"\n')
        lines.append('#include "grpc_include_end.h"\n')
        
        with open(cc_file_path, "w") as f:
            f.writelines(lines)
            
    print(f"Successfully generated files inside: {generated_dir}")

def main():
    print("Starting gRPC Plugin Setup...")
    
    # Locate and set Android NDK path
    ndk_path = find_ndk_path(str(project_root))
    if ndk_path:
        os.environ["ANDROID_NDK_HOME"] = ndk_path
        print(f"Android NDK resolved to: {ndk_path}")
    else:
        print("WARNING: Android NDK not found. Android vcpkg triplet builds might fail.")

    try:
        bootstrap_vcpkg()
        install_dependencies()
        generate_protobufs()
        print("gRPC Plugin Setup completed successfully!")
    except Exception as e:
        print(f"Error during gRPC setup: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
