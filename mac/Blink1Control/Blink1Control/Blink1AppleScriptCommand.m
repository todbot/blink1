//
//  Blink1AppleScriptCommand.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 1/31/13.
//  Copyright (c) 2013 ThingM. All rights reserved.
//

#import "Blink1AppleScriptCommand.h"
#import "B1SAppDelegate.h"

@implementation Blink1AppleScriptCommand


- (id)performDefaultImplementation
{
    // get the arguments
    NSDictionary *args = [self evaluatedArguments];
    DLog(@"The direct parameter is: '%@'", [self directParameter]);
	DLog(@"The other parameters are: '%@'", args);

    NSString* cmd = [self directParameter];
    
    B1SAppDelegate* b1sad = (B1SAppDelegate*)[[NSApplication sharedApplication] delegate];
    Blink1* blink1 = [b1sad blink1];

    NSString* respstr = @"no command";
    
    if( [cmd isEqualToString:@"fadeToRGB"] ) {
        NSString* rgbstr = [args valueForKey:@"rgb"];
        NSNumber* time = [args valueForKey:@"time"];
        NSColor * colr = [Blink1 colorFromHexRGB: rgbstr];
        float secs = [time floatValue];
        
        [blink1 fadeToRGB:colr atTime:secs];
        respstr = [NSString stringWithFormat:@"fadeToRGB color %@ at time: %f",rgbstr,secs];
    }
        
    return respstr;
}

@end
