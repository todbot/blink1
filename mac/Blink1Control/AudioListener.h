//
//  AudioListener.h
//  Blink1Control
//
//  Created by Tod E. Kurt on 9/6/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Foundation/Foundation.h>

@class QTCaptureView;
@class QTCaptureSession;
@class QTCaptureDeviceInput;
@class QTCaptureMovieFileOutput;
@class QTCaptureAudioPreviewOutput;
@class QTCaptureConnection;
@class QTCaptureDevice;


@interface AudioListener : NSObject {
    QTCaptureSession			*session;

    QTCaptureMovieFileOutput	*movieFileOutput;
    NSTimer						*audioLevelTimer;
  
}

@end
