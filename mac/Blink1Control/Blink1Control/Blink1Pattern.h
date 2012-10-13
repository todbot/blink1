//
//  Pattern.h
//  Blink1Control
//
//  Created by Tod E. Kurt on 10/10/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "Blink1.h"

typedef enum {
    PatternStart,
    PatternChange,
    PatternStop,
} modetype;

@interface Blink1Pattern : NSObject {
    NSTimer * timer;
}

@property (retain) NSString* name;
@property (retain) NSMutableArray* colors;
@property (retain) NSMutableArray* times;
@property (weak) Blink1* blink1;

@property int repeats;
@property int playpos;
@property int playcount;
@property int mode;


- (id)initWithPatternString:(NSString *)patternstr name:(NSString*)namestr;

- (void) update;
- (void) play;
- (void) stop;

- (NSString*)patternString;
- (NSString*)description;
- (NSDictionary*) proxyForJson;


@end
