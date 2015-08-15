// Copyright by Michael Zeilfelder
//
// Code was originally based on work from Patryk Nadrowski's "Chernobyl Game Engine"
// But I reduced it to the minimum nececcary for my project and then did some more changes.
// So not much left of the original code. That code was also under the zlib license.
//
// TODO: Don't know yet how the JNI and Android combination exactly works. It might be possible all the constant getting and releasing of classes and JNIEnv is not necessary.
// 		 But have to find someone who knows about this or do a lot of testing with different Android application states (and in my experience different Android devices
//		 as they often tend to behave not as similar as one would expect). So for now going the save and slow way and just avoiding to call JNI functions too often.
// 		 And yes - it all looks horrible. I just hope one day google wakes up and realizes that a multi-billion-dollar company forcing thousands of poor NDK developers to write
//       shitty JNI wrappers for strategic reasons is very evil. I don't want to touch Android again after this, coding JNI makes me want to kill myself far too much.

#include "billing.h"

#ifdef _IRR_ANDROID_PLATFORM_
#include <android_native_app_glue.h>
#include <android/log.h>	// for the occasional debugging, style: __android_log_print(ANDROID_LOG_VERBOSE, "Irrlicht", "%s\n", "We do log");

#if 1	// debug
	#define CHECK_JNI(obj) if (!obj) {__android_log_print(ANDROID_LOG_VERBOSE, "Irrlicht", "%s:%d\n", __FILE__, __LINE__); }
#else	// release
	#define CHECK_JNI(obj)
#endif

/* InAppBilling */

InAppBilling::InAppBilling(android_app& androidApp)
	: mAndroidApp(androidApp)
{
}

InAppBilling::~InAppBilling()
{
}

void InAppBilling::init()
{
	// TODO: Use some config
	registerExpectedProduct("full_test");
}

void InAppBilling::requestServerConnnection()
{
	JNIEnv* jniEnv = getJNIEnv();

	jobject objectActivity = mAndroidApp.activity->clazz;
	CHECK_JNI(objectActivity)
	jclass classActivity = jniEnv->GetObjectClass(objectActivity);
	CHECK_JNI(classActivity)
	jmethodID methodGetInAppHelper = jniEnv->GetMethodID(classActivity, "getBillingHelper", "()Lcom/irrgheist/hcraft_championship/BillingHelper;");
	CHECK_JNI(methodGetInAppHelper)

	jobject objectInAppHelper = jniEnv->CallObjectMethod(objectActivity, methodGetInAppHelper);
	CHECK_JNI(objectInAppHelper)
	jclass classInAppHelper = jniEnv->GetObjectClass(objectInAppHelper);
	CHECK_JNI(classInAppHelper)

	jmethodID methodRequestSynchronize = jniEnv->GetMethodID(classInAppHelper, "requestServerConnection", "()V");
	CHECK_JNI(methodRequestSynchronize)
	jniEnv->CallVoidMethod(objectInAppHelper, methodRequestSynchronize);

	jniEnv->DeleteLocalRef(classInAppHelper);
	jniEnv->DeleteLocalRef(objectInAppHelper);
	jniEnv->DeleteLocalRef(classActivity);

	dropJNIEnv();
}

void InAppBilling::requestPurchase(const std::string& articleName)
{
	JNIEnv* jniEnv = getJNIEnv();

	jobject objectActivity = mAndroidApp.activity->clazz;
	CHECK_JNI(objectActivity)
	jclass classActivity = jniEnv->GetObjectClass(objectActivity);
	CHECK_JNI(classActivity)
	jmethodID methodGetInAppHelper = jniEnv->GetMethodID(classActivity, "getBillingHelper", "()Lcom/irrgheist/hcraft_championship/BillingHelper;");
	CHECK_JNI(methodGetInAppHelper)

	jobject objectInAppHelper = jniEnv->CallObjectMethod(objectActivity, methodGetInAppHelper);
	CHECK_JNI(objectInAppHelper)
	jclass classInAppHelper = jniEnv->GetObjectClass(objectInAppHelper);
	CHECK_JNI(classInAppHelper)
	jmethodID methodRequestPurchase = jniEnv->GetMethodID(classInAppHelper, "requestPurchase", "(Ljava/lang/String;)V");
	CHECK_JNI(methodRequestPurchase)

	jstring tName = jniEnv->NewStringUTF(articleName.c_str());
	CHECK_JNI(tName)
	jboolean tConsumable = JNI_FALSE;

	jniEnv->CallVoidMethod(objectInAppHelper, methodRequestPurchase, tName, tConsumable);

	jniEnv->DeleteLocalRef(tName);
	jniEnv->DeleteLocalRef(classInAppHelper);
	jniEnv->DeleteLocalRef(objectInAppHelper);
	jniEnv->DeleteLocalRef(classActivity);

	dropJNIEnv();
}

int InAppBilling::getSynchronizeState()
{
	return getIntMethod("getSynchronizeState");
}

int InAppBilling::getNumAvailableItems()
{
	return getIntMethod("getNumAvailableItems");
}

std::string InAppBilling::getItemName(int index)
{
	return getIndexedStringMethod("getItemName", index);
}

std::string InAppBilling::getItemPrice(int index)
{
	return getIndexedStringMethod("getItemPrice", index);
}

int InAppBilling::getNumPurchasedItems()
{
	return getIntMethod("getNumPurchasedItems");
}

std::string InAppBilling::getPurchasedItem(int index)
{
	return getIndexedStringMethod("getItemPrice", index);
}

void InAppBilling::registerExpectedProduct(const char* articleName)
{
	if ( articleName && articleName[0] )
	{
		JNIEnv* jniEnv = getJNIEnv();

		jobject objectActivity = mAndroidApp.activity->clazz;
		CHECK_JNI(objectActivity)
		jclass classActivity = jniEnv->GetObjectClass(objectActivity);
		CHECK_JNI(classActivity)
		jmethodID methodGetInAppHelper = jniEnv->GetMethodID(classActivity, "getBillingHelper", "()Lcom/irrgheist/hcraft_championship/BillingHelper;");
		CHECK_JNI(methodGetInAppHelper)

		jobject objectInAppHelper = jniEnv->CallObjectMethod(objectActivity, methodGetInAppHelper);
		CHECK_JNI(objectInAppHelper)
		jclass classInAppHelper = jniEnv->GetObjectClass(objectInAppHelper);
		CHECK_JNI(classInAppHelper)
		jmethodID tMethodRegisterExpectedObject = jniEnv->GetMethodID(classInAppHelper, "registerExpectedProduct", "(Ljava/lang/String;)V");
		CHECK_JNI(tMethodRegisterExpectedObject)

		jstring tName = jniEnv->NewStringUTF(articleName);
		CHECK_JNI(tName)
		jboolean tConsumable = JNI_FALSE;
		jniEnv->CallVoidMethod(objectInAppHelper, tMethodRegisterExpectedObject, tName, tConsumable);
		jniEnv->DeleteLocalRef(tName);
	}
}

int InAppBilling::getIntMethod(const char *methodName)
{
	int result = -1;
	JNIEnv* jniEnv = getJNIEnv();

	jobject objectActivity = mAndroidApp.activity->clazz;
	CHECK_JNI(objectActivity)
	jclass classActivity = jniEnv->GetObjectClass(objectActivity);
	CHECK_JNI(classActivity)
	jmethodID methodGetInAppHelper = jniEnv->GetMethodID(classActivity, "getBillingHelper", "()Lcom/irrgheist/hcraft_championship/BillingHelper;");
	CHECK_JNI(methodGetInAppHelper)

	jobject objectInAppHelper = jniEnv->CallObjectMethod(objectActivity, methodGetInAppHelper);
	CHECK_JNI(objectInAppHelper)
	jclass classInAppHelper = jniEnv->GetObjectClass(objectInAppHelper);
	CHECK_JNI(classInAppHelper)

	jmethodID method = jniEnv->GetMethodID(classInAppHelper, methodName, "()I");
	CHECK_JNI(method)
	result = jniEnv->CallIntMethod(objectInAppHelper, method);

	jniEnv->DeleteLocalRef(classInAppHelper);
	jniEnv->DeleteLocalRef(objectInAppHelper);
	jniEnv->DeleteLocalRef(classActivity);

	dropJNIEnv();

	return result;
}

std::string InAppBilling::getIndexedStringMethod(const char *methodName, int index)
{
	JNIEnv* jniEnv = getJNIEnv();

	jobject objectActivity = mAndroidApp.activity->clazz;
	CHECK_JNI(objectActivity)
	jclass classActivity = jniEnv->GetObjectClass(objectActivity);
	CHECK_JNI(classActivity)
	jmethodID methodGetInAppHelper = jniEnv->GetMethodID(classActivity, "getBillingHelper", "()Lcom/irrgheist/hcraft_championship/BillingHelper;");
	CHECK_JNI(methodGetInAppHelper)

	jobject objectInAppHelper = jniEnv->CallObjectMethod(objectActivity, methodGetInAppHelper);
	CHECK_JNI(objectInAppHelper)
	jclass classInAppHelper = jniEnv->GetObjectClass(objectInAppHelper);
	CHECK_JNI(classInAppHelper)

	jmethodID method = jniEnv->GetMethodID(classInAppHelper, methodName, "(I)Ljava/lang/String;");
	CHECK_JNI(method)
	jstring s = (jstring) jniEnv->CallObjectMethod(objectInAppHelper, method, index);
	const char * str = jniEnv->GetStringUTFChars(s, 0);
	std::string result(str);
    jniEnv->ReleaseStringUTFChars(s, str);

	jniEnv->DeleteLocalRef(classInAppHelper);
	jniEnv->DeleteLocalRef(objectInAppHelper);
	jniEnv->DeleteLocalRef(classActivity);

	dropJNIEnv();

	return result;
}

JNIEnv* InAppBilling::getJNIEnv()
{
	JNIEnv* jni = 0;
	mAndroidApp.activity->vm->AttachCurrentThread(&jni, NULL);
	return jni;
}

void InAppBilling::dropJNIEnv()
{
	mAndroidApp.activity->vm->DetachCurrentThread();
}

#else	// not _IRR_ANDROID_PLATFORM_

#endif
