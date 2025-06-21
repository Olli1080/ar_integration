#include "voxel.h"

#include "Components/InstancedStaticMeshComponent.h"

A_voxel::A_voxel()
{
	PrimaryActorTick.bCanEverTick = true;

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
	//instanced->SetMobility(EComponentMobility::Movable);
	this->AddInstanceComponent(instanced);

	instanced->AttachToComponent(root,
		FAttachmentTransformRules::KeepRelativeTransform);
}

void A_voxel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/**
	 * consume to_spawn set
	 */
	TArray<FTransform> swap_spawn;
	{
		std::unique_lock lock(voxel_mtx);
		swap_spawn = all_transforms;
		//Swap(swap_spawn, to_spawn);
	}
	instanced->ClearInstances();
	/**
	 * spawn voxels as instance
	 */
	instanced->AddInstances(swap_spawn, false);
}

void A_voxel::BeginDestroy()
{
	Super::BeginDestroy();
}

void A_voxel::insert(const TArray<FVector>& points)
{
	init = false;
	
	//TSet<FIntVector> temp_new;
	{
		std::unique_lock lock(voxel_mtx);
		for (const auto& p : points)
		{
			if (p.ContainsNaN())
				continue;
			/**
			 * calculate index vector of points
			 * and insert if not present
			 */
			FIntVector index = FIntVector(
				(p + p.GetSignVector() * voxel_size * 0.5f) / voxel_size
			);
			bool exists = false;
			voxels.Add(index, &exists);
			if (exists)
				continue;

			all_transforms.Add(FTransform(FQuat::Identity,
				FVector(index) * voxel_size,
				FVector(0.01 * voxel_size)));

			//temp_new.Add(index);
		}
	}
	//std::unique_lock lock(spawn_mtx);
	//to_spawn.Append(temp_new);
}
//
//void A_voxel::insert_sync(const TArray<FVector>& points)
//{
//	init = false;
//
//	//std::scoped_lock lock{ voxel_mtx, spawn_mtx };
//	TArray<FTransform> instance_transforms;
//	for (const auto& p : points)
//	{
//		if (p.ContainsNaN())
//			continue;
//		/**
//		 * calculate index vector of points
//		 * and insert if not present
//		 */
//		FIntVector index = FIntVector(
//			(p + p.GetSignVector() * voxel_size * 0.5f) / voxel_size
//		);
//		bool exists = false;
//		voxels.Add(index, &exists);
//		if (exists)
//			continue;
//
//		instance_transforms.Emplace(FTransform(FQuat::Identity,
//			FVector(index) * voxel_size,
//			FVector(0.01 * voxel_size)));
//	}
//	instanced->AddInstances(instance_transforms, false, true);
//}
//
//void A_voxel::insert_point(const FVector& point)
//{
//	init = false;
//
//	/**
//	 * calculate index of point
//	 * and insert if not present
//	 */
//	FIntVector index = FIntVector(
//		(point + point.GetSignVector() * voxel_size * 0.5f) / voxel_size
//	);
//	bool exists = false;
//	{
//		std::unique_lock lock(voxel_mtx);
//		voxels.Add(index, &exists);
//	}
//	if (exists)
//		return;
//
//	std::unique_lock lock(spawn_mtx);
//	to_spawn.Add(index);
//}

void A_voxel::set_voxel_size(float size)
{
	size = std::abs(size);
	if (init && size > 0.f)
		voxel_size = size;
}
/*
void A_voxel::clear_all()
{
	std::scoped_lock lock{spawn_mtx, voxel_mtx};

	to_spawn.Empty();
	voxels.Empty();

	instanced->ClearInstances();
	init = true;
}
*/