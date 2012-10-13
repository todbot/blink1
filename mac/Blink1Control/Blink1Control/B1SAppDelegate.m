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


// called every 100ms
- (void) playPattern: (NSString*)pname
{
    //if( pname != nil ) {
    //}
    NSString* key;
    for( key in patterns) {
        Blink1Pattern* pattern = [patterns objectForKey:key];
        [pattern update];
    }
    
}

//
- (NSString*) getContentsOfUrl: (NSString*) urlstr
{
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:urlstr]];
    NSData *response = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
    NSString *responseStr = [[NSString alloc] initWithData:response encoding:NSUTF8StringEncoding];
    return responseStr;
}

//
// Search for 'pattern: "pattern name"' in contentStr
// contentStr can also be JSON
// returns pattern name if successful, or 'nil' if no pattern found
//
- (NSString*) readColorPattern: (NSString*)contentStr
{
    NSString* str = nil;
    NSScanner *scanner = [NSScanner scannerWithString:contentStr];
    Boolean isPattern = [scanner scanUpToString:@"pattern" intoString:&str];
    if( isPattern || (!isPattern && str==nil) ) { // match or at begining of string
        [scanner scanString:@"pattern" intoString:NULL]; // consume 'pattern'
        [scanner scanUpToString:@":" intoString:NULL];   // read colon
        [scanner scanString:@":" intoString:NULL];       // consume colon
        [scanner scanUpToString:@"\"" intoString:NULL];  // read open quote
        [scanner scanString:@"\"" intoString:NULL];      // consume open quote
        [scanner scanUpToString:@"\"" intoString:&str];  // read string
    }
    return str;
}



// for "watchfile" functionality
- (void)updateWatchFile:(NSString*)wPath
{
    NSLog(@"updateWatchFile %@",wPath);
    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:wPath];
    if( !fileExists ) { // if no file, make one to watch, with dummy content
        NSString *content = @"Put this in a file please.";
        NSData *fileContents = [content dataUsingEncoding:NSUTF8StringEncoding];
        [[NSFileManager defaultManager] createFileAtPath:wPath
                                                contents:fileContents
                                              attributes:nil];
    }

    if( myVDKQ != nil ) [myVDKQ removePath:wPath];
    [myVDKQ addPath:wPath];
    watchFileChanged = true;
}

// for "watchfile" functionality
-(void) VDKQueue:(VDKQueue *)queue receivedNotification:(NSString*)noteName forPath:(NSString*)fpath;
{
    NSLog(@"watch file: %@ %@", noteName, fpath);
    if( [noteName isEqualToString:@"VDKQueueFileWrittenToNotification"] ) {
        NSLog(@"watcher: file written %@ %@", noteName, fpath);
        watchFileChanged = true;
    }
    // FIXME: this doesn't work
    if( [noteName isEqualToString:@"VDKQueueLinkCountChangedNotification"]) {
        NSLog(@"re-adding deleted file");
        [self updateWatchFile:fpath];
    }
}


//
- (void) watchUrl
{
    NSLog(@"watchUrl!");
    NSString* key;
    for( key in inputs) {
        NSMutableDictionary* input = [inputs objectForKey:key];
        NSString* type = [input valueForKey:@"type"];
        NSString* val  = [input valueForKey:@"value"];
        if( [type isEqualToString:@"url"]) {
            NSString *urlstr = val;
            NSString* respstr = [self getContentsOfUrl:urlstr];
            NSString* patternstr = [self readColorPattern:respstr];
            if( patternstr ) {  // pattern detected
                [self playPattern: patternstr];                
            }
            else { // is rgb hex string? convert to fake pattern
                //NSString* colorstr = [self readColorString:respstr];
                //if( colorstr )
                //[self playColor: colorstr];
                // else no pattern or rgb hex string detected
            }
        }
        //else if( [type isEqualToString:@"file"] ) {
        //}
    }
}

//
// a special case of watchUrl really
//
- (void) watchIfttt
{   
    if( !enableIftttWatch ) { return; }
    NSLog(@"iftttWatch!");
    NSString* blink1_uid = @"2023abcdf";
    NSString* baseEventUrl = @"http://api.thingm.com/blink1/events";
    NSString* eventUrlStr = [NSString stringWithFormat:@"%@/%@", baseEventUrl, blink1_uid];

    NSString* jsonStr = [self getContentsOfUrl: eventUrlStr];
    NSLog(@"got string: %@",jsonStr);
    
    id object = [_jsonparser objectWithString:jsonStr];
    NSDictionary* list = [(NSDictionary*)object objectForKey:@"events"];
    for (NSDictionary *event in list) {
        NSString * bl1_id     = [event objectForKey:@"blink1_id"];
        NSString * bl1_name   = [event objectForKey:@"name"];
        NSString * bl1_source = [event objectForKey:@"source"];
        NSLog(@"bl1_id:%@, name:%@, source:%@", bl1_id, bl1_name, bl1_source);
    }
}

//
// trigger color patterns when they need to be
//
- (void) updatePattern
{
    //NSLog(@"updatePattern");
    NSString* key;
    for( key in patterns) {
        
    }
}



//
- (void) loadPrefs
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSDictionary *inputspref = [prefs dictionaryForKey:@"inputs"];
    NSDictionary *patternspref = [prefs dictionaryForKey:@"patterns"];
    
    if( inputspref != nil ) {    NSLog(@"inputspref: %@", [_jsonwriter stringWithObject:inputspref]);
        [inputs addEntriesFromDictionary:inputspref];
    }
    if( patternspref != nil ) {  NSLog(@"patternspref: %@", [_jsonwriter stringWithObject:patternspref]);
        [patterns addEntriesFromDictionary:patternspref];
    }
}

//
- (void) savePrefs
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setObject:inputs forKey:@"inputs"];
    [prefs synchronize];
}

// ----------------------------------------------------------------------------
// Start
// ----------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    inputs   = [[NSMutableDictionary alloc] init];
    patterns = [[NSMutableDictionary alloc] init];

    // set up json parser
    _jsonparser = [[SBJsonParser alloc] init];
    _jsonwriter = [[SBJsonWriter alloc] init];
    _jsonwriter.humanReadable = YES;
    _jsonwriter.sortKeys = YES;

    [self loadPrefs];
    
    [self setupHttpServer];
    
    // set up file watcher
    myVDKQ = [[VDKQueue alloc] init];
    [myVDKQ setDelegate:self];
    [self updateWatchFile:@"/Users/tod/tmp/blink1-colors.txt"];
    
    // set up url watcher
    float timersecs = 10.0;
    iftttWatchTimer = [NSTimer scheduledTimerWithTimeInterval:timersecs
                                                       target:self
                                                     selector:@selector(watchIfttt)
                                                     userInfo:nil
                                                      repeats:YES];
    urlWatchTimer = [NSTimer scheduledTimerWithTimeInterval:timersecs
                                                       target:self
                                                     selector:@selector(watchUrl)
                                                     userInfo:nil
                                                      repeats:YES];
    patternTimer = [NSTimer scheduledTimerWithTimeInterval:0.1
                                                    target:self
                                                  selector:@selector(updatePattern)
                                                  userInfo:nil
                                                   repeats:YES];

    enableIftttWatch = false;
    //enableUrlWatch = false;

    // set up cpu use measurement tool
    cpuuse = [[CPUuse alloc] init];
    [cpuuse setup]; // FIXME: how to put stuff in init

    // set up blink(1) library
    blink1 = [[Blink1 alloc] init];
    serialnums = [blink1 enumerate]; 
    [self updateUI];  //FIXME: right way to do this?


    // test Task
	NSString*	result;
    result = [Task runWithToolPath:@"/usr/bin/grep" arguments:[NSArray arrayWithObject:@"france"] inputString:@"bonjour!\nvive la france!\nau revoir!" timeOut:0.0];
    NSLog(@"result: %@", result);
	
	result = [Task runWithToolPath:@"/bin/sleep" arguments:[NSArray arrayWithObject:@"2"] inputString:nil timeOut:1.0];
    NSLog(@"result: %@", result);
    
}


// set up local http server
- (void)setupHttpServer
{
    self.http = [[RoutingHTTPServer alloc] init];
    
	// Set a default Server header in the form of YourApp/1.0
	NSDictionary *bundleInfo = [[NSBundle mainBundle] infoDictionary];
	NSString *appVersion = [bundleInfo objectForKey:@"CFBundleShortVersionString"];
	if (!appVersion) appVersion = [bundleInfo objectForKey:@"CFBundleVersion"];

	NSString *serverHeader = [NSString stringWithFormat:@"%@/%@",
							  [bundleInfo objectForKey:@"CFBundleName"],
							  appVersion];
	[http setDefaultHeader:@"Server" value:serverHeader];
    
    [self setupHttpRoutes];
    
	// Server on port 8080 serving files from our embedded Web folder
	[http setPort:8080];
	NSString *htmlPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"html"];
	[http setDocumentRoot:htmlPath];
    NSLog(@"htmlPath: %@",htmlPath);
    
	NSError *error;
	if (![http start:&error]) {
		NSLog(@"Error starting HTTP server: %@", error);
	}

}

//-----------------------------------------------------------------------------
// set up routes
// ----------------------------------------------------------------------------
- (void)setupHttpRoutes
{
	[http get:@"/blink1" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:serialnums forKey:@"blink1_serialnums"];
        [respdict setObject:@"blink1" forKey:@"status"];
        
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
	}];
    
    [http get:@"/blink1/enumerate" withBlock:^(RouteRequest *request, RouteResponse *response) {
        
        serialnums = [blink1 enumerate];
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:serialnums forKey:@"blink1_serialnums"];
        [respdict setObject:@"linkBlink1s" forKey:@"status"];
        
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/fadeToRGB" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* rgbstr = [request param:@"rgb"];
        NSString* timestr = [request param:@"time"];
        if( rgbstr==nil ) rgbstr = @"";
        if( timestr==nil ) timestr = @"";
        NSColor * colr = [Blink1 colorFromHexRGB: rgbstr];
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
    
    // color patterns
    
    [http get:@"/blink1/pattern" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* statusstr = @"pattern results";
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
        [respdict setObject:[patterns allValues] forKey:@"patterns"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/pattern/add" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname      = [request param:@"pname"];
        NSString* patternstr = [request param:@"pattern"];
        Blink1Pattern* pattern = nil;
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];

        if( pname != nil && patternstr != nil ) {
            //[blink1controller addPattern:patternstr name:pname];
            //[self addPattern:patternstr name:pname];
            pattern = [[Blink1Pattern alloc] initWithPatternString:patternstr name:pname];
            [patterns setObject:pattern forKey:pname];

            [respdict setObject:pattern forKey:@"pattern"];
        }
        
        [respdict setObject:@"pattern add" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/pattern/del" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        NSString* statusstr = @"no such input";
        if( pname != nil ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            if( pattern != nil ) {
                //[blink1controller removePatternWithName:pname];
                [patterns removeObjectForKey:pname];
                statusstr = [NSString stringWithFormat:@"pattern %@ removed", pname];
            } else {
                statusstr = @"no such pattern";
            }
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/pattern/play" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        if( pname != nil ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            if( pattern != nil ) {
                [pattern play];
            }
        }
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"play" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/pattern/stop" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        if( pname != nil ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            if( pattern != nil ) {
                [pattern stop];
            }
        }
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"stop" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    
    // inputs
    
    [http get:@"/blink1/input" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* statusstr = @"input results";
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        //[[blink1controller inputs] allValues];
        [respdict setObject:[inputs allValues] forKey:@"inputs"];
        [respdict setObject:statusstr forKey:@"status"];
        
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/input/del" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        
        NSString* statusstr = @"no such input";
        if( iname != nil ) {
            NSDictionary *input = [inputs objectForKey:iname];
            if( input != nil ) {
                NSString* type = [input objectForKey:@"type"];
                if( [type isEqualToString:@"file"] ) {
                    NSString* path = [input objectForKey:@"value"];
                    NSLog(@"removed path %@",path);
                    [myVDKQ removePath:path];
                }
                [inputs removeObjectForKey:iname];
                statusstr = [NSString stringWithFormat:@"input %@ removed", iname];
            } else {
                statusstr = @"no such input";
            }
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
        
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/input/file" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* path  = [request param:@"path"];
        NSString* pname = [request param:@"pname"];
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];

        if( iname != nil && path != nil ) {
            NSString* fpath = [path stringByExpandingTildeInPath];
            if( pname != nil ) pname = iname;
            [input setObject:iname forKey:@"iname"];
            [input setObject:@"file" forKey:@"type"];
            [input setObject:fpath forKey:@"value"];
            [input setObject:pname forKey:@"pname"];
            [inputs setObject:input forKey:iname];
            
            [self performSelectorOnMainThread:@selector(updateWatchFile:)
                                   withObject:fpath
                                waitUntilDone:NO];

            NSLog(@"watching file %@",fpath);
        }
        else {
            //path = watchPath;
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"watchfile" forKey:@"status"];
        [respdict setObject:input forKey:@"input"];

        if( watchFileChanged ) {
            NSString* filecontents = [NSString stringWithContentsOfFile:path
                                                               encoding:NSUTF8StringEncoding error:nil];
            [respdict setObject:filecontents forKey:@"new_event"];
            watchFileChanged = false;
        }

        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/input/url" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* url   = [request param:@"url"];
        NSString* pname = [request param:@"pname"];
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        
        if( iname != nil && url != nil ) {
            if( pname == nil ) pname = iname;
            [input setObject:iname forKey:@"iname"];
            [input setObject:@"url" forKey:@"type"];
            [input setObject:url forKey:@"value"];
            [input setObject:pname forKey:@"pname"];
            [inputs setObject:input forKey:iname];
        }
        
        [input setObject:@"watchurl" forKey:@"status"];
        
        [response respondWithString: [_jsonwriter stringWithObject:input]];
    }];
    
    [http get:@"/blink1/input/ifttt" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* enable = [request param:@"enable"];
        if( enable != nil ) {
            enableIftttWatch = [enable isEqualToString:@"on"];
        }
        enable = [NSString stringWithFormat:@"%s",((enableIftttWatch)?"on":"off")];
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:enable forKey:@"status"];
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

// unused
- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response
{
	[response respondWithString:@"Handled through selector"];
}

//
- (void) awakeFromNib
{
    [self activateStatusMenu];
}

//
- (void) activateStatusMenu
{
    statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [statusItem setTitle: NSLocalizedString(@"blink(1)",@"")];
    [statusItem setHighlightMode:YES];
    [statusItem setMenu:statusMenu];  // instead, we'll do it by hand
    
    //[statusItem setAction:@selector(openStatusMenu:)];
    //[statusItem setTarget:self];
}

//FIXME: what's the better way of doing this?
- (void) updateUI
{
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
- (IBAction) openStatusMenu: (id) sender
{
    serialnums = [blink1 enumerate];
    [self updateUI];
    
    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    [statusItem popUpStatusItemMenu:statusMenu];
}

//
- (IBAction) openConfig: (id) sender
{
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
- (IBAction) playIt: (id) sender
{
    NSLog(@"Play!");

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:playURL]]];
    [_window display];
    [_window setIsVisible:YES];
}

//
- (IBAction) reScan: (id) sender
{
    serialnums = [blink1 enumerate];
    [self updateUI];
    
    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    [statusItem popUpStatusItemMenu:statusMenu];
    
}

//
- (IBAction) quit: (id) sender
{
    NSLog(@"Quit!");
    [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}



@end


/*
 NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
 NSString *myString = [prefs stringForKey:@"keyToLookupString"];      // getting an NSString
 if( myString == nil ) {
 // saving an NSString
 [prefs setObject:@"TextToSave" forKey:@"keyToLookupString"];
 }
 */
/*
 NSString* iname = @"myTodInput";
 NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
 [input setObject:iname forKey:@"iname"];
 [input setObject:@"todftt" forKey:@"type"];
 [input setObject:@"blargh" forKey:@"value"];
 [inputs setObject:input forKey:iname];
 */




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
