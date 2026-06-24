#include "QuestRegistrationManager.h"
#include "IntegrationGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TransformHelper.h"

AQuestRegistrationManager::AQuestRegistrationManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AQuestRegistrationManager::BeginPlay()
{
	Super::BeginPlay();
}

void AQuestRegistrationManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDetecting)
	{
		ProcessCameraFrame();
	}
}

void AQuestRegistrationManager::StartDetection()
{
	bIsDetecting = true;
}

void AQuestRegistrationManager::StopDetection()
{
	bIsDetecting = false;
}

#include "QuestCameraAndroid.h"
#pragma push_macro("check")
#undef check

// Avoid conflict with OpenCV's int64/uint64 typedefs on Android/Clang
#pragma push_macro("int64")
#pragma push_macro("uint64")
#undef int64
#undef uint64
#define int64 opencv_int64
#define uint64 opencv_uint64

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco.hpp>

#pragma pop_macro("uint64")
#pragma pop_macro("int64")
#pragma pop_macro("check")

void AQuestRegistrationManager::ProcessCameraFrame()
{
	TArray<uint8> FrameData;
	int32 Width, Height;

	if (QuestCameraAndroid::Get().GetLatestFrame(FrameData, Width, Height))
	{
		// 1. Wrap FrameData in a cv::Mat (Quest frames are usually RGBA or YUV)
		cv::Mat img(Height, Width, CV_8UC4, FrameData.GetData());
		
		// 2. Run OpenCV Aruco detection
		cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
		cv::aruco::DetectorParameters parameters;
		
		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f>> corners;
		std::vector<std::vector<cv::Point2f>> rejected;
		cv::aruco::ArucoDetector detector(dictionary, parameters);
		detector.detectMarkers(img, corners, ids, rejected);
		
		if (ids.size() > 0) {
			// Marker found! Use the first detected marker for registration.
			// MarkerSize is in meters (e.g., 0.1f for 10cm)
			
			// We need camera intrinsics for pose estimation. 
			// These can be obtained from the Quest Camera API via JNI.
			cv::Mat camMatrix = cv::Mat::eye(3, 3, CV_64F); // Placeholder
			cv::Mat distCoeffs = cv::Mat::zeros(8, 1, CV_64F); // Placeholder
			
			// Define marker corners in 3D (MarkerSize is the side length)
			float halfSize = MarkerSize / 2.0f;
			std::vector<cv::Point3f> objPoints = {
				cv::Point3f(-halfSize, halfSize, 0),
				cv::Point3f(halfSize, halfSize, 0),
				cv::Point3f(halfSize, -halfSize, 0),
				cv::Point3f(-halfSize, -halfSize, 0)
			};
			
			cv::Mat rvec, tvec;
			cv::solvePnP(objPoints, corners[0], camMatrix, distCoeffs, rvec, tvec);
			
			FVector R(rvec.at<double>(0), rvec.at<double>(1), rvec.at<double>(2));
			FVector T(tvec.at<double>(0), tvec.at<double>(1), tvec.at<double>(2));
			
			FTransform MarkerTransform = OpenCVToUnreal(R, T);
			UpdateWorldAnchor(MarkerTransform);
			StopDetection();
		}
	}
}

FTransform AQuestRegistrationManager::OpenCVToUnreal(const FVector& RotationVector, const FVector& TranslationVector)
{
	using namespace Transformation;
	
	static TransformationConverterWrapper Conv(
		TransformationMetaBuilder()
			.right({ Axis::X, AxisDirection::POSITIVE })
			.forward({ Axis::Z, AxisDirection::POSITIVE })
			.up({ Axis::Y, AxisDirection::NEGATIVE })
			.scale(1, 1) // Meters
			.build(),
		UnrealMeta()
	);

	// Convert Rodrigues to Quat (OpenCV space)
	float Angle = RotationVector.Size();
	FVector Axis = RotationVector / Angle;
	FQuat OpenCVQuat(Axis, Angle);
	
	// Convert using the library
	FVector UnrealTranslation = Conv.convert_point(TranslationVector);
	FQuat UnrealQuat = Conv.convert_quaternion(OpenCVQuat);
	
	return FTransform(UnrealQuat, UnrealTranslation);
}

void AQuestRegistrationManager::UpdateWorldAnchor(const FTransform& MarkerTransform)
{
	auto* GameState = Cast<AIntegrationGameState>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->update_anchor_transform(MarkerTransform);
		OnRegistrationComplete.Broadcast();
	}
}
