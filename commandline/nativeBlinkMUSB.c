
#include <jni.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "hiddata.h"
#include "blinkmusb-lib.h"

/* ------------------------------------------------------------------------- */

static usbDevice_t* dev = NULL;   // sigh.

// the goal is to have a usbDevice_t* per LinkM instance, 
// but for some reason I cannot get a LinkM instance int or long to
// store the pointer in and then retrieve it. 
// So for now, there's one global 'dev', so only one LinkM per system.

// maybe one way to support multiple LinkMs per system is to have 
// small array of devs (e.g "dev[8]") and then allow up to 8 devs
// BUT, also need to change C API to support more advanced query & finding
// of LinkMs.


/**
 *
 */
JNIEXPORT jint JNICALL Java_thingm_blinkm_BlinkMUSB_open
(JNIEnv *env, jobject obj) //, jint vid, jint pid, jstring vstr, jstring pstr)
{
    int err;
    
    // open up linkm, get back a 'dev' to pass around
    err = blinkmusb_openstatic( &dev ); // FIXME: pass in vid/pid in the future

    return err;
}

/**
 *
 */
JNIEXPORT void JNICALL Java_thingm_blinkm_BlinkMUSB_close
(JNIEnv *env, jobject obj)
{
    blinkmusb_close(dev);
}

JNIEXPORT jint JNICALL Java_thingm_blinkm_BlinkMUSB_setRGB
(JNIEnv *env, jobject obj, jint r, jint g, jint b)
{
    int err;
    err = blinkmusb_setRGB(dev, r,g,b);
    return err;
}

JNIEXPORT jint JNICALL Java_thingm_blinkm_BlinkMUSB_fadeToRGB
(JNIEnv *env, jobject obj, jint fadeMillis, jint r, jint g, jint b)
{
    int err;
    err = blinkmusb_fadeToRGB(dev, fadeMillis, r,g,b);
    return err;
}

/**
 *
 *
JNIEXPORT void JNICALL Java_thingm_blinkm_BlinkMUSB_command
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

    err = blinkmusb_command(dev, cmdbyte,num_send,num_recv,byte_send,byte_recv);

    if( err ) {
        (*env)->ExceptionDescribe(env);          // throw an exception.
        (*env)->ExceptionClear(env);
        jclass newExcCls = (*env)->FindClass(env,"java/io/IOException");
        (*env)->ThrowNew(env, newExcCls, blinkmusb_error_msg(err));
    }
    
    if( jb_send != NULL )
        (*env)->ReleaseByteArrayElements(env, jb_send, (jbyte*) byte_send, 0);
    if( jb_recv != NULL ) 
        (*env)->ReleaseByteArrayElements(env, jb_recv, (jbyte*) byte_recv, 0);
}
*/
