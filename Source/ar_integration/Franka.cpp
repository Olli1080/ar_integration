#include "Franka.h"
#include "Math/UnrealMathUtility.h"
#include "Math/UnitConversion.h"
#include <string>

static FRobot generateFrankaBlueprint()
{
	FRobot robot;

	auto& base = robot.base;
	auto& links = robot.links;
	auto& tcp = robot.tcp;

	base.transform = FTransform::Identity;
		//FTransform(UE::Math::TQuat<double>(UE::Math::TVector<double>(1, 0, 0), UE_PI / 2.)/*, offset */);
	// FTransform(UE::Math::TQuat<double>(UE::Math::TVector<double>(0, 1, 0), UE_PI)/*, offset */);
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/Base_white.Base_white'"), EColorFranka::WHITE });
	base.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Base_dark.Base_dark'"), EColorFranka::DARK});

	{ //link 1
		FLink link;
		link.DhParameter = { FUnitConversion::Convert<double>(0.333, EUnit::Meters, EUnit::Centimeters), 	0, 	  0,FMath::DegreesToRadians(0), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link1_white.Link1_white'"), EColorFranka::WHITE});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 2
		FLink link;
		link.DhParameter = { 0, 	0, 	  0,    FMath::DegreesToRadians(-90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link2_white.Link2_white'"), EColorFranka::WHITE});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 3
		FLink link;
		link.DhParameter = { FUnitConversion::Convert<double>(0.316, EUnit::Meters, EUnit::Centimeters), 	0,       0,    FMath::DegreesToRadians(90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link3_white.Link3_white'"), EColorFranka::WHITE});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link3_dark.Link3_dark'"), EColorFranka::DARK});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 4
		FLink link;
		link.DhParameter = { 0, 	0,  FUnitConversion::Convert<double>(0.0825, EUnit::Meters, EUnit::Centimeters),    FMath::DegreesToRadians(90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link4_white.Link4_white'"), EColorFranka::WHITE});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link4_dark.Link4_dark'"), EColorFranka::DARK});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 5
		FLink link;
		link.DhParameter = { FUnitConversion::Convert<double>(0.384, EUnit::Meters, EUnit::Centimeters), 	0, FUnitConversion::Convert<double>(-0.0825, EUnit::Meters, EUnit::Centimeters), FMath::DegreesToRadians(-90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link5_white.Link5_white'"), EColorFranka::WHITE});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link5_dark.Link5_dark'"), EColorFranka::DARK});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 6
		FLink link;
		link.DhParameter = { 0, 	0,       0,    FMath::DegreesToRadians(90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link6_white.Link6_white'"), EColorFranka::WHITE});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link6_dark.Link6_dark'"), EColorFranka::DARK });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link6_green.Link6_green'"), EColorFranka::GREEN });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link6_blue.Link6_blue'"), EColorFranka::BLUE });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link6_red.Link6_red'"), EColorFranka::RED });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 7
		FLink link;
		link.DhParameter = { 0, 	0,   FUnitConversion::Convert<double>(0.088, EUnit::Meters, EUnit::Centimeters),    FMath::DegreesToRadians(90), DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link7_with_KMS_white.Link7_with_KMS_white'"), EColorFranka::WHITE });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link7_with_KMS_dark.Link7_with_KMS_dark'"), EColorFranka::DARK });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Link7_with_KMS_metalic.Link7_with_KMS_metalic'"), EColorFranka::METALLIC });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 8
		FLink link;
		link.DhParameter = { FUnitConversion::Convert<double>(0.1714, EUnit::Meters, EUnit::Centimeters), 	FMath::DegreesToRadians(-45),   0.0,    0, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Hand_white.Hand_white'"), EColorFranka::WHITE });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Hand_metalic.Hand_metalic'"), EColorFranka::METALLIC });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 9
		FLink link;
		link.DhParameter = { FUnitConversion::Convert<double>(0.06, EUnit::Meters, EUnit::Centimeters), 	0,   0.0,    0, DHConvention::CRAIGS };
		{
			FComposite comp;
			comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/finger_metalic.finger_metalic'"), EColorFranka::METALLIC });
			comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/finger_rubber.finger_rubber'"), EColorFranka::DARK });
			link.composite.Add(std::move(comp));
		}
		{
			FComposite comp;
			comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/finger_metalic.finger_metalic'"), EColorFranka::METALLIC });
			comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/finger_rubber.finger_rubber'"), EColorFranka::DARK });
			comp.transform = FTransform(UE::Math::TQuat<double>(UE::Math::TVector<double>(0, 1, 0), UE_PI));

			link.composite.Add(std::move(comp));
		}
		links.Add(std::move(link));
	}
	tcp.transform = FTransform(UE::Math::TVector<double>(0, 0, -6));
	tcp.transform.SetScale3D(UE::Math::TVector<double>(0.1));


	return robot;
}

// Standardwerte festlegen
AFranka::AFranka()
{
	// Stellen Sie dies actor so ein, dass Tick() in jedem Frame aufgerufen wird. Sie können dies deaktivieren, um die Leistung zu verbessern, wenn Sie es nicht benötigen.
	PrimaryActorTick.bCanEverTick = true;

	TArray<UMaterial*> materials;

	materials.SetNum(7);
	materials[static_cast<uint8_t>(EColorFranka::WHITE)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_white.phong_white'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::DARK)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_dark.phong_dark'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::BLUE)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_blue.phong_blue'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::GRAY)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_gray.phong_gray'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::GREEN)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_green.phong_green'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::METALLIC)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_metallic.phong_metallic'")).Object;
	materials[static_cast<uint8_t>(EColorFranka::RED)] = ConstructorHelpers::FObjectFinder<UMaterial>(
		TEXT("/Script/Engine.Material'/Game/Franka/phong_red.phong_red'")).Object;


	const auto& blueprint = generateFrankaBlueprint();

	const auto root = addStructureComponent(TEXT("rude"));
	root->SetRelativeTransform(blueprint.base.transform);
	{
		auto tt = CreateDefaultSubobject<USceneComponent>(TEXT("meh"));
		tt->SetupAttachment(root);
		tt->SetRelativeTransform(conv);

		int i = 0;

		for (const auto& [meshPath, materialType] : blueprint.base.meshData)
		{
			const auto& mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(*meshPath).Object;
			const auto& material = materials[static_cast<uint8_t>(materialType)];

			FNameBuilder builder;
			builder.Append("baseMesh[");
			builder.Append(std::to_string(i++).c_str());
			builder.Append("]");

			auto temp = CreateDefaultSubobject<UStaticMeshComponent>(builder.ToString());
			temp->SetupAttachment(tt);

			temp->SetStaticMesh(mesh);
			temp->SetMaterial(0, material);

			//meshes.Add(std::move(temp));
		}
	}
	{
		int i = 0;
		for (const auto& [DhParameter, composite] : blueprint.links)
		{
			FNameBuilder builder;
			builder.Append("link[");
			builder.Append(std::to_string(i++).c_str());
			builder.Append("]");

			const auto linkComponent = addStructureComponent(builder.ToString());
			linkComponent->SetRelativeTransform(DhParameter.generateDHMatrix());

			linkStructure.Add(linkComponent);
			parameters.Add(DhParameter);

			int j = 0;
			for (const auto& [transform, meshData] : composite)
			{
				FNameBuilder builderComp;
				builderComp.Append("com[");
				builderComp.Append((std::to_string(i) + ":" + std::to_string(j++)).c_str());
				builderComp.Append("]");

				const auto comp = CreateDefaultSubobject<USceneComponent>(builderComp.ToString());
				comp->SetupAttachment(linkComponent);
				comp->SetRelativeTransform(transform * conv);
				//comp->SetRelativeTransform(transform);

				int k = 0;
				for (const auto& [meshPath, materialType] : meshData)
				{
					const auto& mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(*meshPath).Object;
					const auto& material = materials[static_cast<uint8_t>(materialType)];

					FNameBuilder builderMesh;
					builderMesh.Append("mesh[");
					builderMesh.Append((std::to_string(i) + ":" + std::to_string(j) + ":" + std::to_string(k++)).c_str());
					builderMesh.Append("]");

					const auto temp = CreateDefaultSubobject<UStaticMeshComponent>(builderMesh.ToString());
					temp->SetupAttachment(comp);

					temp->SetStaticMesh(mesh);
					temp->SetMaterial(0, material);
					//temp->SetRelativeTransform(conv);

					//meshes.Add(std::move(temp));
				}
			}
		}
	}
	tcpComp = CreateDefaultSubobject<USceneComponent>(TEXT("tcp"));
	tcpComp->SetupAttachment(structure.Last());
	tcpComp->SetRelativeTransform(blueprint.tcp.transform);

	coord_blueprint = ConstructorHelpers::FObjectFinder<UBlueprint>(TEXT("/Script/Engine.Blueprint'/Game/coord_frame.coord_frame'")).Object->GeneratedClass;
	//blueprint.tcp.transform
}

// Wird aufgerufen, wenn das Spiel beginnt oder wenn es gespawnt wird
void AFranka::BeginPlay()
{
	Super::BeginPlay();

	if (spawned)
		return;
	spawned = true;

	FActorSpawnParameters params;
	params.bNoFail = true;
	
	auto sth = GetWorld()->SpawnActor<AActor>(coord_blueprint, params);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	sth->AttachToComponent(tcpComp, rules);
}

// Jeden Frame aufgerufen
void AFranka::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFranka::SetJoints(const FFrankaJoints& new_joints)
{
	this->joints = new_joints;

	for (int i = 0; i < 7; ++i)
		linkStructure[i]->SetRelativeTransform(parameters[i].generateDHMatrix(0.0, joints.getValue(i)));
}
#if WITH_EDITOR

void AFranka::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	/*if (PropertyChangedEvent.MemberProperty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Property of member changed [name]: %s, index: %i"), *PropertyChangedEvent.MemberProperty->GetFName().ToString(), PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.GetPropertyName().ToString()))
		
	}

	if (PropertyChangedEvent.Property)
	{
		UE_LOG(LogTemp, Warning, TEXT("Property changed [name]: %s, index: %i"), *PropertyChangedEvent.Property->GetOwnerProperty()->GetFName().ToString(), PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.Property->GetOwnerProperty()->GetFName().ToString()))
	}*/
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void AFranka::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("Meow: %s"), );

	auto node = PropertyChangedEvent.PropertyChain.GetActiveMemberNode();
	if (node)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Works %s"), *node->GetValue()->GetFName().ToString());
		auto prop = node->GetValue();
		auto memberProp = PropertyChangedEvent.GetMemberPropertyName().ToString();
		if (prop->GetFName().ToString().Equals(FString(L"joints")) && memberProp.StartsWith("theta_"))
		{
			memberProp.RemoveFromStart("theta_");
			int index = FCString::Atoi(*memberProp);
			linkStructure[index]->SetRelativeTransform(parameters[index].generateDHMatrix(0.0, joints.getValue(index)));

		//	int index = PropertyChangedEvent.GetArrayIndex(prop->GetFName().ToString());
			 
		//	linkStructure[index]->SetRelativeTransform(parameters[index].generateDHMatrix(0.0, editor_joints[index].value).Inverse());

			//UE_LOG(LogTemp, Warning, TEXT("Works: %i"), index);
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Property changed [name]: %s, index: %i"), *PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName().ToString(), PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue()->GetFName().ToString()));
	//UE_LOG(LogTemp, Warning, TEXT("MEH: %s, index: %i"), *PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetNextNode()->GetValue()->GetFName().ToString(), PropertyChangedEvent.GetArrayIndex(PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetNextNode()->GetValue()->GetFName().ToString()));

	//UE_LOG(LogTemp, Warning, TEXT("MEH: %s, index: %i"), *PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetPrevNode()->GetValue()->GetFName().ToString(), 1000);

	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}
#endif
/*
UStaticMeshComponent* AFranka::addMesh(FName name, USceneComponent* parent, UStaticMesh* mesh, UMaterial* material)
{
	auto temp = CreateDefaultSubobject<UStaticMeshComponent>(name);
	temp->SetupAttachment(parent);
	temp->SetStaticMesh(mesh);

	temp->SetMaterial(0, material);

	temp->SetRelativeTransform(conv);
	meshes.Add(std::move(temp));

	return meshes.Last();
}
*/
USceneComponent* AFranka::addStructureComponent(FName name)
{
	auto temp = CreateDefaultSubobject<USceneComponent>(name);
	if (structure.Num() == 0)
		SetRootComponent(temp);
	else
		temp->SetupAttachment(structure.Last());
	structure.Add(temp);
	return temp;
}

USceneComponent* AFranka::addStructureLink(FName name, FTransform transform)
{
	auto current = addStructureComponent(name);
	current->SetRelativeTransform(parameters[linkStructure.Num()].generateDHMatrix());
	//current->SetRelativeTransform(transform);

	if (linkStructure.Num() == 0)
		current->SetupAttachment(GetRootComponent());
	else
		current->SetupAttachment(linkStructure.Last());
	linkStructure.Add(current);
	return current;
}

double FFrankaJoints::getValue(int idx) const
{
	switch (idx)
	{
	case 0:
		return theta_0;
	case 1:
		return theta_1;
	case 2:
		return theta_2;
	case 3:
		return theta_3;
	case 4:
		return theta_4;
	case 5:
		return theta_5;
	case 6:
		return theta_6;
	}
	return 0;
	//TODO::
}
