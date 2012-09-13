//
//  B1SAppDelegate.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 8/31/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//
//
// ToDo:
// - pattern timer
// - shell script runner
// - cpu load real-time input
// - network real-time load input
// - file watcher input (for dropbox)
// - ifttt input

#import "B1SAppDelegate.h"
#import "RoutingHTTPServer.h"


@interface B1SAppDelegate ()

- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response;

@end

@implementation B1SAppDelegate

@synthesize window = _window;
@synthesize webView = _webView;
@synthesize blink1status = _blink1status;
@synthesize blink1serial = _blink1serial;

@synthesize statusItem;
@synthesize statusMenu;

@synthesize http;
@synthesize blink1;


//FIXME: what to do with these URLs?
NSString* confURL =  @"http://127.0.0.1:8080/bootstrap/blink1.html";
NSString* playURL =  @"http://127.0.0.1:8080/colorpicker/index.html";

NSString* watchPath;
BOOL watchFileChanged;

// for "watchfile" functionality
- (void)updateFileWatcher:(NSString*)path
{
    NSLog(@"updateFileWatcher %@",path);
    watchPath = [path stringByExpandingTildeInPath];
    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:watchPath];
    if( !fileExists ) {
        NSString *content = @"Put this in a file please.";
        NSData *fileContents = [content dataUsingEncoding:NSUTF8StringEncoding];
        [[NSFileManager defaultManager] createFileAtPath:watchPath
                                                contents:fileContents
                                              attributes:nil];
    }

    if( myVDKQ != nil ) [myVDKQ removePath:watchPath];
    [myVDKQ addPath:watchPath];
    watchFileChanged = TRUE;
}

// for "watchfile" functionality
-(void) VDKQueue:(VDKQueue *)queue receivedNotification:(NSString*)noteName forPath:(NSString*)fpath;
{
    NSLog(@"watcher: %@ %@", noteName, fpath);
    if( [noteName isEqualToString:@"VDKQueueFileWrittenToNotification"] ) {
        NSLog(@"watcher: file written %@ %@", noteName, fpath);
        watchFileChanged = TRUE;
    }
    // FIXME: this doesn't work
    if( [noteName isEqualToString:@"VDKQueueLinkCountChangedNotification"]) {
        NSLog(@"re-adding deleted file");
        [self updateFileWatcher:watchPath];
    }
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
    NSLog(@"appVersion: %@",appVersion);
	NSString *serverHeader = [NSString stringWithFormat:@"%@/%@",
							  [bundleInfo objectForKey:@"CFBundleName"],
							  appVersion];
    NSLog(@"serverHeader: %@",serverHeader);
	[http setDefaultHeader:@"Server" value:serverHeader];
    
	[self setupRoutes];
	[http setPort:8080];
	//[http setDocumentRoot:[@"~/Sites" stringByExpandingTildeInPath]];
	// Serve files from our embedded Web folder
	NSString *htmlPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"html"];
    NSLog(@"htmlPath: %@",htmlPath);
	[http setDocumentRoot:htmlPath];
    
	NSError *error;
	if (![http start:&error]) {
		NSLog(@"Error starting HTTP server: %@", error);
	}
        
    // set up json parser
    _jsonparser = [[SBJsonParser alloc] init];
    _jsonwriter = [[SBJsonWriter alloc] init];
    _jsonwriter.humanReadable = YES;
    _jsonwriter.sortKeys = YES;

    myVDKQ = [[VDKQueue alloc] init];
    [myVDKQ setDelegate:self];
    [self updateFileWatcher:@"/Users/tod/tmp/blink1-colors.txt"];
    
    // set up blink(1) library
    self.blink1 = [[Blink1 alloc]init];

    serialnums = [blink1 enumerate]; 
    [self updateUI];  //FIXME: right way to do this?

    // test Task
	NSString*	result;
    result = [Task runWithToolPath:@"/usr/bin/grep" arguments:[NSArray arrayWithObject:@"france"] inputString:@"bonjour!\nvive la france!\nau revoir!" timeOut:0.0];
    NSLog(@"result: %@", result);
	
	result = [Task runWithToolPath:@"/bin/sleep" arguments:[NSArray arrayWithObject:@"2"] inputString:nil timeOut:1.0];
    NSLog(@"result: %@", result);
    
    cpuuse = [[CPUuse alloc] init];
    [cpuuse setup]; // FIXME: how to put stuff in init
    
}

//
- (void)setupRoutes {
	[http get:@"/blink1" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:serialnums forKey:@"blink1_serialnums"];
        [respdict setObject:@"blink1" forKey:@"status"];
        
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];

	}];
    
    [http get:@"/blink1/fadeToRGB" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* rgbstr = [request param:@"rgb"];
        NSString* timestr = [request param:@"time"];
        if( rgbstr==nil ) rgbstr = @"";
        if( timestr==nil ) timestr = @"";
        NSColor * colr = [blink1 colorFromHexRGB: rgbstr];
        float secs = 0.1;
        [[NSScanner scannerWithString:timestr] scanFloat:&secs];

        [blink1 fadeToRGB:colr atTime:secs];

        NSString* statusstr = [NSString stringWithFormat:@"fadeToRGB: %@ t:%2.3f",colr,secs];
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:rgbstr forKey:@"rgb"];
        [respdict setObject:[NSString stringWithFormat:@"%2.3f",secs] forKey:@"time"];
        [respdict setObject:statusstr forKey:@"status"];

		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
	}];

    [http get:@"/blink1/list" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        serialnums = [blink1 enumerate];
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:serialnums forKey:@"blink1_serialnums"];
        [respdict setObject:@"linkBlink1s" forKey:@"status"];

        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/input/watchfile" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* path = [request param:@"path"];
        
        if( path != nil ) {
            [self performSelectorOnMainThread:@selector(updateFileWatcher:)
                                   withObject:path
                                waitUntilDone:NO];

            NSLog(@"watching file %@",path);
        }
        else {
            path = watchPath;
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"watchfile" forKey:@"status"];
        [respdict setObject:path forKey:@"path"];

        if( watchFileChanged ) {
            NSString* filecontents = [NSString stringWithContentsOfFile:watchPath
                                                               encoding:NSUTF8StringEncoding error:nil];
            [respdict setObject:filecontents forKey:@"new_event"];
            watchFileChanged = FALSE;
        }

        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/input/cpuload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        int cpuload = [cpuuse getCPUuse];
        NSLog(@"cpu use:%d%%",cpuload);

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[NSNumber numberWithInt:cpuload] forKey:@"cpuload"];
        [respdict setObject:@"cpuload" forKey:@"status"];
        
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/input/netload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"netload" forKey:@"status"];
        [respdict setObject:@"not implemented" forKey:@"error"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
        
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
    [statusItem setTitle: NSLocalizedString(@"blink(1)",@"")];
    [statusItem setHighlightMode:YES];
    [statusItem setMenu:statusMenu];  // instead, we'll do it by hand
    
    //[statusItem setAction:@selector(openStatusMenu:)];
    //[statusItem setTarget:self];
}

//FIXME: what's the better way of doing this?
- (void) updateUI {
    if( [serialnums count] ) {
        NSString* serstr = [serialnums objectAtIndex:0];
        [_blink1serial setTitle: [NSString stringWithFormat:@"serial:%@",serstr]];
        [_blink1status setTitle: @"blink(1) found"];
    }
    else {
        [_blink1serial setTitle: @"serial:-none-"];
        [_blink1status setTitle: @"blink(1) not found"];
    }

}

//
- (IBAction) openStatusMenu: (id) sender {
    
    serialnums = [blink1 enumerate];
    [self updateUI];
    
    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    [statusItem popUpStatusItemMenu:statusMenu];
}

//
- (IBAction) openConfig: (id) sender {
    NSLog(@"Config!");
    serialnums = [blink1 enumerate];
    [self updateUI];
    
    // Load the HTML content.
    //[[[_webView mainFrame] frameView] setAllowsScrolling:NO];

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:confURL]]];
    [NSApp activateIgnoringOtherApps:YES];
    [_window display];
    [_window setIsVisible:YES];
}

//
- (IBAction) playIt: (id) sender {
    NSLog(@"Play!");

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:playURL]]];
    [_window display];
    [_window setIsVisible:YES];
}

//
- (IBAction) reScan: (id) sender {
    serialnums = [blink1 enumerate];
    [self updateUI];
    
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




/*
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
 [http handleMethod:@"GET" withPath:@"/selector" target:self     selector:@selector(handleSelectorRequest:withResponse:)];
 */


/*
 NSString *jsonString = @"{\"tod\":1, \"bar\":2, \"garb\":\"gobble\", \"arrr\":[ 3,6,89] }";
 
 id object = [_jsonparser objectWithString:jsonString];
 //if (object) {
 NSLog(@"val:%@",[_jsonwriter stringWithObject:object]);
 //} else {
 NSLog(@"error:%@",[NSString stringWithFormat:@"An error occurred: %@", _jsonparser.error]);
 //}
 
 NSMutableDictionary *dictionary = [[NSMutableDictionary alloc] init];
 [dictionary setObject:@"tod kurt" forKey:@"todbot"];
 [dictionary setObject:@"carlyn maw" forKey:@"carlynorama"];
 NSLog(@"dict:%@",[_jsonwriter stringWithObject:dictionary]);
 */
