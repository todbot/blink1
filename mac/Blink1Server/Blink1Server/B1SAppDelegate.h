//
//  B1SAppDelegate.h
//  Blink1Server
//
//  Created by Tod E. Kurt on 8/31/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

#import "SBJson.h"

@class RoutingHTTPServer;


@interface B1SAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (strong) IBOutlet WebView *webView;

@property (assign) IBOutlet NSMenuItem *blink1status;
@property (assign) IBOutlet NSMenuItem *blink1serial;

@property (strong) RoutingHTTPServer *http;

@property (assign) IBOutlet NSMenu *statusMenu;
@property (retain) NSStatusItem * statusItem;

@property (retain)  SBJsonWriter *_jsonwriter;
@property (retain)  SBJsonParser *_jsonparser;

@end
