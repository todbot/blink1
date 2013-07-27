
#include <jni.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "blink1-lib.h"

/* ------------------------------------------------------------------------- */

//static hid_device* dev;

void setDeviceToJava(JNIEnv *env, jobject obj, hid_device* dev);
hid_device* getDeviceFromJava(JNIEnv *env, jobject obj);
//void setIsEnumerated(JNIEnv *env, 
int isEnumerated = 0;

//
hid_device* getDevicePtr(JNIEnv *env, jobject obj)
{
    jclass class = (*env)->GetObjectClass(env, obj); 
    jfieldID fieldId = (*env)->GetFieldID(env, class, "hidDevicePtr", "J");
    jlong jhidptr = (*env)->GetLongField(env, obj, fieldId);
    hid_device* dev = (hid_device*) jhidptr;
    return dev;
}

//
void setDevicePtr(JNIEnv *env, jobject obj, hid_device* devt)
{
    jclass class = (*env)->GetObjectClass(env, obj); 
    jfieldID fieldId = (*env)->GetFieldID(env, class, "hidDevicePtr", "J");
    (*env)->SetLongField(env, obj, fieldId, (jlong)devt );
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
        //printf("native path=%s\n",blink1_getCachedPath(i));
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
    hid_device* devt = blink1_openByPath( devicepath );
    (*env)->ReleaseStringUTFChars(env, jdevicepath, devicepath);

    if( devt == NULL ) return NULL;

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
   
    return obj;
}

//JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_openBySerial__Ljava_lang_String_2
//  (JNIEnv *env, jobject obj, jstring jserialnumber)
JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_openBySerial
  (JNIEnv *env, jclass class, jstring jserialnumber)
{
    const char *serialnumber = (*env)->GetStringUTFChars(env, jserialnumber, 0);
    hid_device* devt = blink1_openBySerial( serialnumber );
    (*env)->ReleaseStringUTFChars(env, jserialnumber, serialnumber);

    if( devt == NULL ) return NULL;

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
   
    return obj;
}

JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_openById
  (JNIEnv *env, jclass class, jint id)
{
    hid_device* devt = blink1_openById( id );

    if( devt == NULL ) return NULL;

    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    setDevicePtr(env,obj, devt);
   
    return obj;
}

/*
 * 
 */
JNIEXPORT jobject JNICALL Java_thingm_blink1_Blink1_open
  (JNIEnv *env, jclass class)
{
    jmethodID constructorMethodID = (*env)->GetMethodID(env, class, "<init>", "()V");
    jobject obj = (*env)->NewObject(env, class, constructorMethodID);

    hid_device* devt = blink1_open();

    setDevicePtr(env,obj, devt);
    isEnumerated = 1;  // blink1_open() does enumeration (blink1_openById() does not)
    
    if( devt == NULL ) obj = NULL;
    return obj;
}

/**
 *
 *
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_open
(JNIEnv *env, jobject obj) //, jint vid, jint pid, jstring vstr, jstring pstr)
{
    int err = 0;
    
    hid_device* devt = blink1_open();

    if( devt == NULL ) err = -1;

    isEnumerated = 1;  // blink1_open() does enumeration (blink1_openById() does not)

    setDevicePtr(env,obj, devt);
    
    return err;  // FIXME: error
}
*/

/**
 *
 */
JNIEXPORT void JNICALL Java_thingm_blink1_Blink1_close
(JNIEnv *env, jobject obj)
{
    hid_device* devt = getDevicePtr(env,obj);
    blink1_close(devt);
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_setRGB
(JNIEnv *env, jobject obj, jint r, jint g, jint b)
{
    int err;
    hid_device* devt = getDevicePtr(env,obj);
    err = blink1_setRGB(devt, r,g,b);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_fadeToRGB
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b)
{
    int err;
    hid_device* devt = getDevicePtr(env,obj);

    err = blink1_fadeToRGB(devt, fadeMillis, r,g,b);
    //err = blink1_fadeToRGB(dev, fadeMillis, r,g,b);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_writePatternLine
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b, jint pos)
{
    int err;
    hid_device* devt = getDevicePtr(env,obj);
    err = blink1_writePatternLine(devt, fadeMillis, r,g,b, pos);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_play
(JNIEnv *env, jobject obj, jboolean play, jint pos)
{
    int err;
    //play = (play) ? 1 : 0; // normalize just in case
    hid_device* devt = getDevicePtr(env,obj);
    err = blink1_play(devt, play, pos);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_serverdown
(JNIEnv *env, jobject obj, jboolean on, jint millis)
{
    int err;
    //on = (on) ? 1 : 0; // normalize just in case
    hid_device* devt = getDevicePtr(env,obj);
    err = blink1_serverdown(devt, on, millis, 0);
    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_getFirmwareVersion
(JNIEnv *env, jobject obj)
{
    hid_device* devt = getDevicePtr(env,obj);
    jint c = blink1_getVersion(devt);
    return c;
}


// --------------------------------------------------------------------------
// old ideas


/*
//
hid_device* getDeviceFromJava(JNIEnv *env, jobject obj)
{
    jclass cls = (*env)->GetObjectClass(env,obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "hidDevicePtr", "J");
    if( fid==0 ) {
        printf("nativeBlinkM: no fid");
        return NULL;
    }
    jlong lp = (*env)->GetLongField(env,cls,fid);
    hid_device* dev = (hid_device*)lp;
    //hid_device* dev = (hid_device*) (*env)->GetLongField(env,cls,fid);
    printf("nativeBlink1: setDeviceToJava: %ld\n", (long)dev);
    return dev;
}

//
void setDeviceToJava(JNIEnv *env, jobject obj, hid_device* dev)
{
    jclass cls = (*env)->GetObjectClass(env,obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "hidDevicePtr", "J");
    if( fid==0 ) {
        printf("nativeBlinkM: no fid");
        return;
    }
    printf("nativeBlink1: setDeviceToJava: %ld\n", (long)dev);
    (*env)->SetLongField(env, obj, fid, (jlong)dev );
}
*/

/*
//
hid_device* getDeviceFromJava(JNIEnv *env, jobject obj)
{
    jclass cls = (*env)->GetObjectClass(env,obj);
    jfieldID fid = (*env)->GetFieldID(env, cls, "hidDevicePtr", "Ljava/nio/ByteBuffer;");
    jobject bb = (*env)->GetObjectField(env,obj,fid);
    printf("got here\n");
    hid_device* dev = (hid_device*) (*env)->GetDirectBufferAddress(env,bb);
    printf("nativeBlink1: getDeviceFromJava: %ld\n", (long)dev);
    return dev;
}

//
void setDeviceToJava(JNIEnv *env, jobject obj, hid_device* dev)
{
    printf("nativeBlink1: setDeviceToJava: %ld\n", (long)dev);
    (*env)->NewDirectByteBuffer( env, (void*) dev, 100 );
}
*/

/**
 *
 *
JNIEXPORT void JNICALL Java_thingm_blink1_Blink1_command
(JNIEnv *env, jobject obj, jint cmd, jbyteArray jb_send, jbyteArray jb_recv)
{
    int err;
    uint8_t cmdbyte = (uint8_t) cmd;
    int num_send=0;
    int num_recv=0;
    uint8_t* byte_send = NULL;
    uint8_t* byte_recv = NULL;

    if( jb_send != NULL ) {
        num_send = (*env)->GetArrayLength(env, jb_send );
        byte_send = (uint8_t*)(*env)->GetByteArrayElements(env, jb_send,0);
    }
    if( jb_recv != NULL ) {
        num_recv = (*env)->GetArrayLength(env, jb_recv );
        byte_recv = (uint8_t*)(*env)->GetByteArrayElements(env, jb_recv,0);
    }

    err = blink1_command(dev, cmdbyte,num_send,num_recv,byte_send,byte_recv);

    if( err ) {
        (*env)->ExceptionDescribe(env);          // throw an exception.
        (*env)->ExceptionClear(env);
        jclass newExcCls = (*env)->FindClass(env,"java/io/IOException");
        (*env)->ThrowNew(env, newExcCls, blink1_error_msg(err));
    }
    
    if( jb_send != NULL )
        (*env)->ReleaseByteArrayElements(env, jb_send, (jbyte*) byte_send, 0);
    if( jb_recv != NULL ) 
        (*env)->ReleaseByteArrayElements(env, jb_recv, (jbyte*) byte_recv, 0);
}
*/
