//
//  Blink1.h
//  Blink1Control
//
//  Created by Tod E. Kurt on 9/6/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Blink1 : NSObject {
    

}

- (NSMutableArray *) enumerate;

- (void) fadeToRGB:(NSColor*) c atTime:(float) t;

+ (NSColor *) colorFromInt: (unsigned int)colorCode;
+ (NSColor *) colorFromHexRGB:(NSString *) inColorString;
+ (NSString*) toHexColorString: (NSColor*)colr;

@end
