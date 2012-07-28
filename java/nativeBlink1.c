
#include <jni.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "blink1-lib.h"

/* ------------------------------------------------------------------------- */

static hid_device* dev;

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

JNIEXPORT jobjectArray JNICALL Java_thingm_blink1_Blink1_getDevicePaths
(JNIEnv *env, jobject obj)
{
    int count = blink1_enumerate();

    jclass strCls = (*env)->FindClass(env,"Ljava/lang/String;");
    jobjectArray strarray = (*env)->NewObjectArray(env,count,strCls,NULL);

    for( int i=0; i<count; i++ ) { 
        printf("path=%s\n",blink1_cached_path(i));
        jstring str = (*env)->NewStringUTF(env, blink1_cached_path(i) );
        (*env)->SetObjectArrayElement(env,strarray,i,str);
        (*env)->DeleteLocalRef(env,str);
    }
    return strarray;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_open__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jdevicepath)
{
    int err = 0;
    const char *devicepath = (*env)->GetStringUTFChars(env, jdevicepath, 0);
    
    dev = blink1_open_path( devicepath );
    
    (*env)->ReleaseStringUTFChars(env, jdevicepath, devicepath);

    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_open
(JNIEnv *env, jobject obj) //, jint vid, jint pid, jstring vstr, jstring pstr)
{
    int err=0;
    
    hid_device* devt = blink1_open();
    dev = devt;

    //setDeviceToJava(env,obj, devt);
    
    return err;  // FIXME: error
}

/**
 *
 */
JNIEXPORT void JNICALL Java_thingm_blink1_Blink1_close
(JNIEnv *env, jobject obj)
{
    //hid_device* dev = getDeviceFromJava(env,obj);
    blink1_close(dev);
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_setRGB
(JNIEnv *env, jobject obj, jint r, jint g, jint b)
{
    int err;
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_setRGB(dev, r,g,b);
    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_fadeToRGB
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b)
{
    int err;
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_fadeToRGB(dev, fadeMillis, r,g,b);
    return err;
}


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
