
#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#import "osxFixes-c.h"

#include "mainwindow.h"

@interface OSXFixes : NSObject
{
    MainWindow* mw;
}

// The Objective-C member function you want to call from C++
- (void) setMainWindow: (void*)amw;
//- (int) doSomethingWith:(void *) aParameter;
- (int) installSleepWakeNotifiers;
- (void) receiveSleepNote: (NSNotification*) note;
- (void) receiveWakeNote: (NSNotification*) note;

@end



