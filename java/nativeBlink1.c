
#include <jni.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "blink1-lib.h"

/* ------------------------------------------------------------------------- */

void setDeviceToJava(JNIEnv *env, jobject obj, blink1_device* dev);
blink1_device* getDeviceFromJava(JNIEnv *env, jobject obj);

int isEnumerated = 0;

//
blink1_device* getDevicePtr(JNIEnv *env, jobject obj)
{
    jclass class = (*env)->GetObjectClass(env, obj); 
    jfieldID fieldId = (*env)->GetFieldID(env, class, "blink1DevicePtr", "J");
    jlong jdevptr = (*env)->GetLongField(env, obj, fieldId);
    blink1_device* dev = (blink1_device*) jdevptr;
    return dev;
}

//
void setDevicePtr(JNIEnv *env, jobject obj, blink1_device* devt)
{
    jclass class = (*env)->GetObjectClass(env, obj); 
    jfieldID fieldId = (*env)->GetFieldID(env, class, "blink1DevicePtr", "J");
    (*env)->SetLongField(env, obj, fieldId, (jlong)devt );
}

void setErrorCode(JNIEnv *env, jobject obj, int code)
{
    jclass class = (*env)->GetObjectClass(env, obj); 
    jfieldID fieldId = (*env)->GetFieldID(env, class, "errorCode", "I");
    (*env)->SetIntField(env, obj, fieldId, code );
}


//
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_enumerate
(JNIEnv *env, jclass class)
{
    jint c = blink1_enumerate();
    isEnumerated = 1;

    return c;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_getCount
(JNIEnv *env, jclass class)
{
    if( !isEnumerated ) {
        blink1_enumerate();
        isEnumerated = 1;
    }
    int count = blink1_getCachedCount();
    return count;
}

JNIEXPORT jobjectArray JNICALL Java_thingm_blink1_Blink1_getDevicePaths
(JNIEnv *env, jobject obj)
{
    if( !isEnumerated ) {
        blink1_enumerate();
        isEnumerated = 1;
    }

    int count = blink1_getCachedCount();

    jclass stringClass = (*env)->FindClass(env,"Ljava/lang/String;");    
    jobjectArray strarray = (*env)->NewObjectArray(env,count,stringClass,NULL);

    for( int i=0; i<count; i++ ) { 
        jstring str = (*env)->NewStringUTF(env, blink1_getCachedPath(i) );
        (*env)->SetObjectArrayElement(env,strarray,i,str);
        (*env)->DeleteLocalRef(env,str);
    }
    return strarray;
}

JNIEXPORT jobjectArray JNICALL Java_thingm_blink1_Blink1_getDeviceSerials
(JNIEnv *env, jobject obj)
{
    if( !isEnumerated ) {
        blink1_enumerate();
        isEnumerated = 1;
    }

    int count = blink1_getCachedCount();

    jclass stringClass = (*env)->FindClass(env,"Ljava/lang/String;");
    jobjectArray strarray = (*env)->NewObjectArray(env,count,stringClass,NULL);

    for( int i=0; i<count; i++ ) { 
        char serstr[9];
        sprintf( serstr, "%s", blink1_getCachedSerial(i) );
        jstring str=(*env)->NewStringUTF(env, serstr);
        (*env)->SetObjectArrayElement(env,strarray,i,str);
        (*env)->DeleteLocalRef(env,str);
    }
    return strarray;
}

JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_openByPath
  (JNIEnv *env, jclass class, jstring jdevicepath)
{
    const char *devicepath = (*env)->GetStringUTFChars(env, jdevicepath, 0);
    blink1_device* devt = blink1_openByPath( devicepath );
    (*env)->ReleaseStringUTFChars(env, jdevicepath, devicepath);

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
   
    setErrorCode(env,obj,  (devt == NULL) ? -1 : 0 );
        
    return obj;
}

JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_openBySerial
  (JNIEnv *env, jclass class, jstring jserialnumber)
{
    const char *serialnumber = (*env)->GetStringUTFChars(env, jserialnumber, 0);
    blink1_device* devt = blink1_openBySerial( serialnumber );
    (*env)->ReleaseStringUTFChars(env, jserialnumber, serialnumber);

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);

    setErrorCode(env,obj,  (devt == NULL) ? -1 : 0 );

    return obj;
}

JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_openById
  (JNIEnv *env, jclass class, jint id)
{
    blink1_device* devt = blink1_openById( id );

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
   
    setErrorCode(env,obj,  (devt == NULL) ? -1 : 0 );

    return obj;
}

/*
 * 
 */
JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_open
  (JNIEnv *env, jclass class)
{
    blink1_device* devt = blink1_open();

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
    isEnumerated = 1;  // blink1_open() does enumeration (blink1_openById() does not)
    
    setErrorCode(env,obj,  (devt == NULL) ? -1 : 0 );

    return obj;
}

/**
 *
 */
JNIEXPORT void JNICALL Java_thingm_blink1_Blink1_close
(JNIEnv *env, jobject obj)
{
    blink1_device* devt = getDevicePtr(env,obj);
    blink1_close(devt);
    setDevicePtr(env,obj, NULL);

    setErrorCode(env,obj,0);
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_setRGB
(JNIEnv *env, jobject obj, jint r, jint g, jint b)
{
    blink1_device* devt = getDevicePtr(env,obj);

    int err = blink1_setRGB(devt, r,g,b);

    setErrorCode(env,obj,err);

    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_fadeToRGB__IIII
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b)
{
    blink1_device* devt = getDevicePtr(env,obj);

    int err = blink1_fadeToRGB(devt, fadeMillis, r,g,b);
    //err = blink1_fadeToRGB(dev, fadeMillis, r,g,b);

    setErrorCode(env,obj,err);

    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_fadeToRGB__IIIII
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b, jint ledn)
{
    blink1_device* devt = getDevicePtr(env,obj);

    int err = blink1_fadeToRGBN(devt, fadeMillis, r,g,b, ledn);
    //err = blink1_fadeToRGB(dev, fadeMillis, r,g,b);

    setErrorCode(env,obj,err);

    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_writePatternLine
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b, jint pos)
{
    blink1_device* devt = getDevicePtr(env,obj);
    int err = blink1_writePatternLine(devt, fadeMillis, r,g,b, pos);
    setErrorCode(env,obj,err);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_play
(JNIEnv *env, jobject obj, jboolean play, jint pos)
{
    //play = (play) ? 1 : 0; // normalize just in case
    blink1_device* devt = getDevicePtr(env,obj);
    int err = blink1_play(devt, play, pos);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_serverdown
(JNIEnv *env, jobject obj, jboolean on, jint millis)
{
    //on = (on) ? 1 : 0; // normalize just in case
    blink1_device* devt = getDevicePtr(env,obj);
    int err = blink1_serverdown(devt, on, millis, 0);
    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_getFirmwareVersion
(JNIEnv *env, jobject obj)
{
    blink1_device* devt = getDevicePtr(env,obj);
    jint c = blink1_getVersion(devt);
    return c;
}


// eof
