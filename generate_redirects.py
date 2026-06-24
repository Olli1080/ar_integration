import os, re

classes = {
    'A_franka_tcps': 'AFrankaTcps',
    'A_franka_voxel': 'AFrankaVoxel',
    'A_integration_game_state': 'AIntegrationGameState',
    'A_pcl_client': 'APclClient',
    'A_procedural_mesh_actor': 'AProceduralMeshActor',
    'A_QuestCamera': 'AQuestCamera',
    'A_QuestInteractionManager': 'AQuestInteractionManager',
    'A_QuestManager': 'AQuestManager',
    'A_QuestRegistrationManager': 'AQuestRegistrationManager',
    'A_voxel': 'AVoxel',
    'U_debug_client': 'UDebugClient',
    'U_franka_client': 'UFrankaClient',
    'U_franka_tcp_client': 'UFrankaTcpClient',
    'U_franka_joint_client': 'UFrankaJointClient',
    'U_franka_joint_sync_client': 'UFrankaJointSyncClient',
    'U_franka_Interface': 'UFrankaInterface',
    'U_franka_shadow_controller': 'UFrankaShadowController',
    'U_grpc_channel': 'UGrpcChannel',
    'U_grpc_wrapper': 'UGrpcWrapper',
    'U_ip_input': 'UIpInput',
    'U_mesh_client': 'UMeshClient',
    'U_object_client': 'UObjectClient',
    'U_Base_Client_Interface': 'UBaseClientInterface'
}

def strip_prefix(name):
    if name[0] in ['A', 'U', 'I', 'F', 'E']:
        return name[1:]
    return name

redirects = []

for old_c, new_c in classes.items():
    if old_c == new_c: continue
    old_stripped = strip_prefix(old_c)
    new_stripped = strip_prefix(new_c)
    redirects.append(f'+ClassRedirects=(OldName="/Script/ar_integration.{old_stripped}",NewName="/Script/ar_integration.{new_stripped}",MatchSubstring=true)')

with open('Config/DefaultEngine.ini', 'r', encoding='utf-8') as f:
    ini_content = f.read()

# Strip existing CoreRedirects
ini_content = re.sub(r'\[CoreRedirects\].*', '', ini_content, flags=re.DOTALL)

with open('Config/DefaultEngine.ini', 'w', encoding='utf-8') as f:
    f.write(ini_content.strip() + '\n\n[CoreRedirects]\n' + '\n'.join(redirects) + '\n')
