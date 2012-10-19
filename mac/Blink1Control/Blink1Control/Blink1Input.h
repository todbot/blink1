//
//  Blink1Input.h
//  Blink1Control
//
//  Created by Tod E. Kurt on 10/18/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Blink1Input : NSObject

@property Boolean enabled;
@property (retain) NSString* name;
@property (retain) NSString* pname;
@property (retain) NSString* type;
@property (retain) NSString* status;
@property (retain) NSString* lastVal;

@end
