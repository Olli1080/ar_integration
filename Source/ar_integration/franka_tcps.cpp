#include "franka_tcps.h"

#include "Components/InstancedStaticMeshComponent.h"

A_franka_tcps::A_franka_tcps()
{
	PrimaryActorTick.bCanEverTick = false;

	/**
	 * load global mesh and material by engine ref
	 */
	mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(
		TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object;

	mat = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("Material'/Game/vox_mat.vox_mat'")).Object;

	auto root = CreateDefaultSubobject<USceneComponent>("root");
	SetRootComponent(root);

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

	instanced->AttachToComponent(root,
		FAttachmentTransformRules::KeepRelativeTransform);
}

void A_franka_tcps::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void A_franka_tcps::BeginDestroy()
{
	Super::BeginDestroy();
}

void A_franka_tcps::set_tcps(const TArray<FVector>& data)
{
	instanced->ClearInstances();
	//instanced->SetRelativeRotation(FQuat(FVector(0., 0., 1.), UE_PI / 2.f));

	for (const auto& p : data)
		instanced->AddInstance(FTransform(
			FQuat::Identity, p, FVector(0.01, 0.01, 0.01)
		));
}

void A_franka_tcps::clear_Implementation()
{
	instanced->ClearInstances();
}

void A_franka_tcps::set_visibility_Implementation(Visual_Change vis_change)
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
