//
//  B1SAppDelegate.h
//  Blink1Control
//
//  Created by Tod E. Kurt on 8/31/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

#import "SBJson.h"
#import "Task.h"
#import "VDKQueue.h"

#import "CPUuse.h"

#import "Blink1.h"


@class RoutingHTTPServer;


@interface B1SAppDelegate : NSObject <NSApplicationDelegate,VDKQueueDelegate> {
    
    NSMutableArray* serialnums;
    CPUuse* cpuuse;
    
    SBJsonWriter *_jsonwriter;
    SBJsonParser *_jsonparser;
    
    VDKQueue * myVDKQ;
    //NSString* watchPath;
    Boolean watchFileChanged;

    NSTimer * iftttWatchTimer;
    Boolean enableIftttWatch;
    
    NSTimer * urlWatchTimer;
    Boolean enableUrlWatch;
    
    NSMutableDictionary *inputs;
    NSMutableDictionary *patterns;
}

@property (strong) RoutingHTTPServer *http;
@property (strong) Blink1 *blink1;

@property (assign) IBOutlet NSWindow *window;
@property (strong) IBOutlet WebView *webView;

@property (assign) IBOutlet NSMenuItem *blink1status;
@property (assign) IBOutlet NSMenuItem *blink1serial;

@property (assign) IBOutlet NSMenu *statusMenu;
@property (retain) NSStatusItem * statusItem;



@end
