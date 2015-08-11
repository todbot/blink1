
#include "osxFixes.h"


@implementation MyObject

// C "trampoline" function to invoke Objective-C method
int MyObjectDoSomethingWith (void *amw, void *aParameter)
{
    MyObject* obj = [[MyObject alloc]init];
    [obj setMainWindow: (MainWindow*) amw];
    // Call the Objective-C method using Objective-C syntax
    return [obj doSomethingWith:aParameter];
}

//
- (void) setMainWindow: (void*)amw
{
    mw = (MainWindow*)amw;
}

- (int) doSomethingWith:(void *) aParameter
{
    // The Objective-C function you wanted to call from C++.
    // do work here..
    NSLog(@"I AM ALIVE!\n");
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
    mw->onApplicationStateChange((Qt::ApplicationState) 0x77 ); // 0x77 == sleep
}
- (void) receiveWakeNote: (NSNotification*) note
{
    NSLog(@"receiveWakeNote: %@", [note name]);
    mw->onApplicationStateChange((Qt::ApplicationState) 0x66 ); // 0x66 == wake
}
 
@end


