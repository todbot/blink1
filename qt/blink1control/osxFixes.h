
#import "osxFixes-c.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include "mainwindow.h"

@interface MyObject : NSObject
{
    MainWindow* mw;
}

// The Objective-C member function you want to call from C++
- (void) setMainWindow: (void*)amw;
- (int) doSomethingWith:(void *) aParameter;
- (void) receiveSleepNote: (NSNotification*) note;
- (void) receiveWakeNote: (NSNotification*) note;

@end



