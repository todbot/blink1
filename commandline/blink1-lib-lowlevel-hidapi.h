
#include "hidapi.h"


//
int blink1_enumerate(void)
{
    return blink1_enumerateByVidPid( blink1_vid(), blink1_pid() );
}

// get all matching devices by VID/PID pair
int blink1_enumerateByVidPid(int vid, int pid)
{
    struct hid_device_info *devs, *cur_dev;

    int p = 0; 
    devs = hid_enumerate(vid, pid);
    cur_dev = devs;    
    while (cur_dev) {
        if( (cur_dev->vendor_id != 0 && cur_dev->product_id != 0) &&  
            (cur_dev->vendor_id == vid && cur_dev->product_id == pid) ) { 
            if( cur_dev->serial_number != NULL ) { // can happen if not root
                strcpy( blink1_infos[p].path,   cur_dev->path );
                sprintf( blink1_infos[p].serial, "%ls", cur_dev->serial_number);
                //wcscpy( blink1_infos[p].serial, cur_dev->serial_number );
                //uint32_t sn = wcstol( cur_dev->serial_number, NULL, 16);
                uint32_t serialnum = strtol( blink1_infos[p].serial, NULL, 16);
                blink1_infos[p].type = BLINK1_MK1;
                if( serialnum >= blink1mk2_serialstart ) {
                    blink1_infos[p].type = BLINK1_MK2;
                }
                p++;
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    LOG("blink1_enumerateByVidPid: done, %d devices found\n",p);
    for( int i=0; i<p; i++ ) { 
        LOG("blink1_enumerateByVidPid: blink1_infos[%d].serial=%s\n",
            i, blink1_infos[i].serial);
    }
    blink1_cached_count = p;
    blink1_sortCache();

    return p;
}

//
blink1_device* blink1_openByPath(const char* path)
{
    if( path == NULL || strlen(path) == 0 ) return NULL;

    LOG("blink1_openByPath: %s\n", path);

    blink1_device* handle = hid_open_path( path ); 

    int i = blink1_getCacheIndexByPath( path );
    if( i >= 0 ) {  // good
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
    }
    
    return handle;
}

//
blink1_device* blink1_openBySerial(const char* serial)
{
    if( serial == NULL || strlen(serial) == 0 ) return NULL;
    int vid = blink1_vid();
    int pid = blink1_pid();
    
    LOG("blink1_openBySerial: %s at vid/pid %x/%x\n", serial, vid,pid);

    wchar_t wserialstr[serialstrmax] = {L'\0'};
#ifdef _WIN32   // omg windows you suck
    swprintf( wserialstr, serialstrmax, L"%S", serial); // convert to wchar_t*
#else
    swprintf( wserialstr, serialstrmax, L"%s", serial); // convert to wchar_t*
#endif
    LOG("blink1_openBySerial: serialstr: '%ls' %d\n", wserialstr, 
        blink1_getCacheIndexBySerial( serial ) );
    blink1_device* handle = hid_open(vid,pid, wserialstr ); 
    if( handle ) LOG("blink1_openBySerial: got a blink1_device handle\n"); 

    int i = blink1_getCacheIndexBySerial( serial );
    if( i >= 0 ) {
        LOG("blink1_openBySerial: good, serial id:%d was in cache\n",i);
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
        LOG("blink1_openBySerial: uh oh, serial id:%d was NOT IN CACHE\n",i);
    }

    return handle;
}

//
blink1_device* blink1_openById( uint32_t i ) 
{ 
    LOG("blink1_openById: %d \n", i );
    if( i > blink1_max_devices ) { // then i is a serial number not an array index
        char serialstr[serialstrmax];
        sprintf( serialstr, "%X", i);  // convert to wchar_t* 
        return blink1_openBySerial( serialstr );  
    } 
    else {
        return blink1_openByPath( blink1_getCachedPath(i) );
    }
}

//
blink1_device* blink1_open(void)
{
    blink1_enumerate();
    
    return blink1_openById( 0 );
}

//
// FIXME: search through blink1s list to zot it too?
void blink1_close( blink1_device* dev )
{
    if( dev != NULL ) {
        blink1_clearCacheDev(dev); // FIXME: hmmm 
        hid_close(dev);
    }
    dev = NULL;
    //hid_exit(); // FIXME: this cleans up libusb in a way that hid_close doesn't
}

//
int blink1_write( blink1_device* dev, void* buf, int len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int rc = hid_send_feature_report( dev, buf, len );
    // FIXME: put this in an ifdef?
    if( rc==-1 ) {
        LOG("blink1_write error: %ls\n", hid_error(dev));
    }
    return rc;
}

// len should contain length of buf
// after call, len will contain actual len of buf read
int blink1_read( blink1_device* dev, void* buf, int len)
{
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    int rc = hid_send_feature_report(dev, buf, len); // FIXME: check rc

    if( (rc = hid_get_feature_report(dev, buf, len) == -1) ) {
      LOG("error reading data: %s\n",blink1_error_msg(rc));
    }
    return rc;
}

// FIXME: Does not work at all times
// for mk1 devices only
int blink1_readRGB_mk1(blink1_device *dev, uint16_t* fadeMillis,
                       uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint8_t buf[blink1_buf_size] = { blink1_report_id };
    int rc;
    blink1_sleep( 50 ); // FIXME:
    if((rc = hid_get_feature_report(dev, buf, sizeof(buf))) == -1){
        LOG("error reading data.\n");
    }
    *r = buf[2];
    *g = buf[3];
    *b = buf[4];
    return rc;
}

//
char *blink1_error_msg(int errCode)
{
    /*
    static char buf[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:    return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:  return "The specified device was not found";
        case USBOPEN_ERR_IO:        return "Communication error with device";
        default:
            sprintf(buf, "Unknown USB error %d", errCode);
            return buf;
    }
    */
    return NULL;    /* not reached */
}
