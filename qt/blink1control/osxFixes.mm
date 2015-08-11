
#include "osxFixes.h"

// C "trampoline" function to invoke Objective-C method
//int installOSXSleepWakeNotifiers (void *amainwindow, void *aParameter)
int installOSXSleepWakeNotifiers(void *amainwindow)
{
    OSXFixes* obj = [[OSXFixes alloc]init];
    [obj setMainWindow: (MainWindow*) amainwindow];
    return [obj installSleepWakeNotifiers]; // Call ObjC method using ObjC syntax
}

// ------------------
// Objective-C object
//
@implementation OSXFixes


//
- (void) setMainWindow: (void*)amw
{
    mw = (MainWindow*)amw;
}

//- (int) installSleepNotifiers:(void *) aParameter
- (int) installSleepWakeNotifiers
{
    // The Objective-C function you wanted to call from C++.
    // do work here..
    //These notifications are filed on NSWorkspace's notification center, not the default
    // notification center. You will not receive sleep/wake notifications if you file
    //with the default notification center.
    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
            selector: @selector(receiveSleepNote:)
            name: NSWorkspaceWillSleepNotification object: NULL];
 
    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver: self
            selector: @selector(receiveWakeNote:)
            name: NSWorkspaceDidWakeNotification object: NULL];
    
    return 21 ; // half of 42
}
// test with: osascript -e 'tell application "Finder" to sleep'
- (void) receiveSleepNote: (NSNotification*) note
{
    NSLog(@"receiveSleepNote: %@", [note name]);
    //mw->onApplicationStateChange((Qt::ApplicationState) 0x77 ); // 0x77 == sleep
    mw->goingToSleep();
}
//
- (void) receiveWakeNote: (NSNotification*) note
{
    NSLog(@"receiveWakeNote: %@", [note name]);
    //mw->onApplicationStateChange((Qt::ApplicationState) 0x66 ); // 0x66 == wake
    mw->wakingUp();
}
 
@end


    /* this doesn't seem to work
//#ifdef Q_OS_MAC
//#include <CoreFoundation/CoreFoundation.h>
//#endif
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if( mainBundle ){
        // get the application's Info Dictionary. For app bundles this would live in the bundle's Info.plist,
        // for regular executables it is obtained in another way.
        CFMutableDictionaryRef infoDict = (CFMutableDictionaryRef) CFBundleGetInfoDictionary(mainBundle);
        if( infoDict ){
            CFDictionarySetValue(infoDict, CFSTR("NSAppSleepDisabled"), CFSTR("1"));
            // Add or set the "LSUIElement" key with/to value "1". This can simply be a CFString.
            //CFDictionarySetValue(infoDict, CFSTR("LSUIElement"), CFSTR("1"));
            // That's it. We're now considered as an "agent" by the window server, and thus will have
            // neither menubar nor presence in the Dock or App Switcher.
        }
    }
    */


