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
//

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
@synthesize statusImage;
@synthesize statusHighlightImage;

@synthesize http;
@synthesize blink1;


//FIXME: what to do with these URLs?
NSString* confURL =  @"http://127.0.0.1:8080/blink_1/";
NSString* playURL =  @"http://127.0.0.1:8080/bootstrap/blink1.html";


//
- (void) playPattern: (NSString*)pname
{
    [self playPattern:pname restart:true];
}

// play a pattern
- (void) playPattern: (NSString*)pname restart:(Boolean)restart
{
    if( pname == nil ) return;
    Blink1Pattern* pattern = [patterns objectForKey:pname];
    if( pattern != nil ) {
        [pattern setBlink1:blink1];  // just in case
        if( ![pattern playing] || ([pattern playing] && restart) )
            [pattern play];
    }
}

// stop a currently playing pattern, or "all" to stop all patterns
- (void) stopPattern: (NSString*)pname
{
    if( pname == nil ) return;
    if( [pname isEqualToString:@"all"] ) {
        for( Blink1Pattern* pattern in [patterns allValues] ) {
            [pattern stop];
        }
    }
    else {
        Blink1Pattern* pattern = [patterns objectForKey:pname];
        if( pattern != nil )
            [pattern stop];
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
    BOOL isPattern = [scanner scanUpToString:@"pattern" intoString:&str];
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


// for "watchfile" functionality, should be put in its own class
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

// for "watchfile" functionality, should be put in its own class
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
// the main deal for triggering color patterns
//
- (void) updateInputs
{
    NSLog(@"updateInputs");
    if( !inputsEnable ) return;
    
    int cpuload = [cpuuse getCPUuse];
    
    NSString* key;
    for( key in inputs) {
        NSMutableDictionary* input = [inputs objectForKey:key];
        NSString* type  = [input valueForKey:@"type"];
        NSString* val   = [input valueForKey:@"value"];
        if( [type isEqualToString:@"url"]) {
            NSString* urlstr = val;
            NSString* respstr = [self getContentsOfUrl:urlstr];
            NSString* patternstr = [self readColorPattern:respstr];
            if( patternstr ) {  // pattern detected
                [self playPattern: patternstr];
            } else  {
                NSColor* colr = [Blink1 colorFromHexRGB:respstr];
                if( colr != nil ) {
                    NSLog(@"hex color: %@", [Blink1 toHexColorString:colr]);
                }
            }
        }
        else if( [type isEqualToString:@"file"] ) {
            // this is done using FSEvents
        }
        else if( [type isEqualToString:@"ifttt"] ) {
            
        }
        else if( [type isEqualToString:@"cpuload"] ) {
            int level = [val intValue];
            NSLog(@"cpuload:%d%% - level:%d",cpuload,level);
            if( cpuload >= level ) {
                [self playPattern: [input valueForKey:@"pname"] restart:NO];
            }
        }
        else if( [type isEqualToString:@"netload"] ) {
            
        }
    } //for(key)
}


//
// unused, but will be put inside "updateInputs" soon
// a special case of watchUrl really
//
- (void) watchIfttt
{   
    //if( !enableIftttWatch ) { return; }
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
- (void) loadPrefs
{
    inputs   = [[NSMutableDictionary alloc] init];
    patterns = [[NSMutableDictionary alloc] init];

    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    NSDictionary *inputspref  = [prefs dictionaryForKey:@"inputs"];
    NSData *patternspref      = [prefs objectForKey:@"patterns"];
    NSString* blink1_id_prefs = [prefs stringForKey:@"blink1_id"];
    NSString* host_id_prefs   = [prefs stringForKey:@"host_id"];
    //BOOL first_run            = [prefs boolForKey:@"first_run"];
    
    if( inputspref != nil ) {
        [inputs addEntriesFromDictionary:inputspref];
    }
    if( patternspref != nil ) {
        patterns = [NSKeyedUnarchiver unarchiveObjectWithData:patternspref];
        //for( Blink1Pattern* pattern in [patterns allValues] ) {
        //    [pattern setBlink1:blink1];
        //}
    }

    [blink1 setHost_id:host_id_prefs]; // accepts nil
    if( blink1_id_prefs != nil ) {
        [blink1 setBlink1_id:blink1_id_prefs];
    } else {
        [blink1 regenerateBlink1Id];
    }
    NSLog(@"blink1_id:%@",[blink1 blink1_id]);
    
    //if( !first_run ) {
    //}
}

//
- (void) savePrefs
{
    NSUserDefaults *prefs = [NSUserDefaults standardUserDefaults];
    [prefs setObject:inputs forKey:@"inputs"];
    NSData *data = [NSKeyedArchiver archivedDataWithRootObject:patterns];
    [prefs setObject:data forKey:@"patterns"];
    [prefs setObject:[blink1 blink1_id] forKey:@"blink1_id"];
    [prefs setObject:[blink1 host_id]   forKey:@"host_id"];
    [prefs synchronize];
}

// ----------------------------------------------------------------------------
// Start
// ----------------------------------------------------------------------------
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    srand([[NSDate date]  timeIntervalSince1970]);

    blink1 = [[Blink1 alloc] init];      // set up blink(1) library
    [blink1 enumerate];
    __weak id weakSelf = self; // FIXME: hmm, http://stackoverflow.com/questions/4352561/retain-cycle-on-self-with-blocks
    blink1.updateHandler = ^(NSColor *lastColor, float lastTime) {
        NSString* lastcolorstr = [Blink1 toHexColorString:lastColor];
        [[weakSelf window] setTitle:[NSString stringWithFormat:@"blink(1) control - %@",lastcolorstr]];
    };
     
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
    [self updateWatchFile:@"/Users/tod/tmp/blink1-colors.txt"];  //FIXME: test
    
    // set up input watcher
    float timersecs = 5.0;
    inputsTimer = [NSTimer timerWithTimeInterval:timersecs target:self selector:@selector(updateInputs) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:inputsTimer forMode:NSRunLoopCommonModes];

    inputsEnable = true;
    
    // set up cpu use measurement tool
    cpuuse = [[CPUuse alloc] init];
    [cpuuse setup]; // FIXME: how to put stuff in init

    [self updateUI];  // FIXME: right way to do this?

/*
    // testing Task
	NSString*	result;
    result = [Task runWithToolPath:@"/usr/bin/grep" arguments:[NSArray arrayWithObject:@"france"] inputString:@"bonjour!\nvive la france!\nau revoir!" timeOut:0.0];
    NSLog(@"result: %@", result);
	
	result = [Task runWithToolPath:@"/bin/sleep" arguments:[NSArray arrayWithObject:@"2"] inputString:nil timeOut:1.0];
    NSLog(@"result: %@", result);
  */  
    //
    [self openConfig:nil];
    
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
        [respdict setObject:[blink1 serialnums] forKey:@"blink1_serialnums"];
        [respdict setObject:[blink1 blink1_id] forKey:@"blink1_id"];
        [respdict setObject:@"blink1" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
	}];
    
    [http get:@"/blink1/id" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums] forKey:@"blink1_serialnums"];
        [respdict setObject:[blink1 blink1_id] forKey:@"blink1_id"];
        [respdict setObject:@"id" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/regenerateblink1id" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* blink1_id_old = [blink1 blink1_id];
        [blink1 setHost_id:nil];
        NSString* blink1_id = [blink1 regenerateBlink1Id];
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:blink1_id_old forKey:@"blink1_id_old"];
        [respdict setObject:blink1_id     forKey:@"blink1_id"];
        [respdict setObject:@"regenerateblink1id" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/enumerate" withBlock:^(RouteRequest *request, RouteResponse *response) {
        //serialnums = [blink1 enumerate];
        [blink1 enumerate];
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums] forKey:@"blink1_serialnums"];
        [respdict setObject:@"enumerate" forKey:@"status"];
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

    [http get:@"/blink1/off" withBlock:^(RouteRequest *request, RouteResponse *response) {
        [self stopPattern:@"all"];
        [blink1 fadeToRGB:[Blink1 colorFromHexRGB: @"#000000"] atTime:0.1];

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"off" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/on" withBlock:^(RouteRequest *request, RouteResponse *response) {
        [self stopPattern:@"all"];
        [blink1 fadeToRGB:[Blink1 colorFromHexRGB: @"#FFFFFF"] atTime:0.1];

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"on" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    
    // color patterns
    
    [http get:@"/blink1/pattern" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [NSMutableDictionary dictionaryWithDictionary:patterns];
        [respdict setObject:@"pattern results" forKey:@"status"];
        //[respdict setObject:[patterns allValues] forKey:@"patterns"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/pattern/add" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname      = [request param:@"pname"];
        NSString* patternstr = [request param:@"pattern"];
        Blink1Pattern* pattern = nil;
        
        if( pname != nil && patternstr != nil ) {
            //[blink1controller addPattern:patternstr name:pname];
            //[self addPattern:patternstr name:pname];
            pattern = [[Blink1Pattern alloc] initWithPatternString:patternstr name:pname];
            [pattern setBlink1:blink1];
            [patterns setObject:pattern forKey:pname];
            //[respdict setObject:pattern forKey:@"pattern"];
        }
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:pname forKey:@"pname"];
        [respdict setObject:[pattern patternString] forKey:@"pattern"];
        [respdict setObject:@"pattern add" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
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
        [self savePrefs];
    }];

    [http get:@"/blink1/pattern/play" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        [self playPattern: pname];
        //if( pname != nil ) {
        //}
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"pattern play" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/pattern/stop" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        [self stopPattern:pname];
         /* if( pname != nil ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            if( pattern != nil ) {
                [pattern stop];
            }
        }*/
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"stop" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    
    // inputs
    
    [http get:@"/blink1/input" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* enable = [request param:@"enable"];
        inputsEnable = [enable isEqualToString:@"on"];

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
        [self savePrefs];
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
        
        NSMutableDictionary *respdict = [NSMutableDictionary dictionaryWithDictionary:input];
        [respdict setObject:@"input file" forKey:@"status"];

        if( watchFileChanged ) {
            NSString* filecontents = [NSString stringWithContentsOfFile:path
                                                               encoding:NSUTF8StringEncoding error:nil];
            [respdict setObject:filecontents forKey:@"new_event"];
            watchFileChanged = false;
        }

        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
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
        
        [input setObject:@"input url" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:input]];
        [self savePrefs];
    }];
    
    [http get:@"/blink1/input/ifttt" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* enable = [request param:@"enable"];
        if( enable != nil ) {
            //enableIftttWatch = [enable isEqualToString:@"on"];
        }
        //enable = [NSString stringWithFormat:@"%s",((enableIftttWatch)?"on":"off")];
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:enable forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    [http get:@"/blink1/input/cpuload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* level = [request param:@"level"];
        NSString* pname = [request param:@"pname"];
        
        //int cpuload = [cpuuse getCPUuse];
        //NSLog(@"cpu use:%d%%",cpuload);
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && level != nil ) {
            if( pname == nil ) pname = iname;
            [input setObject:iname      forKey:@"iname"];
            [input setObject:@"cpuload" forKey:@"type"];
            [input setObject:level      forKey:@"value"];
            [input setObject:pname      forKey:@"pname"];
            [inputs setObject:input forKey:iname];
        }

        NSMutableDictionary *respdict = [NSMutableDictionary dictionaryWithDictionary:input];
        //[respdict setObject:[NSNumber numberWithInt:cpuload] forKey:@"cpuload"];
        [respdict setObject:@"cpuload" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];

    [http get:@"/blink1/input/netload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* level = [request param:@"level"];
        NSString* pname = [request param:@"pname"];

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:iname forKey:@"iname"];
        [respdict setObject:level forKey:@"level"];
        [respdict setObject:pname forKey:@"pname"];
        [respdict setObject:@"netload not implemented yet" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
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
// Put status bar icon up on the screen
// Two icon files, one for the "normal" state and one for the "highlight" state.
// These icons should be 18x18 pixels in size, and should be done as PNGs
// so you can get the transparency you need.
// (http://www.sonsothunder.com/devres/revolution/tutorials/StatusMenu.html)
//
- (void) activateStatusMenu
{
    statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];

    //Allocates and loads the images into the application which will be used for our NSStatusItem
    NSBundle *bundle = [NSBundle mainBundle];    
    statusImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"blink1iconA1" ofType:@"png"]];
    statusHighlightImage = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"blink1iconA1i" ofType:@"png"]];
    //Sets the images in our NSStatusItem
    [statusItem setImage:statusImage];
    //[statusItem setAlternateImage:statusHighlightImage];
    
    //[statusItem setTitle: NSLocalizedString(@"blink(1)",@"")];
    [statusItem setHighlightMode:YES];
    [statusItem setMenu:statusMenu];  // instead, we'll do it by hand
    
    //[statusItem setAction:@selector(openStatusMenu:)];
    //[statusItem setTarget:self];
}

//FIXME: what's the better way of doing this?
- (void) updateUI
{
    if( [[blink1 serialnums] count] ) {
        NSString* serstr = [[blink1 serialnums] objectAtIndex:0];
        [_blink1serial setTitle: [NSString stringWithFormat:@"serial:%@",serstr]];
        [_blink1status setTitle: @"blink(1) found"];
    }
    else {
        [_blink1serial setTitle: @"serial:-none-"];
        [_blink1status setTitle: @"blink(1) not found"];
    }

}

// GUI action:
- (IBAction) openStatusMenu: (id) sender
{
    [blink1 enumerate];
    [self updateUI];
    
    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    [statusItem popUpStatusItemMenu:statusMenu];
}

// GUI action: open up main config page
- (IBAction) openConfig: (id) sender
{
    NSLog(@"Config!");
    [blink1 enumerate];
    [self updateUI];
    
    // Load the HTML content.
    //[[[_webView mainFrame] frameView] setAllowsScrolling:NO];
    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:confURL]]];
    [_window display];
    [_window setIsVisible:YES];
    [NSApp activateIgnoringOtherApps:YES];
}

// GUI action: open up 'play' page (currently used for testing alternate interface)
- (IBAction) playIt: (id) sender
{
    NSLog(@"Play!");

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:playURL]]];
    [_window display];
    [_window setIsVisible:YES];
    [NSApp activateIgnoringOtherApps:YES];
}

// GUI action: turn off blink1
- (IBAction) allOff: (id) sender
{
    NSLog(@"allOff");
    [self stopPattern:@"all"];
    [blink1 fadeToRGB:[Blink1 colorFromHexRGB: @"#000000"] atTime:0.1];
}

// GUI action: unused, rescan is done on config open now
- (IBAction) reScan: (id) sender
{
    [blink1 enumerate];
    [self updateUI];
    
    //[NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(showMenu) userInfo:nil repeats:NO];
    //[NSApp activateIgnoringOtherApps:YES];
    [statusItem popUpStatusItemMenu:statusMenu];
    
}

// GUI action: quit the app
- (IBAction) quit: (id) sender
{
    NSLog(@"Quit!");
    [self savePrefs];
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
