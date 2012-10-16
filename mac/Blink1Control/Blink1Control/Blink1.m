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

@synthesize serialnums;
@synthesize blink1_id;
@synthesize host_id;
@synthesize lastColor;
@synthesize updateHandler;

//
- (id) init
{
    self = [super init];
    serialnums = [[NSMutableArray alloc] init];
    return self;
}

//
- (NSMutableArray*) enumerate
{
    @synchronized(self) {
        [serialnums removeAllObjects];
        int count = blink1_enumerate();
        for( int i=0; i< count; i++ ) {
            NSString * serstr = [[NSString alloc] initWithBytes:blink1_getCachedSerial(i)
                                                         length:8*4
                                                       encoding:NSUTF32LittleEndianStringEncoding];
            [serialnums addObject: serstr];
        }
    }
    return serialnums;
}

//
- (NSString*) regenerateBlink1Id
{
    if( host_id == nil ) {
        host_id = [NSString stringWithFormat:@"%8.8X",rand()];
    }
    NSString* blink1_serial = @"00000000"; // 8-hexdigit serialnum
    if( [serialnums count] ) {  // we have a blink1
        blink1_serial = [serialnums objectAtIndex:0];
    }
    blink1_id = [NSString stringWithFormat:@"%@%@",host_id,blink1_serial];
    return blink1_id;
}

//
- (void)fadeToRGBstr:(NSString*) hexcstr atTime:(float)t
{
    [self fadeToRGB:[Blink1 colorFromHexRGB: hexcstr] atTime:t];
}

//
- (void)fadeToRGB:(NSColor*) c atTime:(float) t
{
    CGFloat r,g,b;
    [c getRed:&r green:&g blue:&b alpha:NULL];
    r *= 255; g *= 255; b*=255;
    NSLog(@"blink1fadeToRGB: rgb:%d,%d,%d t:%2.3f", (int)r,(int)g,(int)b,t);
    lastColor = c;
    if( updateHandler ) { updateHandler(c,t); }
    if( [serialnums count] == 0 ) return;
    @synchronized(self) {
        hid_device *dev = blink1_openById(0);
        blink1_fadeToRGB(dev, (int)(t*1000), (int)r,(int)g,(int)b );
        blink1_close(dev);
    }
}


//
+ (NSColor *) colorFromInt: (unsigned int)colorCode
{
    NSColor *result = nil;
	unsigned char red, green, blue;

    red		= (unsigned char) (colorCode >> 16);
	green	= (unsigned char) (colorCode >> 8);
	blue	= (unsigned char) (colorCode);	// masks off high bits
    
	result = [NSColor colorWithCalibratedRed:(float)red/0xff
                                       green:(float)green/0xff
                                        blue:(float)blue/0xff
                                       alpha:1.0];
	return result;
}

//
- (NSString*) lastColorHexString
{
    return [Blink1 toHexColorString:lastColor];
}

/*
 NSColor: Instantiate from Web-like Hex RRGGBB string
 Original Source: <http://cocoa.karelia.com/Foundation_Categories/NSColor__Instantiat.m>
 (See copyright notice at <http://cocoa.karelia.com>)
 */
+ (NSColor *) colorFromHexRGB:(NSString *) inColorString
{
    NSString* hexNum = [inColorString substringFromIndex:1];
    unsigned int colorCode;
    NSColor *result = nil;
    
	if (hexNum != nil) {
		NSScanner *scanner = [NSScanner scannerWithString:hexNum];
		(void) [scanner scanHexInt:&colorCode];	// ignore error
        result = [Blink1 colorFromInt: colorCode];
	}
    
    return result;
}

//
+ (NSString*) toHexColorString: (NSColor*)colr
{
    return [NSString stringWithFormat:@"#%0.2X%0.2X%0.2X",
            (int)(255 * [colr redComponent]),
            (int)(255 * [colr blueComponent]),
            (int)(255 * [colr greenComponent])];
}


@end
