//
//  Pattern.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 10/10/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import "Blink1Pattern.h"

@implementation Blink1Pattern

@synthesize name;
@synthesize colors;
@synthesize times;
@synthesize repeats;
@synthesize playpos;
@synthesize playcount;
@synthesize mode;
@synthesize blink1;

//
// patternstr format "name,repeats,color1,color1time,color2,c2time,..."
//
- (id)initWithPatternString:(NSString *)patternstr name:(NSString*)namestr
{
    self = [super init];
    name = [NSString stringWithString:namestr];
    
    NSArray* parts = [patternstr componentsSeparatedByString:@","];
    
    repeats = [[parts objectAtIndex:0] intValue];
    playpos = 0;
    playcount = 0;

    colors = [NSMutableArray array];
    times  = [NSMutableArray array];
  
    int len2x = (int)[parts count] - 1;
    if( (len2x % 2) == 0 ) {    // even number, so good
        for( int i=0; i<len2x; i+=2 ) {
            NSString* colorstr = [parts objectAtIndex:1+i+0];
            NSString* timestr  = [parts objectAtIndex:1+i+1];
            NSColor* color = [Blink1 colorFromHexRGB:colorstr];
            NSNumber* secs = [NSNumber numberWithFloat:[timestr floatValue]];
            [colors addObject: color];
            [times  addObject: secs];
        }
    }
    else {
        self = nil;
    }
    
    return self;
}

//
- (void) play
{
    NSTimeInterval nextTime = [[times objectAtIndex:playpos] doubleValue];
    NSColor* color = [colors objectAtIndex:playpos];
    playpos = 0; playcount = 0;
    NSLog(@"play: p:%d c:%d color: %@ nextTime:%f",playpos,playcount,[Blink1 toHexColorString:color],nextTime);
    timer = [NSTimer timerWithTimeInterval:nextTime target:self selector:@selector(update) userInfo:nil repeats:NO];
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
}

//
- (void) stop
{
    NSLog(@"stop!");
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(update) object:nil];
}

// called by the player periodically
- (void) update
{
    Boolean stopit = false;
    playpos++;
    if( playpos == [times count] ) {
        playpos = 0;
        if( repeats != 0 ) {  // infinite
            playcount++;
            if( playcount == repeats ) {
                stopit = true;
            }
        }
    }
    
    if( stopit ) {
        [self stop];
    } else {
        NSTimeInterval nextTime = [[times objectAtIndex:playpos] doubleValue];
        NSColor* color = [colors objectAtIndex:playpos];
        NSLog(@"updt: p:%d c:%d %@ nextTime:%f",playpos,playcount,[Blink1 toHexColorString:color],nextTime);
        timer = [NSTimer timerWithTimeInterval:nextTime target:self selector:@selector(update) userInfo:nil repeats:NO];
        [[NSRunLoop currentRunLoop] addTimer:timer forMode:NSRunLoopCommonModes];
    }
}

// render pattern back to a string format
- (NSString*)patternString
{
    NSMutableArray* pattern = [NSMutableArray array];
    [pattern addObject:[NSNumber numberWithInt:repeats]];
    for( int i=0; i< [colors count]; i++) {
        [pattern addObject:[Blink1 toHexColorString: [colors objectAtIndex:i]]];
        [pattern addObject:[[times  objectAtIndex:i] stringValue]];
    }
    NSString* patternstr = [pattern componentsJoinedByString:@","];
    return patternstr;
}

- (NSString*) description
{
    return [NSString stringWithFormat:
            @"Blink1Pattern: name=%@, repeats=%d playpos=%d, playcount=%d",
            name,repeats,playpos,playcount];
}


// used by SBJson
- (NSDictionary*) proxyForJson
{
    return [NSDictionary dictionaryWithObjectsAndKeys:
            self.name, @"name",
            [self patternString], @"pattern",
            nil];
}

@end
