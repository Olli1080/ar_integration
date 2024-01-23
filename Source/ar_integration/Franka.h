#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"

#include <cmath>

#include "Franka.generated.h"

USTRUCT(BlueprintType)
struct F_DHParameter
{
	GENERATED_BODY()

	F_DHParameter()
	{
		
	}

	F_DHParameter(double d, double theta, double a, double alpha)
		: d(d), theta(theta), a(a), alpha(alpha)
	{}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Units="Meters"))
	double d = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Meters"))
	double a = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double alpha = 0.0;
	
	FTransform generateDHMatrix(double dD = 0.0, double dTheta = 0.0, double dA = 0.0, double dAlpha = 0.0) const
	{
		/*FTransform conv = FTransform(FMatrix(
			UE::Math::TVector<double>(0, 1, 0),
			UE::Math::TVector<double>(1, 0, 0), 
			UE::Math::TVector<double>(0, 0, 1), 
			UE::Math::TVector<double>(0, 0, 0)));
			*/
		/*auto conv = FTransform(FMatrix(
			UE::Math::TVector<double>(-1, 0, 0),
			UE::Math::TVector<double>(0, 1, 0),
			UE::Math::TVector<double>(0, 0, 1),
			UE::Math::TVector<double>(0, 0, 0)));*/

		double cT = std::cos(theta + dTheta);
		double sT = std::sin(theta + dTheta);

		double cA = std::cos(alpha + dAlpha);
		double sA = std::sin(alpha + dAlpha);

		auto out = FTransform(FMatrix(
			UE::Math::TPlane<double>(cT, -sT, 0, 0),
			UE::Math::TPlane<double>(sT * cA, cT * cA, sA, 0),
			UE::Math::TPlane<double>(-sT * sA, -cT * sA, cA, 0),
			UE::Math::TPlane<double>(-(a + dAlpha) * cT, (a + dAlpha) * sT, (d + dD), 1)));
		return out;
	}
};

USTRUCT(BlueprintType)
struct F_Joint
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double value = 0.0;
};

UENUM(BlueprintType)
enum class EColorFranka : uint8 {
	WHITE		UMETA(DisplayName = "White"),
	DARK		UMETA(DisplayName = "Dark"),
	GREEN		UMETA(DisplayName = "Green"),
	BLUE        UMETA(DisplayName = "Blue"),
	RED			UMETA(DisplayName = "Red"),
	GRAY        UMETA(DisplayName = "Gray"),
	METALLIC    UMETA(DisplayName = "Metallic"),
	SIZE		UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct F_LinkVisual
{
	GENERATED_BODY()

	UPROPERTY()
	EColorFranka color;

	UPROPERTY()
	UStaticMesh* mesh;
};

USTRUCT()
struct F_LinkData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<F_LinkVisual> visuals;
};
/// <summary>
/// //////////////////
/// </summary>
USTRUCT()
struct FRobotMesh
{
	GENERATED_BODY()

	UPROPERTY()
	FString meshPath;

	UPROPERTY()
	EColorFranka materialType;
};

USTRUCT()
struct FBase
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform transform;

	UPROPERTY()
	TArray<FRobotMesh> meshData;
};

USTRUCT()
struct FTCP
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform transform;
};

USTRUCT()
struct FComposite
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform transform;

	UPROPERTY()
	TArray<FRobotMesh> meshData;
};

USTRUCT()
struct FLink
{
	GENERATED_BODY()

	UPROPERTY()
	F_DHParameter DhParameter;

	UPROPERTY()
	TArray<FComposite> composite;
};

USTRUCT()
struct FRobot
{
	GENERATED_BODY()

	UPROPERTY()
	FBase base;

	UPROPERTY()
	TArray<FLink> links;

	UPROPERTY()
	FTCP tcp;
};

USTRUCT(BlueprintType)
struct FFrankaJoints
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_2;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_4;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Units = "Radians"))
	double theta_6;

	double getValue(int idx) const;
};

static FRobot generateFrankaBlueprint();

UCLASS(Blueprintable)
class AR_INTEGRATION_API AFranka : public AActor
{
	GENERATED_BODY()
	
public:
	// Legt Standardwerte für die Eigenschaften von this actorfest
	AFranka();

protected:
	// Wird aufgerufen, wenn das Spiel beginnt oder wenn es gespawnt wird
	virtual void BeginPlay() override;

public:

	static inline FTransform conv = FTransform(FMatrix(
		UE::Math::TPlane<double>{1, 0, 0, 0},
		UE::Math::TPlane<double>{0, 0, -1, 0},
		UE::Math::TPlane<double>{0, 1, 0, 0},
		UE::Math::TPlane<double>{0, 0, 0, 1}));
	// Jeden Frame aufgerufen
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, EditFixedSize)
	TArray<F_DHParameter> parameters;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetJoints)
	FFrankaJoints joints;

	UFUNCTION(BlueprintSetter)
	void SetJoints(const FFrankaJoints& new_joints);

	/*UFUNCTION(BlueprintCallable)
	void SetJoint(int idx, F_Joint value);*/

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<USceneComponent*> linkStructure;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<USceneComponent*> structure;
	
	//UStaticMeshComponent* addMesh(FName name, USceneComponent* parent, UStaticMesh* mesh, UMaterial* material = nullptr);
	USceneComponent* addStructureComponent(FName name);
	USceneComponent* addStructureLink(FName name, FTransform transform = FTransform::Identity);

private:

	UPROPERTY()
	UStaticMesh* base;

	UPROPERTY()
	TArray<F_LinkData> links;

	UPROPERTY()
	USceneComponent* tcpComp;

	UPROPERTY()
	TSubclassOf<AActor> coord_blueprint;

	bool spawned = false;
};
