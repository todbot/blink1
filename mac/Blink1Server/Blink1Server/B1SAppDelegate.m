//
//  B1SAppDelegate.m
//  Blink1Server
//
//  Created by Tod E. Kurt on 8/31/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//


#import "B1SAppDelegate.h"
#import "RoutingHTTPServer.h"

#include "blink1-lib.h"

@interface B1SAppDelegate ()

- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response;

@end

@implementation B1SAppDelegate

@synthesize window = _window;
@synthesize webView = _webView;
@synthesize blink1status = _blink1status;
@synthesize blink1serial = _blink1serial;

@synthesize http;

@synthesize statusItem;
@synthesize statusMenu;

@synthesize _jsonparser;
@synthesize _jsonwriter;


NSString* startURL =  @"http://127.0.0.1:8080/html5/bootstrap/blink1.html";

NSMutableArray* serialnums;

//
- (void)blink1fadeToRGB:(NSColor*) c atTime:(float) t
{
    CGFloat r,g,b;
    [c getRed:&r green:&g blue:&b alpha:NULL];
    r *= 255; g *= 255; b*=255;
    NSLog(@"updateBlink1: r,g,b:%d,%d,%d t:%f", (int)r,(int)g,(int)b,t);
    
    hid_device *dev = blink1_openById(0);
    blink1_fadeToRGB(dev, (int)(t*1000), (int)r,(int)g,(int)b );
    blink1_close(dev);
}
//
- (void)blink1enumerate {
    serialnums = [[NSMutableArray alloc] init];
    int count = blink1_enumerate();
    NSLog(@"found %d blink1s", count);
    for( int i=0; i< count; i++ ) {
        NSString * serstr = [[NSString alloc] initWithBytes:blink1_getCachedSerial(i)
                                                     length:8*4
                                                   encoding:NSUTF32LittleEndianStringEncoding];
        NSLog(@"serial: %@", serstr);
        [serialnums addObject: serstr];
    }
    
    if( count ) {
        NSString* serstr = [serialnums objectAtIndex:0];
        [_blink1serial setTitle: [NSString stringWithFormat:@"serial:%@",serstr]];
        [_blink1status setTitle: @"blink(1) found"];
    }
    else {
        [_blink1serial setTitle: @"serial:-none-"];
        [_blink1status setTitle: @"blink(1) not found"];
    }
}

/*
 NSColor: Instantiate from Web-like Hex RRGGBB string
 Original Source: <http://cocoa.karelia.com/Foundation_Categories/NSColor__Instantiat.m>
 (See copyright notice at <http://cocoa.karelia.com>)
 */
- (NSColor *) colorFromHexRGB:(NSString *) inColorString {
    NSString* hexNum = inColorString; //[inColorString substringFromIndex:1];
	NSColor *result = nil;
	unsigned int colorCode = 0;
	unsigned char red, green, blue;

	if (nil != inColorString) {
		NSScanner *scanner = [NSScanner scannerWithString:hexNum];
		(void) [scanner scanHexInt:&colorCode];	// ignore error
	}
	red		= (unsigned char) (colorCode >> 16);
	green	= (unsigned char) (colorCode >> 8);
	blue	= (unsigned char) (colorCode);	// masks off high bits
    
    NSLog(@"r:%d g:%d b:%d",red, green, blue);
	result = [NSColor
              colorWithCalibratedRed:(float)red/0xff green:(float)green/0xff blue:(float)blue/0xff alpha:1.0];
	return result;
}

//
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	self.http = [[RoutingHTTPServer alloc] init];
    
	// Set a default Server header in the form of YourApp/1.0
	NSDictionary *bundleInfo = [[NSBundle mainBundle] infoDictionary];
	NSString *appVersion = [bundleInfo objectForKey:@"CFBundleShortVersionString"];
	if (!appVersion) {
		appVersion = [bundleInfo objectForKey:@"CFBundleVersion"];
	}
	NSString *serverHeader = [NSString stringWithFormat:@"%@/%@",
							  [bundleInfo objectForKey:@"CFBundleName"],
							  appVersion];
	[http setDefaultHeader:@"Server" value:serverHeader];
    
	[self setupRoutes];
	[http setPort:8080];
	[http setDocumentRoot:[@"~/Sites" stringByExpandingTildeInPath]];
    
	NSError *error;
	if (![http start:&error]) {
		NSLog(@"Error starting HTTP server: %@", error);
	}
        
    _jsonparser = [[SBJsonParser alloc] init];
    _jsonwriter = [[SBJsonWriter alloc] init];
    _jsonwriter.humanReadable = YES;
    _jsonwriter.sortKeys = YES;

    /*
    NSString *jsonString = @"{\"tod\":1, \"bar\":2, \"garb\":\"gobble\", \"arrr\":[ 3,6,89] }";

    id object = [_jsonparser objectWithString:jsonString];
    //if (object) {
        NSLog(@"val:%@",[_jsonwriter stringWithObject:object]);
    //} else {
        NSLog(@"error:%@",[NSString stringWithFormat:@"An error occurred: %@", _jsonparser.error]);
    //}
    
    NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:@"bar" forKey:@"foo"];
    [dictionary setObject:@"tod kurt" forKey:@"todbot"];
    [dictionary setObject:@"carlyn maw" forKey:@"carlynorama"];
    NSLog(@"dict:%@",[_jsonwriter stringWithObject:dictionary]);
    */
    [self blink1enumerate];
}

//
- (void)setupRoutes {
	[http get:@"/hello" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        NSString* parmsstr = [_jsonwriter stringWithObject:[request params]];

        [response respondWithString:parmsstr];
	}];
    
    [http get:@"/blink1/fadeToRGB" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* statusstr = @"fadeToRGB";
        NSString* rgbstr = [request param:@"rgb"]; if( rgbstr==nil ) rgbstr = @"";
        NSColor * colr = [self colorFromHexRGB: rgbstr];
        float secs = 100;
        [[NSScanner scannerWithString:[request param:@"time"]] scanFloat:&secs];

        [self blink1fadeToRGB: colr atTime:secs];

        statusstr = [NSString stringWithFormat:@"fadeToRGB: %@ t:%f",colr,secs];

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:rgbstr forKey:@"rgb"];
        [respdict setObject:[NSNumber numberWithFloat:secs] forKey:@"time"];
        [respdict setObject:statusstr forKey:@"status"];

		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
	}];

    [http get:@"/blink1/list" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* statusstr = @"listBlink1s";
		[response respondWithString:statusstr];
        
        int count = blink1_enumerate();
        statusstr = [NSString stringWithFormat:@" count:%d",count];

        //NSString* parmsstr = [_jsonwriter stringWithObject:[request params]];

        [response respondWithString:statusstr];
        
    }];

	[http get:@"/hello/:name" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"Hello %@!", [request param:@"name"]]];
	}];
    
	[http get:@"{^/page/(\\d+)}" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"You requested page %@",
									 [[request param:@"captures"] objectAtIndex:0]]];
	}];
    
	[http post:@"/widgets" withBlock:^(RouteRequest *request, RouteResponse *response) {
		// Create a new widget, [request body] contains the POST body data.
		// For this example we're just going to echo it back.
		[response respondWithData:[request body]];
	}];
    
	// Routes can also be handled through selectors
	[http handleMethod:@"GET" withPath:@"/selector" target:self selector:@selector(handleSelectorRequest:withResponse:)];
}

//
- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response {
	[response respondWithString:@"Handled through selector"];
}


//
- (void) awakeFromNib {
    [self activateStatusMenu];
    
}

//
- (void) activateStatusMenu {
    statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    //[statusItem setMenu:statusMenu];  // instead, we'll do it by hand
    [statusItem setTitle: NSLocalizedString(@"blink(1)",@"")];
    [statusItem setHighlightMode:YES];
    
    [statusItem setAction:@selector(openStatusMenu:)];
    [statusItem setTarget:self];
}

//
- (IBAction) openConfig: (id) sender {
    NSLog(@"Config!");
    // Load the HTML content.
    //[[[_webView mainFrame] frameView] setAllowsScrolling:NO];

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:startURL]]];
    [_window display];
    [_window setIsVisible:YES];
}

//
- (IBAction) playIt: (id) sender {
    NSLog(@"Play!");
}

- (IBAction) openStatusMenu: (id) sender {
    NSLog(@"openStatusMenu!");
    [self blink1enumerate];

    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    
    [statusItem popUpStatusItemMenu:statusMenu];
}

//
- (IBAction) quit: (id) sender {
    NSLog(@"Quit!");
    [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}


@end
