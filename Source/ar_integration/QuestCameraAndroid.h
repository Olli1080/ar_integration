#pragma once

#include "CoreMinimal.h"

#if PLATFORM_ANDROID
#include <jni.h>
#include "Android/AndroidJava.h"
#include "Android/AndroidApplication.h"
#endif

/**
 * @class QuestCameraAndroid
 * Low-level JNI bridge for accessing the Quest 3 raw camera stream via Camera2 API.
 */
class QuestCameraAndroid
{
public:
	static QuestCameraAndroid& Get();

	void Initialize();
	void StartStream();
	void StopStream();

	/**
	 * Returns the latest frame as a raw byte array.
	 * Expected format: YUV_420_888 or RGBA.
	 */
	bool GetLatestFrame(TArray<uint8>& OutFrameData, int32& OutWidth, int32& OutHeight);

private:
	QuestCameraAndroid() = default;
	
#if PLATFORM_ANDROID
	jobject JavaCameraInstance = nullptr;
#endif
};
