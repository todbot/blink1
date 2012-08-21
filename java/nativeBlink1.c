
#include <jni.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "blink1-lib.h"

/* ------------------------------------------------------------------------- */

static hid_device* dev;


JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_enumerate
(JNIEnv *env, jobject obj)
{
    //hid_device* dev = getDeviceFromJava(env,obj);
    printf("jni start here!\n");
    jint c = blink1_enumerate();
    return c;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_getCount
(JNIEnv *env, jobject obj)
{
    int count = blink1_getCachedCount();
    return count;
}

JNIEXPORT jobjectArray JNICALL Java_thingm_blink1_Blink1_getDevicePaths
(JNIEnv *env, jobject obj)
{
    int count = blink1_getCachedCount();

    jclass strCls = (*env)->FindClass(env,"Ljava/lang/String;");
    jobjectArray strarray = (*env)->NewObjectArray(env,count,strCls,NULL);

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
    int count = blink1_getCachedCount();

    jclass strCls = (*env)->FindClass(env,"Ljava/lang/String;");
    jobjectArray strarray = (*env)->NewObjectArray(env,count,strCls,NULL);

    for( int i=0; i<count; i++ ) { 
        //printf("native serial=%ls\n", blink1_getCachedSerial(i));
        //FIXME: wrt (char*)?
        // FIXME: hardcoded 16 strlen
        //jstring str=(*env)->NewString(env,(jchar*)blink1_getCachedSerial(i), 8*sizeof(jchar));
        char serstr[9];
        sprintf( serstr, "%ls", blink1_getCachedSerial(i) );
        jstring str=(*env)->NewStringUTF(env, serstr);
        (*env)->SetObjectArrayElement(env,strarray,i,str);
        (*env)->DeleteLocalRef(env,str);
    }
    return strarray;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_openByPath__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jdevicepath)
{
    int err = 0;
    const char *devicepath = (*env)->GetStringUTFChars(env, jdevicepath, 0);
    
    dev = blink1_openByPath( devicepath );
    
    (*env)->ReleaseStringUTFChars(env, jdevicepath, devicepath);

    if( dev == NULL ) err = -1;

    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_openBySerial__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jserialnumber)
{
    int err = 0;
    const jchar *serialnumber = (*env)->GetStringChars(env, jserialnumber, 0);
    
    dev = blink1_openBySerial( (wchar_t*) serialnumber ); //FIXME: okay?
    
    (*env)->ReleaseStringChars(env, jserialnumber, serialnumber);

    if( dev == NULL ) err = -1;

    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_openById
(JNIEnv *env, jobject obj, jint id )
{
    int err = 0;
    hid_device* devt = blink1_openById( id );
    dev = devt;

    //setDeviceToJava(env,obj, devt);
    if( dev == NULL ) err = -1;

    fprintf(stderr, "nativeBlink1:open devid=%d\n", err); // debug
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_open
(JNIEnv *env, jobject obj) //, jint vid, jint pid, jstring vstr, jstring pstr)
{
    int err = 0;
    
    hid_device* devt = blink1_open();
    dev = devt;

    //setDeviceToJava(env,obj, devt);
    if( dev == NULL ) err = -1;

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

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_setRGB
(JNIEnv *env, jobject obj, jint r, jint g, jint b)
{
    int err;
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_setRGB(dev, r,g,b);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_fadeToRGB
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b)
{
    int err;
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_fadeToRGB(dev, fadeMillis, r,g,b);
    return err;
}

/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blink1_Blink1_writePatternLine
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b, jint pos)
{
    int err;
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_writePatternLine(dev, fadeMillis, r,g,b, pos);
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
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_play(dev, play, pos);
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
    //hid_device* dev = getDeviceFromJava(env,obj);
    err = blink1_serverdown(dev, on, millis);
    return err;
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
