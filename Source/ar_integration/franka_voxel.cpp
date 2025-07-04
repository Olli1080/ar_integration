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
		TEXT("Material'/Game/vox_mat.vox_mat'")).Object;

	auto root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(root);

	/**
	 * setup instanced mesh component
	 */
	instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("instance_component"));
	instanced->SetStaticMesh(mesh);
	instanced->bAllowCullDistanceVolume = false;
	instanced->bDisableCollision = true;
	instanced->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//instanced->SetFlags(RF_Transactional);
	instanced->SetMaterial(0, mat);
	instanced->SetMobility(EComponentMobility::Movable);
	this->AddInstanceComponent(instanced);

	instanced->AttachToComponent(root,
		FAttachmentTransformRules::KeepRelativeTransform);
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
	instanced->SetRelativeTransform(data.robot_origin.Inverse() * FTransform(FQuat(FVector(0., 0., 1.), UE_PI / 2.f)));

	TArray<FTransform> temp;
	temp.Reserve(data.indices.Num());

	for (const auto& p : data.indices)
	{
		temp.Add(FTransform(FQuat::Identity,
			p * data.voxel_side_length,
			//Scale is multiplied by 0.01 because the default cube is
			//1 meter in size instead of a centimeter
			FVector(0.01 * data.voxel_side_length)));
	}
	instanced->AddInstances(temp, false);
}

void A_franka_voxel::clear_Implementation()
{
	instanced->ClearInstances();
}

void A_franka_voxel::set_visibility_Implementation(Visual_Change vis_change)
{
	switch (vis_change)
	{
	case ENABLED:
		SetActorHiddenInGame(false);
		break;
	case DISABLED:
		SetActorHiddenInGame(true);
		break;
	case REVOKED:
		clear_Implementation();
		break;
	}
}
