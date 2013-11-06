
#include "hiddata.h"


static blink1_device* static_dev;


//
char *blink1_error_msg(int errCode)
{
    static char buffer[80];

    switch(errCode){
        case USBOPEN_ERR_ACCESS:    return "Access to device denied";
        case USBOPEN_ERR_NOTFOUND:  return "The specified device was not found";
        case USBOPEN_ERR_IO:        return "Communication error with device";
        default:
            sprintf(buffer, "Unknown USB error %d", errCode);
            return buffer;
    }
    return NULL;    /* not reached */
}

//
int blink1_enumerate(void)
{
    LOG("blink1_enumerate!\n");
    return blink1_enumerateByVidPid( blink1_vid(), blink1_pid() );
}

// get all matching devices by VID/PID pair
int blink1_enumerateByVidPid(int vid, int pid)
{
    int p = 0; 
    if( blink1_open() ) { 
        blink1_close(static_dev);
        p = 1;
    }

    /*
    struct hid_device_info *devs, *cur_dev;

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
*/
    
    blink1_cached_count = p;

    blink1_sortCache();

    return p;
}

//
blink1_device* blink1_openByPath(const char* path)
{
    //if( path == NULL || strlen(path) == 0 ) {
    //    LOG("openByPath: empty path");
    //    return NULL;
    //}

    LOG("blink1_openByPath %s\n", path);
    /*
    blink1_device* handle = hid_open_path( path ); 

    int i = blink1_getCacheIndexByPath( path );
    if( i >= 0 ) {  // good
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
    }
    
    return handle;
    */
    return blink1_open();
}

//
blink1_device* blink1_openBySerial(const char* serial)
{
    if( serial == NULL || strlen(serial) == 0 ) {
        LOG("openByPath: empty path");
        return NULL;
    }
    int vid = blink1_vid();
    int pid = blink1_pid();
    
    LOG("blink1_openBySerial %s at vid/pid %x/%x\n", serial, vid,pid);

    /*
    wchar_t wserialstr[serialstrmax] = {L'\0'};
#ifdef _WIN32   // omg windows you suck
    swprintf( wserialstr, serialstrmax, L"%S", serial); // convert to wchar_t*
#else
    swprintf( wserialstr, serialstrmax, L"%s", serial); // convert to wchar_t*
#endif
    LOG("serialstr: '%ls' \n", wserialstr );
    blink1_device* handle = hid_open(vid,pid, wserialstr ); 
    if( handle ) LOG("got a blink1_device handle\n"); 

    int i = blink1_getCacheIndexBySerial( serial );
    if( i >= 0 ) {
        LOG("good, serial was in cache\n");
        blink1_infos[i].dev = handle;
    }
    else { // uh oh, not in cache, now what?
        LOG("uh oh, serial was not in cache\n");
    }

    return handle;
    */
    return blink1_open();
}

//
blink1_device* blink1_openById( uint32_t i ) 
{ 
    if( i > blink1_max_devices ) { // then i is a serial number not array index
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
    int rc = usbhidOpenDevice( &static_dev, 
                               blink1_vid(), NULL,
                               blink1_pid(), NULL,
                               1);  // NOTE: '0' means "not using report IDs"
    LOG("blink1_open\n");
    if( rc != USBOPEN_SUCCESS ) { 
        LOG("cannot open: \n");
    }
    return static_dev;
}

//
// FIXME: search through blink1s list to zot it too?
void blink1_close( blink1_device* dev )
{
    if( dev != NULL ) {
        blink1_clearCacheDev(dev); // FIXME: hmmm 
        usbhidCloseDevice(dev);
    }
    dev = NULL;
    //hid_exit();// FIXME: this cleans up libusb in a way that hid_close doesn't
}

//
int blink1_write( blink1_device* dev, void* buf, int len)
{
    int rc;
    if( dev==NULL ) {
        return -1; // BLINK1_ERR_NOTOPEN;
    }
    if( (rc = usbhidSetReport(dev, buf, len) != 0) ){
        LOG( "blink1_write error: %s\n", blink1_error_msg(rc));
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
    int rc = blink1_write( dev, buf, len); // FIXME: check rc
    if((rc = usbhidGetReport(dev, 1, (char*)buf, &len)) != 0) {
        LOG("error reading data: %s\n", blink1_error_msg(rc));
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
    int len = sizeof(buf);
    blink1_sleep( 50 ); // FIXME:
    if((rc = usbhidGetReport(dev, 1, (char*)buf, &len)) != 0) {
        LOG("error reading data: %s\n", blink1_error_msg(rc));
    }
    *r = buf[2];
    *g = buf[3];
    *b = buf[4];
    return rc;
}


