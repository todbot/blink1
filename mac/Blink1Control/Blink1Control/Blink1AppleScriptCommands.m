//
//  Blink1AppleScriptCommands.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 2/1/13.
//  Copyright (c) 2013 ThingM. All rights reserved.
//

#import "Blink1AppleScriptCommands.h"

@implementation Blink1AppleScriptCommands

-(NSString*)doIt:(NSScriptCommand *)command
{
    DLog(@"doIt: ");
    DLog(@"%@",command);
    return @"i did it!";
}


@end
