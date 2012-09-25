//
//  Blink1.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 9/6/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import "Blink1.h"

#include "blink1-lib.h"


@implementation Blink1

//
- (void)fadeToRGB:(NSColor*) c atTime:(float) t
{
    CGFloat r,g,b;
    [c getRed:&r green:&g blue:&b alpha:NULL];
    r *= 255; g *= 255; b*=255;
    NSLog(@"blink1fadeToRGB: rgb:%d,%d,%d t:%2.3f", (int)r,(int)g,(int)b,t);
    
    hid_device *dev = blink1_openById(0);
    blink1_fadeToRGB(dev, (int)(t*1000), (int)r,(int)g,(int)b );
    blink1_close(dev);
}

//
- (NSMutableArray*) enumerate
{
    NSMutableArray* serialnums = [[NSMutableArray alloc] init];
    int count = blink1_enumerate();
    for( int i=0; i< count; i++ ) {
        NSString * serstr = [[NSString alloc] initWithBytes:blink1_getCachedSerial(i)
                                                     length:8*4
                                                   encoding:NSUTF32LittleEndianStringEncoding];
        [serialnums addObject: serstr];
    }
    return serialnums;
}

/*
 NSColor: Instantiate from Web-like Hex RRGGBB string
 Original Source: <http://cocoa.karelia.com/Foundation_Categories/NSColor__Instantiat.m>
 (See copyright notice at <http://cocoa.karelia.com>)
 */
- (NSColor *) colorFromHexRGB:(NSString *) inColorString
{
    NSString* hexNum = [inColorString substringFromIndex:1];
	NSColor *result = nil;
	unsigned int colorCode = 0;
	unsigned char red, green, blue;
    
	if (nil != hexNum) {
		NSScanner *scanner = [NSScanner scannerWithString:hexNum];
		(void) [scanner scanHexInt:&colorCode];	// ignore error
	}
	red		= (unsigned char) (colorCode >> 16);
	green	= (unsigned char) (colorCode >> 8);
	blue	= (unsigned char) (colorCode);	// masks off high bits
    
	result = [NSColor colorWithCalibratedRed:(float)red/0xff
                                       green:(float)green/0xff
                                        blue:(float)blue/0xff alpha:1.0];
	return result;
}


@end
