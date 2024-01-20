#include "franka_voxel.h"

#include "grpc_wrapper.h"
#include "Components/InstancedStaticMeshComponent.h"

A_franka_voxel::A_franka_voxel()
{
	PrimaryActorTick.bCanEverTick = false;

	/**
	 * load global mesh and material by engine ref
	 */
	mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;

	mat = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/voxel_material_opaque.voxel_material_opaque'")).Object;

	/**
	 * setup instanced mesh component
	 */
	instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("instance_component"));
	instanced->SetStaticMesh(mesh);
	instanced->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//instanced->SetFlags(RF_Transactional);
	instanced->SetMaterial(0, mat);
	instanced->SetMobility(EComponentMobility::Movable);
	this->AddInstanceComponent(instanced);
}

void A_franka_voxel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void A_franka_voxel::BeginDestroy()
{
	Super::BeginDestroy();
}

void A_franka_voxel::set_voxels(const F_voxel_data& data)
{
	instanced->ClearInstances();
	instanced->SetRelativeTransform(data.robot_origin.Inverse());
	
	for (const auto& p : data.indices)
	{
		instanced->AddInstance(FTransform(FQuat::Identity,
			p * data.voxel_side_length, //TODO:: InScale3D check
			FVector(0.01 * data.voxel_side_length)), true);
	}
}