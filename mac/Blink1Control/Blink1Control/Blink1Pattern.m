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
@synthesize playing;
@synthesize mode;
@synthesize blink1;


- (Boolean) setupFromPatternString:(NSString*)patternstr
{
    NSArray* parts = [patternstr componentsSeparatedByString:@","];
    
    repeats = [[parts objectAtIndex:0] intValue];
    playpos = 0;
    playcount = 0;
    colors = [NSMutableArray array];
    times  = [NSMutableArray array];
    playing = false;
    
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
        return false;
    }
    return true;
}

//
// patternstr format "name,repeats,color1,color1time,color2,c2time,..."
//
- (id)initWithPatternString:(NSString *)patternstr name:(NSString*)namestr
{
    self = [super init];
    name = [NSString stringWithString:namestr];

    if( ![self setupFromPatternString:patternstr] ) {
        self = nil;
    }
    
    return self;
}

//
- (void) play
{
    NSTimeInterval nextTime = [[times objectAtIndex:playpos] doubleValue];
    NSColor* color = [colors objectAtIndex:playpos];
    playpos = 0;
    playcount = 0;
    playing = true;
    NSLog(@"%@ play p:%d c:%d %@ nextTime:%f",name,playpos,playcount,[Blink1 toHexColorString:color],nextTime);
    //NSLog(@"play: p:%d c:%d color: %@ nextTime:%f",playpos,playcount,[Blink1 toHexColorString:color],nextTime);
    [blink1 fadeToRGB:color atTime:nextTime/2];
    timer = [NSTimer timerWithTimeInterval:nextTime target:self selector:@selector(update) userInfo:nil repeats:NO];
    [[NSRunLoop mainRunLoop] addTimer:timer forMode:NSDefaultRunLoopMode];
}

//
- (void) stop
{
    NSLog(@"stop! %@",name);
    //[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(update) object:nil];
    //[[NSRunLoop mainRunLoop] cancelPerformSelector:@selector(update) target:self argument:nil];
    playing = false;
}

// called by the player periodically
- (void) update
{
    if( !playing ) return;
    Boolean scheduleNext = true;
    playpos++;
    if( playpos == [times count] ) {
        playpos = 0;
        if( repeats != 0 ) {  // infinite
            playcount++;
            if( playcount == repeats ) {
                scheduleNext = false;
            }
        }
    }
    
    if( scheduleNext ) {
        NSTimeInterval nextTime = [[times objectAtIndex:playpos] doubleValue];
        NSColor* color = [colors objectAtIndex:playpos];
        NSLog(@"%@ updt p:%d c:%d %@ nextTime:%f",name,playpos,playcount,[Blink1 toHexColorString:color],nextTime);
        [blink1 fadeToRGB:color atTime:nextTime/2];
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

// objective-c's "toString"
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


- (void) encodeWithCoder:(NSCoder *)encoder
{
    //Encode properties, other class variables, etc
    [encoder encodeObject:self.name forKey:@"name"];
    [encoder encodeObject:[self patternString] forKey:@"pattern"];
}

- (id) initWithCoder:(NSCoder *)decoder
{
    if((self = [super init])) {
        //decode properties, other class vars
        self.name = [decoder decodeObjectForKey:@"name"];
        NSString* patternstr = [decoder decodeObjectForKey:@"pattern"];
        [self setupFromPatternString:patternstr]; // note: discards return val
    }
    return self;
}

@end
