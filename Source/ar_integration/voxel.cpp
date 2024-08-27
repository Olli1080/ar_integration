#include "voxel.h"

#include "Components/InstancedStaticMeshComponent.h"

A_voxel::A_voxel()
{
	PrimaryActorTick.bCanEverTick = true;

	auto root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(root);

	/**
	 * load global mesh and material by engine ref
	 */
	mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'")).Object;

	mat = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/voxel_material.voxel_material'")).Object;

	/**
	 * setup instanced mesh component
	 */
	instanced = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("instance_component"));
	instanced->SetStaticMesh(mesh);
	instanced->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//instanced->SetFlags(RF_Transactional);
	instanced->SetMaterial(0, mat);
	this->AddInstanceComponent(instanced);
}

void A_voxel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/**
	 * consume to_spawn set
	 */
	TSet<FIntVector> swap_spawn;
	{
		std::unique_lock lock(spawn_mtx);
		Swap(swap_spawn, to_spawn);
	}
	/**
	 * spawn voxels as instance
	 */
	for (const auto& p : swap_spawn)
	{
		instanced->AddInstance(FTransform(FQuat::Identity,
			FVector(p) * voxel_size,
			FVector(0.01 * voxel_size)), true);
	}
}

void A_voxel::BeginDestroy()
{
	Super::BeginDestroy();
}

void A_voxel::insert(const TArray<FVector>& points)
{
	init = false;
	
	TSet<FIntVector> temp_new;
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

			temp_new.Add(index);
		}
	}
	std::unique_lock lock(spawn_mtx);
	to_spawn.Append(temp_new);
}

void A_voxel::insert_point(const FVector& point)
{
	init = false;

	/**
	 * calculate index of point
	 * and insert if not present
	 */
	FIntVector index = FIntVector(
		(point + point.GetSignVector() * voxel_size * 0.5f) / voxel_size
	);
	bool exists = false;
	{
		std::unique_lock lock(voxel_mtx);
		voxels.Add(index, &exists);
	}
	if (exists)
		return;

	std::unique_lock lock(spawn_mtx);
	to_spawn.Add(index);
}

void A_voxel::set_voxel_size(float size)
{
	size = std::abs(size);
	if (init && size > 0.f)
		voxel_size = size;
}