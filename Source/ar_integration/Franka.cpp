#include "Franka.h"

#include <string>
#include <cmath>

#include "TransformHelper.h"

/**
 * @brief Staticmesh conversion
 */
static inline FTransform conv = FTransform(FMatrix(
	UE::Math::TPlane<double>{0, 1, 0, 0},
	UE::Math::TPlane<double>{0, 0, 1, 0},
	UE::Math::TPlane<double>{1, 0, 0, 0},
	UE::Math::TPlane<double>{0, 0, 0, 1}));

static inline Transformation::TransformationMeta DHMeta(
	{ Transformation::Axis::X, Transformation::AxisDirection::POSITIVE },
	{ Transformation::Axis::Y, Transformation::AxisDirection::POSITIVE },
	{ Transformation::Axis::Z, Transformation::AxisDirection::POSITIVE }
);

static inline Transformation::TransformationConverter dh_2_UE{ DHMeta, Transformation::UnrealMeta };

static inline float PI_2 = UE_PI / 2.f;

static FRobot generateFrankaBlueprint()
{
	FRobot robot;

	auto& base = robot.base;
	auto& links = robot.links;
	auto& tcp = robot.tcp;

	base.transform = FTransform(FQuat(FVector(0., 0., 1.), PI_2));
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node0_Part__Feature017_001.node0_Part__Feature017_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Part__Feature017_001.Part__Feature017_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node10_Shell009_001.node10_Shell009_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell009_001.Shell009_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node11_Shell010_001.node11_Shell010_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell010_001.Shell010_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node1_Part__Feature022_001.node1_Part__Feature022_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Part__Feature022_001.Part__Feature022_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node2_Part__Feature023_001.node2_Part__Feature023_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Part__Feature023_001.Part__Feature023_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node3_Part__Feature019_001.node3_Part__Feature019_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Part__Feature019_001.Part__Feature019_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node4_Part__Feature018_001.node4_Part__Feature018_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Part__Feature018_001.Part__Feature018_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node5_Face636_001.node5_Face636_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Face636_001.Face636_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node6_Shell001_001.node6_Shell001_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell001_001.Shell001_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node7_Shell002_001.node7_Shell002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell002_001.Shell002_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node8_Shell_001.node8_Shell_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell_001.Shell_001'") });
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link0/node9_Shell003_001.node9_Shell003_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link0/Shell003_001.Shell003_001'") });

	/*
	base.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/Base_white.Base_white'"), TEXT("/Script/Engine.Material'/Game/Franka/phong_white.phong_white'") });
	base.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Base_dark.Base_dark'"), TEXT("/Script/Engine.Material'/Game/Franka/phong_dark.phong_dark'")});
	*/
	{ //link 1
		FLink link;
		link.DhParameter = { 0.333, 0, 0, 0, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link1/link1.link1'"), TEXT("/Script/Engine.Material'/Game/Franka/link1/Part__Feature_001.Part__Feature_001'")});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 2
		FLink link;
		link.DhParameter = { 0, 0, 0, -PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link2/link2.link2'"), TEXT("/Script/Engine.Material'/Game/Franka/link2/Part__Feature024.Part__Feature024'")});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 3
		FLink link;
		link.DhParameter = { 0.316, 0, 0, PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link3/node0_001_Part__Feature_001_001_001_002.node0_001_Part__Feature_001_001_001_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link3/Part__Feature_001_001_001_002.Part__Feature_001_001_001_002'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link3/node1_Part__Feature001_010_001_002.node1_Part__Feature001_010_001_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link3/Part__Feature001_010_001_002.Part__Feature001_010_001_002'")});
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link3/node2_Part__Feature002_007_001_002.node2_Part__Feature002_007_001_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link3/Part__Feature002_007_001_002.Part__Feature002_007_001_002'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link3/node3_Part__Feature003_004_001_002.node3_Part__Feature003_004_001_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link3/Part__Feature003_004_001_002.Part__Feature003_004_001_002'") });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 4
		FLink link;
		link.DhParameter = { 0, 0, 0.0825, PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link4/node0_Part__Feature_002_003_001.node0_Part__Feature_002_003_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link4/Part__Feature_002_003_001.Part__Feature_002_003_001'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link4/node1_Part__Feature002_001_003_001.node1_Part__Feature002_001_003_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link4/Part__Feature002_001_003_001.Part__Feature002_001_003_001'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link4/node2_Part__Feature003_001_003_001.node2_Part__Feature003_001_003_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link4/Part__Feature003_001_003_001.Part__Feature003_001_003_001'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link4/node3_Part__Feature001_001_003_001.node3_Part__Feature001_001_003_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link4/Part__Feature001_001_003_001.Part__Feature001_001_003_001'")});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 5
		FLink link;
		link.DhParameter = { 0.384, 0, -0.0825, -PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link5/node0_Part__Feature_002_004_003.node0_Part__Feature_002_004_003'"), TEXT("/Script/Engine.Material'/Game/Franka/link5/Part__Feature_002_004_003.Part__Feature_002_004_003'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link5/node1_Shell_001_001_003.node1_Shell_001_001_003'"), TEXT("/Script/Engine.Material'/Game/Franka/link5/Shell_001_001_003.Shell_001_001_003'")});
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/link5/node2_Shell001_001_001_003.node2_Shell001_001_001_003'"), TEXT("/Script/Engine.Material'/Game/Franka/link5/Shell001_001_001_003.Shell001_001_001_003'")});
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 6
		FLink link;
		link.DhParameter = { 0, 0, 0, PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node0_001_Shell006_003_002_001.node0_001_Shell006_003_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell006_003_002_001.Shell006_003_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node0_Shell012_002_002_001.node0_Shell012_002_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell012_002_002_001.Shell012_002_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node10_Shell002_002_001_002_001.node10_Shell002_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell002_002_001_002_001.Shell002_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node11_Shell003_002_001_002_001.node11_Shell003_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell003_002_001_002_001.Shell003_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node12_Shell004_001_001_002_001.node12_Shell004_001_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell004_001_001_002_001.Shell004_001_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node13_Shell005_001_001_002_001.node13_Shell005_001_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell005_001_001_002_001.Shell005_001_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node15_Shell007_002_002_001.node15_Shell007_002_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell007_002_002_001.Shell007_002_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node16_Shell011_002_002_001.node16_Shell011_002_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell011_002_002_001.Shell011_002_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node1_Part__Feature001_009_001_002_001.node1_Part__Feature001_009_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Part__Feature001_009_001_002_001.Part__Feature001_009_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node2_Part__Feature002_006_001_002_001.node2_Part__Feature002_006_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Part__Feature002_006_001_002_001.Part__Feature002_006_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node3_Face064_002_001_002_001.node3_Face064_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Face064_002_001_002_001.Face064_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node4_Face065_002_001_002_001.node4_Face065_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Face065_002_001_002_001.Face065_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node5_Face374_002_001_002_001.node5_Face374_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Face374_002_001_002_001.Face374_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node6_Shell_003_001_002_001.node6_Shell_003_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Shell_003_001_002_001.Shell_003_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node7_Face539_002_001_002_001.node7_Face539_002_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Face539_002_001_002_001.Face539_002_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node8_Union_001_001_002_001.node8_Union_001_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Union_001_001_002_001.Union_001_001_002_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link6/node9_Union001_001_001_002_001.node9_Union001_001_001_002_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link6/Union001_001_001_002_001.Union001_001_001_002_001'") });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 7
		FLink link;
		link.DhParameter = { 0, 0, 0.088, PI_2, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node0_Part__Mirroring_004_001.node0_Part__Mirroring_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring_004_001.Part__Mirroring_004_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node1_Part__Mirroring001_004_002.node1_Part__Mirroring001_004_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring001_004_002.Part__Mirroring001_004_002'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node2_Part__Mirroring002_004_001.node2_Part__Mirroring002_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring002_004_001.Part__Mirroring002_004_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node3_Part__Mirroring003_004_001.node3_Part__Mirroring003_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring003_004_001.Part__Mirroring003_004_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node4_Part__Mirroring004_004_002.node4_Part__Mirroring004_004_002'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring004_004_002.Part__Mirroring004_004_002'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node5_Part__Mirroring005_004_001.node5_Part__Mirroring005_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring005_004_001.Part__Mirroring005_004_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node6_Part__Mirroring006_004_001.node6_Part__Mirroring006_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring006_004_001.Part__Mirroring006_004_001'") });
		comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/link7/node7_Part__Mirroring007_004_001.node7_Part__Mirroring007_004_001'"), TEXT("/Script/Engine.Material'/Game/Franka/link7/Part__Mirroring007_004_001.Part__Mirroring007_004_001'") });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{
		FLink link;
		link.DhParameter = { 0.105, 0, 0., 0, DHConvention::CRAIGS };
		FComposite comp;
		//comp.meshData.Add({ TEXT("/Script/Engine.StaticMesh'/Game/Franka/cobot_pump.cobot_pump'"), TEXT("/Script/Engine.Material'/Game/Franka/phong_white.phong_white'") });

		for (size_t i = 0; i < 9; ++i)
		{
			FString number = FString::FromInt(i);
			comp.meshData.Add({
				FString("/Script/Engine.StaticMesh'/Game/Franka/cobot_pump/pump_Mtl" + number + ".pump_Mtl" + number + "'"),
				FString("/Script/Engine.Material'/Game/Franka/cobot_pump/Mtl" + number + ".Mtl" + number + "'")
			});
		}

		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	/*
	{ //link 8
		FLink link;
		link.DhParameter = { 0.1714, -PI_2 / 2.f, 0.0, 0, DHConvention::CRAIGS };
		FComposite comp;
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Hand_white.Hand_white'"), EColorFranka::WHITE });
		comp.meshData.Add({TEXT("/Script/Engine.StaticMesh'/Game/Franka/Hand_metalic.Hand_metalic'"), EColorFranka::METALLIC });
		link.composite.Add(std::move(comp));
		links.Add(std::move(link));
	}
	{ //link 9
		FLink link;
		link.DhParameter = { 0.06, 0, 0.0, 0, DHConvention::CRAIGS };
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
	tcp.transform.SetScale3D(UE::Math::TVector<double>(0.1));*/


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

		for (const auto& [meshPath, materialPath] : blueprint.base.meshData)
		{
			const auto& mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(*meshPath).Object;
			const auto& material = ConstructorHelpers::FObjectFinder<UMaterial>(*materialPath).Object; //materials[static_cast<uint8_t>(materialType)];

			FNameBuilder builder;
			builder.Append("baseMesh[");
			builder.Append(std::to_string(i++).c_str());
			builder.Append("]");

			auto temp = CreateDefaultSubobject<UStaticMeshComponent>(builder.ToString());
			temp->SetupAttachment(tt);

			temp->SetStaticMesh(mesh);
			temp->SetMaterial(0, material);
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

				int k = 0;
				for (const auto& [meshPath, materialPath] : meshData)
				{
					const auto& mesh = ConstructorHelpers::FObjectFinder<UStaticMesh>(*meshPath).Object;
					const auto& material = ConstructorHelpers::FObjectFinder<UMaterial>(*materialPath).Object; //materials[static_cast<uint8_t>(materialType)];

					FNameBuilder builderMesh;
					builderMesh.Append("mesh[");
					builderMesh.Append((std::to_string(i) + ":" + std::to_string(j) + ":" + std::to_string(k++)).c_str());
					builderMesh.Append("]");

					const auto temp = CreateDefaultSubobject<UStaticMeshComponent>(builderMesh.ToString());
					temp->SetupAttachment(comp);

					temp->SetStaticMesh(mesh);
					temp->SetMaterial(0, material);
				}
			}
		}
	}
	tcpComp = CreateDefaultSubobject<USceneComponent>(TEXT("tcp"));
	tcpComp->SetupAttachment(structure.Last());
	tcpComp->SetRelativeTransform(blueprint.tcp.transform);

#if WITH_COORD
	const auto find_gen_class = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Script/Engine.Blueprint'/Game/coord_frame_2.coord_frame_2_C'"));
	if (!find_gen_class)
		return;
	const auto gen_class = Cast<UBlueprintGeneratedClass>(find_gen_class);
	if (!gen_class)
		return;
	coord_blueprint = gen_class;
#endif
}

// Wird aufgerufen, wenn das Spiel beginnt oder wenn es gespawnt wird
void AFranka::BeginPlay()
{
	Super::BeginPlay();

#if WITH_COORD
	if (!coord_blueprint)
		return;

	if (spawned)
		return;
	spawned = true;

	FActorSpawnParameters params;
	params.bNoFail = true;
	
	auto sth = GetWorld()->SpawnActor<AActor>(coord_blueprint, params);

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
	sth->AttachToComponent(tcpComp, rules);

	for (auto& comp : structure)
	{
		auto sth0 = GetWorld()->SpawnActor<AActor>(coord_blueprint, params);
		sth0->AttachToComponent(comp, rules);
		sth0->SetActorScale3D(FVector{ 0.2 });
	}
#endif
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
		}
	}

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

F_DHParameter::F_DHParameter(double d, double theta, double a, double alpha, DHConvention convention)
	: d(d), theta(theta), a(a), alpha(alpha), convention(convention)
{}

FTransform F_DHParameter::generateDHMatrix(double dD, double dTheta) const
{
	double cT = std::cos(theta + dTheta);
	double sT = std::sin(theta + dTheta);

	double cA = std::cos(alpha);
	double sA = std::sin(alpha);

	FTransform inter;

	if (convention == DHConvention::CLASSIC)
	{
		//[row][column]
		inter = FTransform{ FMatrix
		{
			{cT, -sT * cA, sT * sA, a * cT},
			{sT, cT * cA, -cT * sA, a * sT},
			{0.f, sA, cA, d + dD},
			{0.f, 0.f, 0.f, 1.f}
		}.GetTransposed() };
	}
	else
	{
		inter = FTransform{ FMatrix
		{
			{cT, -sT, 0.f, a},
			{sT * cA, cT * cA, -sA, -(d + dD) * sA},
			{sT * sA, cT * sA, cA, (d + dD) * cA},
			{0.f, 0.f, 0.f, 1.f}
		}.GetTransposed() };
	}
	return dh_2_UE.convert_matrix(inter);
}