#ifndef __OSXFIXES_C_INTERFACE_H__
#define __OSXFIXES_C_INTERFACE_H__


// This is the C "trampoline" function that will be used
// to invoke a specific Objective-C method FROM C++
//int installSleepWakeNotifiers (void *myObjectInstance, void *parameter);
int installOSXSleepWakeNotifiers (void *myObjectInstance);

#endif



