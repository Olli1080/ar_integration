from pathlib import Path
from os.path import exists
from io import BytesIO
from zipfile import ZipFile
from urllib.request import urlopen
import subprocess
import os
import platform

def find_ndk_path(project_root="."):
    """
    Attempts to automatically find the Android NDK path.
    Returns the absolute path to the NDK, or None if not found.
    """
    # 1. Check Environment Variable first (explicit override)
    env_ndk = os.environ.get("ANDROID_NDK_HOME")
    if env_ndk and os.path.exists(env_ndk):
        print(f"Found via ANDROID_NDK_HOME: {env_ndk}")
        return env_ndk

    # 2. Check local.properties (if running inside an Android project)
    local_props_path = os.path.join(project_root, "local.properties")
    if os.path.exists(local_props_path):
        with open(local_props_path, "r") as f:
            for line in f:
                if line.startswith("ndk.dir="):
                    ndk_dir = line.split("=", 1)[1].strip()
                    ndk_dir = ndk_dir.replace("\\:", ":") 
                    if os.path.exists(ndk_dir):
                        print(f"Found via local.properties: {ndk_dir}")
                        return ndk_dir

    # 3. Check default OS installation directories
    system = platform.system()
    ndk_base_path = ""

    if system == "Windows":
        local_app_data = os.environ.get("LOCALAPPDATA", "")
        ndk_base_path = os.path.join(local_app_data, "Android", "Sdk", "ndk")
    elif system == "Darwin": # macOS
        ndk_base_path = os.path.expanduser("~/Library/Android/sdk/ndk")
    elif system == "Linux":
        ndk_base_path = os.path.expanduser("~/Android/Sdk/ndk")

    if os.path.exists(ndk_base_path):
        versions = os.listdir(ndk_base_path)
        versions = [v for v in versions if os.path.isdir(os.path.join(ndk_base_path, v))]
        if versions:
            versions.sort() 
            latest_version = versions[-1]
            best_path = os.path.join(ndk_base_path, latest_version)
            print(f"Found via OS defaults ({system}): {best_path}")
            return best_path
    return None

def run_unreal_project_actions(selector_path, uproject_path):
    """
    Offers the user a menu of actions to perform with the Unreal Version Selector.
    """
    print("\n" + "="*60)
    print("   UNREAL ENGINE PROJECT ACTIONS")
    print("="*60)
    print("The environment is ready. What would you like to do next?")
    print("\n[1] Generate Visual Studio project files (Recommended)")
    print("[2] Switch Unreal Engine version... (Use if you have multiple engines)")
    print("[3] Skip (I will do it manually later)")
    
    try:
        choice = input("\nSelect an action [1-3]: ").strip()
        
        if choice == "1":
            print(f"\n[INFO] Generating project files for {uproject_path.name}...")
            # Using /projectfiles directly generates the .sln
            subprocess.run([str(selector_path), "/projectfiles", str(uproject_path)], check=True)
            print("[SUCCESS] Project files generated.")
        elif choice == "2":
            print("\n[INFO] Opening engine selection dialog...")
            # /switchversion opens the Epic dialog to pick the engine
            subprocess.run([str(selector_path), "/switchversion", str(uproject_path)], check=True)
            print("[INFO] After switching, remember to generate project files.")
        else:
            print("\n[INFO] Skipping project actions.")
    except Exception as e:
        print(f"\n[ERROR] Failed to execute project action: {e}")

# --- Ensure Working Directory is Project Root ---
filePath = Path(__file__).parent.resolve()
os.chdir(filePath)

uproject_file = Path.joinpath(filePath, "ar_integration.uproject")
if (not exists(uproject_file)):
    raise Exception("Not inside UE Project root or ar_integration.uproject not accessible!")

# --- Registry & File Association ---
# Modify register_unreal_version_selector to return the path it found
def register_unreal_version_selector_with_return():
    """
    Refactored version of the registration logic that returns the found selector path.
    Registers per-user (.uproject and Unreal.ProjectFile) settings directly to HKCU.
    """
    if platform.system() != "Windows": return None
    print("Registering Unreal Version Selector for current user...")
    
    potential_paths = [
        filePath.parent / "UnrealEngine" / "Engine" / "Binaries" / "Win64" / "UnrealVersionSelector.exe",
        os.path.join(os.environ.get("ProgramFiles", "C:\\Program Files"), "Epic Games\\Launcher\\Engine\\Binaries\\Win64\\UnrealVersionSelector.exe"),
        os.path.join(os.environ.get("ProgramFiles(x86)", "C:\\Program Files (x86)"), "Epic Games\\Launcher\\Engine\\Binaries\\Win64\\UnrealVersionSelector.exe"),
    ]

    try:
        where_out = subprocess.check_output(["where", "UnrealVersionSelector.exe"], stderr=subprocess.STDOUT).decode().strip()
        potential_paths.insert(0, Path(where_out.split('\n')[0]))
    except: pass

    selector_path = next((p for p in potential_paths if os.path.exists(p)), None)
    if not selector_path:
        print("WARNING: UnrealVersionSelector.exe not found.")
        return None

    print(f"Found Unreal Version Selector at: {selector_path}")
    
    try:
        import ctypes
        import winreg

        print("Applying per-user Registry association (HKEY_CURRENT_USER)...")
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, r"Software\Classes\.uproject") as key:
            winreg.SetValue(key, "", winreg.REG_SZ, "Unreal.ProjectFile")
        
        prog_id_path = r"Software\Classes\Unreal.ProjectFile"
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, prog_id_path) as key:
            winreg.SetValue(key, "", winreg.REG_SZ, "Unreal Engine Project File")
        
        gen_path = prog_id_path + r"\shell\generate"
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, gen_path) as key:
            winreg.SetValue(key, "", winreg.REG_SZ, "&Generate Visual Studio project files")
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, gen_path + r"\command") as key:
            winreg.SetValue(key, "", winreg.REG_SZ, f'"{selector_path}" /projectfiles "%1"')
        
        switch_path = prog_id_path + r"\shell\switch"
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, switch_path) as key:
            winreg.SetValue(key, "", winreg.REG_SZ, "Switch &Unreal Engine version...")
        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, switch_path + r"\command") as key:
            winreg.SetValue(key, "", winreg.REG_SZ, f'"{selector_path}" /switchversion "%1"')

        with winreg.CreateKey(winreg.HKEY_CURRENT_USER, prog_id_path + r"\shell\open\command") as key:
            winreg.SetValue(key, "", winreg.REG_SZ, f'"{selector_path}" /editor "%1"')

        print("[SUCCESS] Per-user .uproject association applied successfully.")
        
        # Notify shell of change
        try:
            ctypes.windll.shell32.SHChangeNotify(0x08000000, 0, None, None)
        except: pass
    except Exception as e:
        print(f"WARNING: Registry registration failed: {e}")

    return selector_path

# Replace the original call with the new logic
found_selector = register_unreal_version_selector_with_return()

# --- Resolve Android NDK Path ---
ndk_path = find_ndk_path(str(filePath))
if ndk_path:
    os.environ["ANDROID_NDK_HOME"] = ndk_path
    print(f"NDK successfully mapped to: {os.environ['ANDROID_NDK_HOME']}")
else:
    print("WARNING: Could not automatically locate the Android NDK.")
    print("Please install the NDK via Android Studio or set ANDROID_NDK_HOME manually.")

# 1. Download Meta XR Plugin (Base)
PluginPath = Path.joinpath(filePath, "Plugins")
metaPluginPath = Path.joinpath(PluginPath, "OculusXR")
if not exists(metaPluginPath):
    print("Downloading Meta XR Plugin...")
    # Base Meta XR Plugin
    resp = urlopen("https://securecdn-fra3-1.oculus.com/binaries/download/?id=26658614450416657")
    myzip = ZipFile(BytesIO(resp.read()))
    myzip.extractall(PluginPath)
    # The zip might extract as MetaXR, in which case we rename it to OculusXR.
    tempMetaXR = Path.joinpath(PluginPath, "MetaXR")
    if exists(tempMetaXR) and not exists(metaPluginPath):
        os.rename(tempMetaXR, metaPluginPath)
    print("Done: Meta XR Plugin downloaded")

# 1b. Download Meta XR Interaction SDK
metaInteractionPath = Path.joinpath(PluginPath, "OculusInteraction")
if not exists(metaInteractionPath):
    print("Downloading Meta XR Interaction SDK...")
    # Provided ID for Interaction SDK
    resp = urlopen("https://securecdn-fra3-1.oculus.com/binaries/download/?id=26643703888595373")
    myzip = ZipFile(BytesIO(resp.read()))
    myzip.extractall(PluginPath)
    # The zip might extract into a different folder name or directly. 
    # If it extracts as MetaXRInteraction, we should rename it to OculusInteraction.
    tempPath = Path.joinpath(PluginPath, "MetaXRInteraction")
    if exists(tempPath) and not exists(metaInteractionPath):
        os.rename(tempPath, metaInteractionPath)
    print("Done: Meta XR Interaction SDK downloaded")

# --- Oculus XR Branding Alignment ---
# Ensure the uplugin is named OculusXR.uplugin for project compatibility.
legacy_uplugin = metaPluginPath / "MetaXR.uplugin"
target_uplugin = metaPluginPath / "OculusXR.uplugin"
if exists(legacy_uplugin):
    print(f"Renaming {legacy_uplugin.name} to {target_uplugin.name} for project compatibility...")
    if exists(target_uplugin):
        os.remove(target_uplugin)
    os.rename(legacy_uplugin, target_uplugin)

# 2. Setup gRPC dependencies and compile Protobufs
try:
    print("\n[INFO] Triggering modular gRPC and Protobuf setup...")
    import sys
    setup_grpc_script = filePath / "Plugins" / "grpc_plugin" / "setup_grpc.py"
    subprocess.run([sys.executable, str(setup_grpc_script)], check=True)
except Exception as e:
    print(f"\n[ERROR] Failed to execute setup_grpc.py: {e}")
    sys.exit(1)

print("Setup logic updated for Quest 3 / UE 5.7.4 migration.")

# --- Final Project Actions ---
if found_selector:
    print("\n" + "="*60)
    print("   FINALIZING PROJECT")
    print("="*60)
    
    import json
    try:
        with open(uproject_file, 'r') as f:
            uproject_data = json.load(f)
            engine_assoc = uproject_data.get("EngineAssociation", "Unknown")
            print(f"[INFO] Current Engine Association: {engine_assoc}")
    except:
        pass

    print(f"[INFO] Attempting to generate project files using discovered selector...")
    
    try:
        # We use a slight delay or check if we can run it
        # Exit code 1 often means the EngineAssociation GUID is unknown to this machine's VersionSelector
        result = subprocess.run([str(found_selector), "/projectfiles", str(uproject_file)], 
                                capture_output=True, text=True)
        
        if result.returncode == 0:
            print("\n[SUCCESS] Visual Studio solution generated successfully.")
            print("         You can now open 'ar_integration.sln' and build.")
        else:
            print(f"\n[WARNING] Generation failed (Code {result.returncode}).")
            print("[INFO] This usually happens when the Engine GUID in the .uproject is not recognized.")
            print("[INFO] Opening 'Switch Engine Version' dialog to help you link your engine...")
            
            # Trigger the switch version dialog so the user can pick their engine
            subprocess.run([str(found_selector), "/switchversion", str(uproject_file)], check=True)
            
            print("\n[INFO] After you have selected the engine, would you like to try generating again?")
            retry = input("Retry generation? [y/N]: ").strip().lower()
            if retry == 'y':
                subprocess.run([str(found_selector), "/projectfiles", str(uproject_file)], check=True)
                print("[SUCCESS] Project files generated.")
            else:
                print("[INFO] Please remember to right-click .uproject and 'Generate project files' later.")
                
    except Exception as e:
        print(f"\n[ERROR] Project finalization encountered an error: {e}")
        run_unreal_project_actions(found_selector, uproject_file)
else:
    print("\n[WARNING] Could not find UnrealVersionSelector.exe.")
    print("          Please manually generate project files via right-click on .uproject.")
