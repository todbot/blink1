#ifndef __MYOBJECT_C_INTERFACE_H__
#define __MYOBJECT_C_INTERFACE_H__


// This is the C "trampoline" function that will be used
// to invoke a specific Objective-C method FROM C++
int MyObjectDoSomethingWith (void *myObjectInstance, void *parameter);

#endif



