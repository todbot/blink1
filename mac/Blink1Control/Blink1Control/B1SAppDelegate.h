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
#import "Netuse.h"

#import "Blink1.h"
#import "Blink1Pattern.h"


@class RoutingHTTPServer;


//@interface B1SAppDelegate : NSObject <NSApplicationDelegate,VDKQueueDelegate> {
@interface B1SAppDelegate : NSObject <NSApplicationDelegate> {
    
    CPUuse* cpuuse;
    Netuse* netuse;
    
    SBJsonWriter *_jsonwriter;
    SBJsonParser *_jsonparser;
    
    NSTimer * inputsTimer;
    BOOL inputsEnable;
    
    NSMutableDictionary *inputs;
    NSMutableDictionary *patterns;
    
    NSImage* statusImageBase;
}

@property (strong) RoutingHTTPServer *http;
@property (strong) Blink1 *blink1;

@property (assign) IBOutlet NSWindow *window;
@property (strong) IBOutlet WebView *webView;

@property (assign) IBOutlet NSMenuItem *blink1status;
@property (assign) IBOutlet NSMenuItem *blink1serial;

@property (assign) IBOutlet NSMenu *statusMenu;
@property (retain) NSStatusItem * statusItem;
@property (retain) NSImage *statusImage;
//@property (retain) NSImage *statusHighlightImage;



@end
