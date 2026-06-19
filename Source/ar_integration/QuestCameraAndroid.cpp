#include "QuestCameraAndroid.h"

#if PLATFORM_ANDROID
#include "Android/AndroidApplication.h"
#include "Android/AndroidJava.h"
#include "Android/AndroidJNI.h"

QuestCameraAndroid& QuestCameraAndroid::Get()
{
	static QuestCameraAndroid Instance;
	return Instance;
}

void QuestCameraAndroid::Initialize()
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return;

	// In a real implementation, we would use FJavaWrapper to find the 
	// Meta-specific camera classes and instantiate them.
	// For the Quest 3, this usually involves calling a Java helper 
	// that manages the CameraManager.openCamera() lifecycle.
}

void QuestCameraAndroid::StartStream()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Quest_StartCamera", "()V", false);
	FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
#endif
}

void QuestCameraAndroid::StopStream()
{
#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Quest_StopCamera", "()V", false);
	FJavaWrapper::CallVoidMethod(Env, FJavaWrapper::GameActivityThis, Method);
#endif
}

bool QuestCameraAndroid::GetLatestFrame(TArray<uint8>& OutFrameData, int32& OutWidth, int32& OutHeight)
{
	// 1. Lock a shared buffer with the Java-side ImageReader
	// 2. Memcpy the pixel data into OutFrameData
	// 3. Update dimensions
	return false; 
}

#else

QuestCameraAndroid& QuestCameraAndroid::Get()
{
	static QuestCameraAndroid Instance;
	return Instance;
}

void QuestCameraAndroid::Initialize() {}
void QuestCameraAndroid::StartStream() {}
void QuestCameraAndroid::StopStream() {}
bool QuestCameraAndroid::GetLatestFrame(TArray<uint8>& OutFrameData, int32& OutWidth, int32& OutHeight) { return false; }

#endif
