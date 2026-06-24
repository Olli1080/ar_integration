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
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return false;

	static jmethodID Method = FJavaWrapper::FindMethod(Env, FJavaWrapper::GameActivityClassID, "AndroidThunkJava_Quest_GetFrame", "([B[I)Z", false);
	if (!Method) return false;

	jbyteArray jOutBuffer = Env->NewByteArray(640 * 480);
	jintArray jOutDims = Env->NewIntArray(2);

	jboolean bResult = Env->CallBooleanMethod(FJavaWrapper::GameActivityThis, Method, jOutBuffer, jOutDims);
	if (bResult)
	{
		jint dims[2];
		Env->GetIntArrayRegion(jOutDims, 0, 2, dims);
		OutWidth = dims[0];
		OutHeight = dims[1];

		int32 Size = OutWidth * OutHeight;
		OutFrameData.SetNumUninitialized(Size);
		Env->GetByteArrayRegion(jOutBuffer, 0, Size, (jbyte*)OutFrameData.GetData());
	}

	Env->DeleteLocalRef(jOutBuffer);
	Env->DeleteLocalRef(jOutDims);

	return bResult;
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
