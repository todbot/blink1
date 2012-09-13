//
//  AudioListener.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 9/6/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import "AudioListener.h"

#import <QTKit/QTKit.h>
//#import <AVFoundation/AVFoundation.h>

@implementation AudioListener


- (void) setup
{
    // Create a capture session
	session = [[QTCaptureSession alloc] init];
	// Attach outputs to session
	movieFileOutput = [[QTCaptureMovieFileOutput alloc] init];
	[movieFileOutput setDelegate:self];
	[session addOutput:movieFileOutput error:nil];
    
    // Start updating the audio level meter
	audioLevelTimer = [NSTimer scheduledTimerWithTimeInterval:0.1 target:self selector:@selector(updateAudioLevels:) userInfo:nil repeats:YES];


}

#pragma mark UI updating
- (void)updateAudioLevels:(NSTimer *)timer
{
	// Get the mean audio level from the movie file output's audio connections
	
	float totalDecibels = 0.0;
	
	QTCaptureConnection *connection = nil;
	NSUInteger i = 0;
	NSUInteger numberOfPowerLevels = 0;	// Keep track of the total number of power levels in order to take the mean
	
	for (i = 0; i < [[movieFileOutput connections] count]; i++) {
		connection = [[movieFileOutput connections] objectAtIndex:i];
		
		if ([[connection mediaType] isEqualToString:QTMediaTypeSound]) {
			NSArray *powerLevels = [connection attributeForKey:QTCaptureConnectionAudioAveragePowerLevelsAttribute];
			NSUInteger j, powerLevelCount = [powerLevels count];
			
			for (j = 0; j < powerLevelCount; j++) {
				NSNumber *decibels = [powerLevels objectAtIndex:j];
				totalDecibels += [decibels floatValue];
				numberOfPowerLevels++;
			}
		}
	}
    
    float audioLevel = 0.0;
	if (numberOfPowerLevels > 0) {
        audioLevel = (pow(10., 0.05 * (totalDecibels / (float)numberOfPowerLevels)) * 20.0);
	}
    NSLog(@"audioLevel: %f",audioLevel);
}


@end
