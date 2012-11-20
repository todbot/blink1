//
//  B1SAppDelegate.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 8/31/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//
//
// ToDo:
// - network real-time load input
//
// inputs have attributes
// - enabled  -- boolean,
// - iname    -- string, name of input
// - type     -- string, type of input ("url","file","ifttt")
// - arg1     -- string, argument for type of input (url, filepath)
// - arg2     -- string, argument for type of input (url, filepath)
// - arg3     -- string, argument for type of input (url, filepath)
// - pname    -- string, name of pattern to play (if none specified in content of input, like file)
// - status   -- string, last status of input
// - lastVal  -- string, last value when input was parsed
// - lastTime -- date?, last time when input was parsed
//
//
// NOTE: CocoaHTTPServer HTTPConnection was modified to replace "+" with "%20"
//

#import "B1SAppDelegate.h"
#import "RoutingHTTPServer.h"

#import <QuartzCore/QuartzCore.h>  // for CoreImage-based colored statusbar 


@interface B1SAppDelegate ()

@end


@implementation B1SAppDelegate

@synthesize window = _window;
@synthesize webView = _webView;
@synthesize blink1status = _blink1status;
@synthesize blink1serial = _blink1serial;

@synthesize statusItem;
@synthesize statusMenu;
@synthesize statusImage;
//@synthesize statusHighlightImage;

@synthesize http;
@synthesize blink1;


const NSInteger http_port_default = 8934;

//FIXME: what to do with these URLs?
// solution: put them in the prefs, duh
NSString* confURLbase =  @"http://localhost:%ld/blink_1/";
NSString* playURLbase =  @"http://localhost:%ld/bootstrap/blink1.html";

NSString* iftttEventUrl = @"http://api.thingm.com/blink1/events";
//NSString* iftttEventUrl = @"http://localhost/~tod/blink1/events";

NSString* scriptsPath = @"~/Documents/blink1-scripts";

NSTimeInterval inputInterval       = 5.0f;  // in seconds

NSTimeInterval iftttUpdateInterval = 15.0f;
NSTimeInterval urlUpdateInterval   = 30.0f;


// play pattern with restart
// pname might also be just a hex color, e.g. "#FF0033"
- (Boolean) playPattern: (NSString*)pname
{
    return [self playPattern:pname restart:true];
}

// play a pattern
// can restart if already playing, or just leave be an already playing pattern
// returns true if pattern was played, false if not
- (Boolean) playPattern: (NSString*)pname restart:(Boolean)restart
{
    DLog(@"play %@",pname);
    if( pname == nil ) return false;
    
    // a hex color, not a proper pattern, send hex color immediately
    if( [pname hasPrefix:@"#"] ) {
        [blink1 fadeToRGB:[Blink1 colorFromHexRGB:pname] atTime:0.1];
        return true;
    }
    // otherwise, treat it as a pattern name
    Blink1Pattern* pattern = [patterns objectForKey:pname];
    if( pattern != nil ) {  // FIXME: what if we have no matching pattern?
        [pattern setBlink1:blink1];  // FIXME: just in case
        if( ![pattern playing] || ([pattern playing] && restart) ) {
            [pattern play];
        }
        return true;
    }
    return false;
}

// stop a currently playing pattern
// return true if pattern existed to stop, else false
- (Boolean) stopPattern: (NSString*)pname
{
    if( pname == nil ) return false;
    Blink1Pattern* pattern = [patterns objectForKey:pname];
    [pattern stop];
    return (pattern!=nil);
}

//
- (void) stopAllPatterns
{
    for( Blink1Pattern* pattern in [patterns allValues] ) {
        [pattern stop];
    }
}

//
// Given a string (contents of file or url),
// analyze it for a pattern name or rgb hex string
// returns pattern to play, or nil if nothing to play
//
- (NSString*) parsePatternOrColorInString: (NSString*) str
{
    //DLog(@"parsePatternOrColorInString: %@",str);
    NSString* patternstr = [self readColorPattern:str];
    NSString* patt = nil;

    if( patternstr ) {  // pattern detected
        DLog(@"found color pattern: %@",patternstr);
        patt = patternstr;
    }
    else  {
        NSColor* colr = [Blink1 colorFromHexRGB:str];
        if( colr ) {
            // TODO: create and play pattern from hex color, like "1,#FF33CC,0.1"
            // BUT: maybe make a 'temp' pattern? or a special class of pattern? or a parameterized meta-pattern?
            // let's try: FIXME: hack using "-1" to mean "temporary pattern"
            //patt = [NSString stringWithFormat:@"-1,%@,0.1",[Blink1 hexStringFromColor:colr]];
            patt = [Blink1 hexStringFromColor:colr];
            DLog(@"hex color patt: %@",patt);
        }
        else {
            DLog(@"no color found");
        }
    }
    return patt;
}

//
// Search for 'pattern: "pattern name"' in contentStr
// contentStr can also be JSON
// returns pattern name if successful, or 'nil' if no pattern found
//
- (NSString*) readColorPattern: (NSString*)contentStr
{
    NSString* str = nil;
    if( contentStr == nil ) return nil;
    NSScanner *scanner = [NSScanner scannerWithString:contentStr];
    BOOL isPattern = [scanner scanUpToString:@"pattern" intoString:NULL];
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

// For a given URL in a string, return the page contents of that URL
// (note: should only be used on small documents)
// returns nil on bad request
- (NSString*) getContentsOfUrl: (NSString*) urlstr
{
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:urlstr]];
    NSData *response = [NSURLConnection sendSynchronousRequest:request returningResponse:nil error:nil];
    if( response == nil ) return nil;
    return [[NSString alloc] initWithData:response encoding:NSUTF8StringEncoding];
}


//
- (Boolean) deleteInput: (NSString*)iname
{
    NSDictionary *input = [inputs objectForKey:iname];
    if( input == nil ) return false;
    
    NSString* type = [input objectForKey:@"type"];
    NSString* arg = [input objectForKey:@"arg"];
    if( [type isEqualToString:@"file"] ) {
        DLog(@"remove file %@",arg);
        //[myVDKQ removePath:arg];
    }
    else if( [type isEqualToString:@"url"] ) {
        DLog(@"remove url %@",arg);
    }
    [inputs removeObjectForKey:iname];
    return true;
}

//
- (void) updateFileInput: (NSMutableDictionary*)input
{
    NSString* path = [input objectForKey:@"arg1"];
    NSString* filepath = [path stringByStandardizingPath];
    //NSString* fpath = [path stringByExpandingTildeInPath];

    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];
    if( fileExists ) {
        NSString* contentstr = [NSString stringWithContentsOfFile:filepath encoding:NSUTF8StringEncoding error:NULL];
        DLog(@"file '%@' contents='%@'",filepath,contentstr);
        NSString* patternstr  = [self parsePatternOrColorInString: contentstr];
        DLog(@"patternstr='%@'",patternstr);
        if( patternstr != nil ) {                                        // FIXME: !!!! what about test mode
            [input setObject:patternstr forKey:@"lastVal"];

            [self playPattern: patternstr]; // FIXME: need to check for no pattern?
            
        }
    }
    else {
        NSString* errstr = [NSString stringWithFormat:@"no such file '%@'",filepath];
        [input setObject:errstr forKey:@"lastVal"];
    }
}

//
- (void) updateUrlInput: (NSMutableDictionary*) input
{
    NSString* urlstr         = [input valueForKey:@"arg1"];
    NSString* lastVal        = [input valueForKey:@"lastVal"];
    NSTimeInterval lastTime  = [[input valueForKey:@"lastTime"] doubleValue];
    NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    if( (now - lastTime) < urlUpdateInterval ) {   // only update URLs every 30 secs
        return;
    }
    [input setObject:[NSNumber numberWithInt:now] forKey:@"lastTime"];
    
    NSString* responsestr = [self getContentsOfUrl: urlstr];
    if( !responsestr ) {
        [input setObject:@"bad url" forKey:@"lastVal"];
        return;
    }
    NSString* patternstr  = [self parsePatternOrColorInString: responsestr];
    
    if( patternstr!=nil && ![patternstr isEqualToString:lastVal] ){ // different!
        DLog(@"url: playing pattern %@",patternstr);
        [input setObject:patternstr forKey:@"lastVal"]; // save last val
        [self playPattern: patternstr]; // FIXME: need to check for no pattern? or already running pattern?
    } else {
        DLog(@"url: no change");
    }
}

// fetch and parse ifttt response, only once per interval
- (void) getIftttResponse: (Boolean) normalmode
{
    NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    if( normalmode && ((now - iftttLastTime) < iftttUpdateInterval) ) {   // only update URLs every 30 secs
        return;
    }
    iftttLastTime = now;

    NSString* eventUrlStr = [NSString stringWithFormat:@"%@/%@", iftttEventUrl, [blink1 blink1_id]];
    NSString* jsonStr = [self getContentsOfUrl: eventUrlStr];
    iftttResponse = [NSMutableDictionary dictionaryWithDictionary:[_jsonparser objectWithString:jsonStr]];
    DLog(@"ifttt time:%ld url:%@ json: '%@'", (long)iftttLastTime, eventUrlStr,jsonStr);
}

// depends on getIftttResponse being called before
- (void) updateIftttInput: (NSMutableDictionary*) input
{
    if( !iftttResponse ) return;

    NSTimeInterval lastTime  = [[input valueForKey:@"lastTime"] doubleValue];
    NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    if( (now - lastTime) < urlUpdateInterval ) {   // only update URLs every 30 secs
        return;
    }
    //[input setObject:[NSNumber numberWithInt:now] forKey:@"lastTimeEval"];

    
    NSString* pname          = [input valueForKey:@"pname"];
    NSString* rulename       = [input valueForKey:@"arg1"];

    NSDictionary* list = [iftttResponse objectForKey:@"events"];
    if( !list ) {
        [input setObject:@"no events" forKey:@"lastVal"];
    }
    
    NSMutableArray* possible_vals = [[NSMutableArray alloc] init];
    
    for (NSDictionary *event in list) {
        NSString * ev_id      = [event objectForKey:@"blink1_id"];
        NSString * ev_name    = [event objectForKey:@"name"];
        NSString * ev_source  = [event objectForKey:@"source"];
        NSString * ev_datestr = [event objectForKey:@"date"];
        NSTimeInterval ev_date = [ev_datestr integerValue];

        //[possible_vals setObject:ev_source forKey:ev_name];
        [possible_vals addObject:ev_name];
        
        //DLog(@"ev_id:%@, name:%@, source:%@ date: %@ lastTime:%f", ev_id, ev_name, ev_source, ev_datestr, iftttLastTime);
        
        if( [ev_name isEqualToString:rulename] ) {  // match
            DLog(@"ifttt match!");
            if( ev_date > lastTime ) {
                DLog(@"ifttt new event!");
                [input setObject:ev_source forKey:@"lastVal"];
                [self playPattern: pname]; // trigger the pattern
            }
        
            [input setObject:[NSNumber numberWithInt:ev_date] forKey:@"lastTime"];  // FIXME:
        }
    }
    [input setObject:possible_vals forKey:@"possibleVals"];  // FIXME: document this
}

//
- (void) updateScriptInput: (NSMutableDictionary*)input
{
    DLog(@"updateScriptInput");
    //NSTimeInterval lastTime  = [[input valueForKey:@"lastTime"] doubleValue];
    //NSTimeInterval now = [[NSDate date] timeIntervalSince1970];
    //if( (now - lastTime) < 30 ) {     // only update URLs every 30 secs
    //    return;
    //}
    //[input setObject:[NSNumber numberWithInt:now] forKey:@"lastTime"];

    
    NSString* path = [input objectForKey:@"arg1"];
    NSString* fpath = [NSString stringWithFormat:@"%@/%@",scriptsPath,path];
    NSString* filepath = [fpath stringByStandardizingPath];

    BOOL fileExists = [[NSFileManager defaultManager] fileExistsAtPath:filepath];
    if( fileExists ) {
        NSString* contentstr = @"";
        contentstr = [Task runWithToolPath:filepath
                                 arguments:NULL
                               inputString:NULL
                                   timeOut:0.0];
        DLog(@"script results='%@'",contentstr);
        NSString* patternstr  = [self parsePatternOrColorInString: contentstr];
        DLog(@"patternstr='%@'",patternstr);
        if( patternstr ) {
            [input setObject:patternstr forKey:@"lastVal"];
            [self playPattern: patternstr]; // FIXME: need to check for no pattern?
        }
    }
    else {
        NSString* errstr = [NSString stringWithFormat:@"no such file '%@'",filepath];
        [input setObject:errstr forKey:@"lastVal"];
    }
}

//
- (void) updateCpuloadInput: (NSMutableDictionary*) input
{
    int min     = [[input valueForKey:@"arg1"] intValue];
    int max     = [[input valueForKey:@"arg2"] intValue];
    if( max==0 ) max=INT16_MAX;
    DLog(@"cpuload:%d%% - min/max:%d/%d",cpuload,min,max);
    if( cpuload >= min && cpuload < max ) {
        [self playPattern: [input valueForKey:@"pname"] restart:NO];
    }
    [input setObject:[NSNumber numberWithInt:cpuload] forKey:@"lastVal"]; // save last val
}

//
- (void) updateNetloadInput: (NSMutableDictionary*) input
{
    int min     = [[input valueForKey:@"arg1"] intValue];
    int max     = [[input valueForKey:@"arg2"] intValue];
    if( max==0 ) max=INT16_MAX;
    DLog(@"netload:%d%% - min/max:%d/%d",netload,min,max);

    if( netload >= min && netload < max ) {
        [self playPattern: [input valueForKey:@"pname"] restart:NO];
    }
    [input setObject:[NSNumber numberWithInt:netload] forKey:@"lastVal"]; // save last val
}


// ----------------------------------------------------------------------------
// the main deal for triggering color patterns
// ----------------------------------------------------------------------------
// this is called every 10 secs via an NSTimer
- (void) updateInputs
{
    //DLog(@"updateInputs");
    if( !inputsEnable ) return;

    // these must be updated periodically for statistics on them to work
    cpuload = [cpuuse getCPUuse];
    netload = [netuse getNetuse];
    [self getIftttResponse:true];

    NSString* key;
    for( key in inputs) {
        NSMutableDictionary* input = [inputs objectForKey:key];
        NSString* type    = [input valueForKey:@"type"];
        
        if( [type isEqualToString:@"url"])
        {
            [self updateUrlInput: input];
        }
        else if( [type isEqualToString:@"ifttt"] )
        {
            [self updateIftttInput: input];
        }
        else if( [type isEqualToString:@"file"] )
        {
            [self updateFileInput:input];
        }
        else if( [type isEqualToString:@"script"] )
        {
            [self updateScriptInput:input];
        }
        else if( [type isEqualToString:@"cpuload"] )
        {
            [self updateCpuloadInput:input];
        }
        else if( [type isEqualToString:@"netload"] )
        {
            [self updateNetloadInput:input];
        }
    } //for(key)
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
    http_port                 = [prefs integerForKey:@"http_port"];
    //BOOL first_run            = [prefs boolForKey:@"first_run"];

    if( http_port == 0 ) {
        http_port = http_port_default;
    }
    
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
    DLog(@"blink1_id:%@",[blink1 blink1_id]);
    
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
    [prefs setInteger:http_port         forKey:@"http_port"];
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
    blink1.updateHandler = ^(NSColor *lastColor, float lastTime)
    {
        NSString* lastcolorstr = [Blink1 hexStringFromColor:lastColor];
        [[weakSelf window] setTitle:[NSString stringWithFormat:@"blink(1) control - %@",lastcolorstr]];
        [weakSelf updateStatusImageHue:lastColor];
    };
     
    // set up json parser
    _jsonparser = [[SBJsonParser alloc] init];
    _jsonwriter = [[SBJsonWriter alloc] init];
    _jsonwriter.humanReadable = YES;
    _jsonwriter.sortKeys = YES;

    [self loadPrefs];
    
    [self setupHttpServer];
    
    // set up input watcher
    inputsTimer = [NSTimer timerWithTimeInterval:inputInterval target:self selector:@selector(updateInputs) userInfo:nil repeats:YES];
    [[NSRunLoop currentRunLoop] addTimer:inputsTimer forMode:NSRunLoopCommonModes];
    inputsEnable = true;
    
    // set up cpu use measurement tool
    netuse = [[Netuse alloc] init];  // FIXME: what about updateInterval?
    cpuuse = [[CPUuse alloc] init];
    [cpuuse setup]; // FIXME: how to put stuff in init

    [self updateUI];  // FIXME: right way to do this?
    [self openConfig:nil]; //
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
    
	// Server on localhost:http_port, serving files from our embedded Web folder
	[http setPort:http_port];
    
    [http setInterface:@"localhost"]; // restrict to local computer, maybe make this a pref?
    
	NSString *htmlPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"html"];
	[http setDocumentRoot:htmlPath];
    DLog(@"port: %ld\nhtmlPath: %@",http_port,htmlPath);
    
	NSError *error;
	if (![http start:&error]) {
		DLog(@"Error starting HTTP server: %@", error);
	}
}

//-----------------------------------------------------------------------------
// Local HTTP server routes
// ----------------------------------------------------------------------------
- (void)setupHttpRoutes
{
	[http get:@"/blink1" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums] forKey:@"blink1_serialnums"];
        [respdict setObject:[blink1 blink1_id]  forKey:@"blink1_id"];
        [respdict setObject:@"blink1" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
	}];
    
    [http get:@"/blink1/id" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums] forKey:@"blink1_serialnums"];
        [respdict setObject:[blink1 blink1_id]  forKey:@"blink1_id"];
        [respdict setObject:@"blink1 id" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/regenerateblink1id" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* blink1_id_old = [blink1 blink1_id];

        [blink1 setHost_id:nil];
        [blink1 enumerate];
        NSString* blink1_id = [blink1 regenerateBlink1Id];

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums]  forKey:@"blink1_serialnums"];
        [respdict setObject:blink1_id_old  forKey:@"blink1_id_old"];
        [respdict setObject:blink1_id      forKey:@"blink1_id"];
        [respdict setObject:@"regenerateblink1id" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    [http get:@"/blink1/enumerate" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* blink1_id_old = [blink1 blink1_id];
        
        [blink1 enumerate];
        [blink1 regenerateBlink1Id];
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 serialnums]  forKey:@"blink1_serialnums"];
        [respdict setObject:blink1_id_old        forKey:@"blink1_id_old"];
        [respdict setObject:[blink1 blink1_id]   forKey:@"blink1_id"];
        [respdict setObject:@"enumerate"         forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/fadeToRGB" withBlock:^(RouteRequest *request, RouteResponse *response) {
        [self stopPattern:@"all"];
        NSString* rgbstr = [request param:@"rgb"];
        NSString* timestr = [request param:@"time"];
        if( rgbstr==nil ) rgbstr = @"";
        if( timestr==nil ) timestr = @"";
        NSColor * colr = [Blink1 colorFromHexRGB: rgbstr];
        float secs = 0.1;
        [[NSScanner scannerWithString:timestr] scanFloat:&secs];

        [blink1 fadeToRGB:colr atTime:secs];

        NSString* statusstr = [NSString stringWithFormat:@"fadeToRGB: %@ t:%2.3f",[Blink1 hexStringFromColor:colr],secs];
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

    [http get:@"/blink1/lastColor" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[blink1 lastColorHexString] forKey:@"lastColor"];
        [respdict setObject:@"lastColor" forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    
    // color patterns
    
    // list patterns
    [http get:@"/blink1/pattern" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];//[NSMutableDictionary dictionaryWithDictionary:patterns];
        [respdict setObject:@"pattern results" forKey:@"status"];
        [respdict setObject:[patterns allValues] forKey:@"patterns"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    // add a pattern
    [http get:@"/blink1/pattern/add" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname      = [request param:@"pname"];
        NSString* patternstr = [request param:@"pattern"];
        DLog(@"pattern add: %@", pname);
        Blink1Pattern* pattern = nil;
        NSString* statusstr = @"pattern add";
        
        if( pname != nil && patternstr != nil ) {
            pattern = [[Blink1Pattern alloc] initWithPatternString:patternstr name:pname];
            [patterns setObject:pattern forKey:pname];
        }
        else {
            statusstr = @"error: need 'pname' and 'pattern' arguments to make pattern";
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        if( pattern!=nil)
            [respdict setObject:pattern forKey:@"pattern"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // delete a pattern
    [http get:@"/blink1/pattern/del" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        NSString* statusstr = @"no pattern by that pname";
        if( pname != nil ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            if( pattern != nil ) {
                [pattern stop];
                [patterns removeObjectForKey:pname];
                statusstr = [NSString stringWithFormat:@"pattern '%@' removed", pname];
            }
        }

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // delete all patterns
    [http get:@"/blink1/pattern/delall" withBlock:^(RouteRequest *request, RouteResponse *response) {
        for( NSString* pname in [patterns allKeys] ) {
            Blink1Pattern* pattern = [patterns objectForKey:pname];
            [pattern stop];
            [patterns removeObjectForKey:pname];
        }
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"all patterns removed" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // play a pattern
    [http get:@"/blink1/pattern/play" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        NSString* statusstr = @"no pattern by that pname";
        if( [self playPattern: pname] ) {
            statusstr = [NSString stringWithFormat:@"pattern '%@' playing",pname];
        }

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    // stop a pattern
    [http get:@"/blink1/pattern/stop" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* pname   = [request param:@"pname"];
        NSString* statusstr = @"no pattern by that pname";
        if( [self stopPattern:pname] ) {
            statusstr = [NSString stringWithFormat:@"pattern '%@' stopped",pname];
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];

    [http get:@"/blink1/pattern/stopall" withBlock:^(RouteRequest *request, RouteResponse *response) {
        [self stopAllPatterns];
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"all patterns stopped" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    //
    // inputs
    //
    
    // list all inputs
    [http get:@"/blink1/input" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* enable = [request param:@"enabled"];
        if( enable != nil ) {   // i.e. param was specified
            inputsEnable = ([enable isEqualToString:@"on"] || [enable isEqualToString:@"true"] );
        }

        NSString* statusstr = @"input results";
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:[inputs allValues] forKey:@"inputs"];
        [respdict setObject:statusstr forKey:@"status"];
        [respdict setObject:[NSNumber numberWithBool:inputsEnable] forKey:@"enabled"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
    }];
    
    // delete an input
    [http get:@"/blink1/input/del" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        
        NSString* statusstr = @"no such input";
        if( iname != nil ) {
            if( [self deleteInput:iname] ) {
                statusstr = [NSString stringWithFormat:@"input '%@' removed",iname];
            }
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // delete all inputs
    [http get:@"/blink1/input/delall" withBlock:^(RouteRequest *request, RouteResponse *response) {
        for( NSString* iname in [inputs allKeys]){
            [self deleteInput:iname];
        }
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:@"delall" forKey:@"status"];
		[response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
        
    // add a file watching input
    [http get:@"/blink1/input/file" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* path  = [request param:@"arg1"];
        NSString* test  = [request param:@"test"];

        NSString* statusstr = @"must specifiy 'iname' and 'path'";

        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && path != nil ) {
            if( !pname )  pname = [iname copy];
            [input setObject:pname   forKey:@"pname"];
            [input setObject:iname   forKey:@"iname"];
            [input setObject:@"file" forKey:@"type"];
            [input setObject:path    forKey:@"arg1"];
            
            [self updateFileInput:input];
                        
            if( !([test isEqualToString:@"on"] || [test isEqualToString:@"true"]) ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"input file";
        }

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init]; //[NSMutableDictionary dictionaryWithDictionary:input];
        [respdict setObject:input     forKey:@"input"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // add a URL watching input
    [http get:@"/blink1/input/url" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* url   = [request param:@"arg1"];
        NSString* test  = [request param:@"test"];

        NSString* statusstr = @"must specifiy 'iname' and 'arg1' (url)";
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && url != nil ) { // the minimum requirements for this input type
            if( !pname )  pname = [iname copy];
            [input setObject:pname   forKey:@"pname"];
            [input setObject:iname   forKey:@"iname"];
            [input setObject:@"url"  forKey:@"type"];
            [input setObject:url     forKey:@"arg1"];
        
            [self updateUrlInput: input];

            if( !([test isEqualToString:@"on"] || [test isEqualToString:@"true"]) ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"input url";
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init]; //[NSMutableDictionary dictionaryWithDictionary:input];
        [respdict setObject:input forKey:@"input"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // add the ifttt watching input
    [http get:@"/blink1/input/ifttt" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* chan  = [request param:@"arg1"];
        NSString* test  = [request param:@"test"];
        
        Boolean testmode = ([test isEqualToString:@"on"] || [test isEqualToString:@"true"]);
        
        NSString* statusstr = @"must specifiy 'iname' and 'arg1' (channel)";
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && chan != nil ) { // the minimum requirements for this input type
            if( !pname )  pname = [iname copy];
            [input setObject:pname     forKey:@"pname"];
            [input setObject:iname     forKey:@"iname"];
            [input setObject:@"ifttt"  forKey:@"type"];
            [input setObject:chan      forKey:@"arg1"];

            if( testmode )
                [self getIftttResponse:false];

            [self updateIftttInput: input];
        
            if( !testmode ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"input ifttt";
        }

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
        [respdict setObject:input     forKey:@"input"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
    
    // add a script execing input
    [http get:@"/blink1/input/script" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* path  = [request param:@"arg1"];
        NSString* test  = [request param:@"test"];
        
        NSString* statusstr = @"must specifiy 'iname' and 'arg1' for scriptname";
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && path != nil ) {
            if( !pname ) pname = [iname copy];
            [input setObject:pname     forKey:@"pname"];
            [input setObject:iname     forKey:@"iname"];
            [input setObject:@"script" forKey:@"type"];
            [input setObject:path      forKey:@"arg1"];
            
            [self updateScriptInput:input];
            
            if( !([test isEqualToString:@"on"] || [test isEqualToString:@"true"]) ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"input script";
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:input     forKey:@"input"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];

    // add a script execing input
    [http get:@"/blink1/input/scriptlist" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* statusstr = @"scriptlist not implemented yet";
        //NSMutableDictionary* scripts = [[NSMutableDictionary alloc] init];
        
        NSArray* dirlist = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:[scriptsPath stringByStandardizingPath] error:nil];
        if( ! dirlist )
            dirlist = [[NSArray alloc] init];
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:dirlist   forKey:@"scripts"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];

    // add a cpu load watching input
    [http get:@"/blink1/input/cpuload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* min   = [request param:@"arg1"];
        NSString* max   = [request param:@"arg2"];
        NSString* test  = [request param:@"test"];
        
        NSString* statusstr = @"must specifiy 'iname' and 'arg1' for min";
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && min != nil ) {
            if( !pname ) pname = [iname copy];
            [input setObject:pname      forKey:@"pname"];
            [input setObject:iname      forKey:@"iname"];
            [input setObject:@"cpuload" forKey:@"type"];
            [input setObject:min        forKey:@"arg1"];
            if( max )
                [input setObject:max    forKey:@"arg2"];
            
            [self updateCpuloadInput:input];

            if( !([test isEqualToString:@"on"] || [test isEqualToString:@"true"]) ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"cpuload input";
        }

        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:statusstr forKey:@"status"];
        [respdict setObject:input     forKey:@"input"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];

    // add a network load watching input
    [http get:@"/blink1/input/netload" withBlock:^(RouteRequest *request, RouteResponse *response) {
        NSString* iname = [request param:@"iname"];
        NSString* pname = [request param:@"pname"];
        NSString* min   = [request param:@"arg1"];
        NSString* max   = [request param:@"arg2"];
        NSString* test  = [request param:@"test"];

        NSString* statusstr = @"must specifiy 'iname' and 'arg1' for min";
        
        NSMutableDictionary* input = [[NSMutableDictionary alloc] init];
        if( iname != nil && min != nil ) {
            if( pname == nil ) pname = [iname copy];
            [input setObject:pname      forKey:@"pname"];
            [input setObject:iname      forKey:@"iname"];
            [input setObject:@"netload" forKey:@"type"];
            [input setObject:min        forKey:@"arg1"];
            if( max )
                [input setObject:max    forKey:@"arg2"];
            
            [self updateNetloadInput:input];
            
            if( !([test isEqualToString:@"on"] || [test isEqualToString:@"true"]) ) {
                [inputs setObject:input forKey:iname];  // not a test, add new input to inputs list
            }
            statusstr = @"netload input";
        }
        
        NSMutableDictionary *respdict = [[NSMutableDictionary alloc] init];
        [respdict setObject:input     forKey:@"input"];
        [respdict setObject:statusstr forKey:@"status"];
        [response respondWithString: [_jsonwriter stringWithObject:respdict]];
        [self savePrefs];
    }];
}


// ---------------------------------------------------------------------------
// GUI stuff
// ---------------------------------------------------------------------------

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
    statusImageBase = [[NSImage alloc] initWithContentsOfFile:[bundle pathForResource:@"blink1iconA1" ofType:@"png"]];
    
    [statusItem setImage:statusImageBase];
        
    [statusItem setHighlightMode:YES];
    [statusItem setMenu:statusMenu];
}

//
- (void) updateStatusImageHue:(NSColor*)tint
{
    statusImage = [self tintImage:statusImageBase withColor:tint];
    [statusItem setImage:statusImage];
}

// from: http://stackoverflow.com/questions/1413135/tinting-a-grayscale-nsimage-or-ciimage
- (NSImage *)tintImage:(NSImage*)srcImage withColor:(NSColor *)tint
{
    if (tint != nil) {
        NSSize size = [srcImage size];
        NSRect bounds = { NSZeroPoint, size };
        NSImage *tintedImage = [[NSImage alloc] initWithSize:size];
        
        [tintedImage lockFocus];
        
        CIFilter *colorGenerator = [CIFilter filterWithName:@"CIConstantColorGenerator"];
        CIColor *color = [[CIColor alloc] initWithColor:tint];

        [colorGenerator setValue:color forKey:@"inputColor"];
        
        CIFilter *monochromeFilter = [CIFilter filterWithName:@"CIColorMonochrome"];
        CIImage *baseImage = [CIImage imageWithData:[srcImage TIFFRepresentation]];
        
        [monochromeFilter setValue:baseImage forKey:@"inputImage"];
        [monochromeFilter setValue:[CIColor colorWithRed:0.75 green:0.75 blue:0.75] forKey:@"inputColor"];
        [monochromeFilter setValue:[NSNumber numberWithFloat:1.0] forKey:@"inputIntensity"];
        
        CIFilter *compositingFilter = [CIFilter filterWithName:@"CISourceAtopCompositing"];//CIMultiplyCompositing"];
        
        [compositingFilter setValue:[colorGenerator valueForKey:@"outputImage"] forKey:@"inputImage"];
        [compositingFilter setValue:[monochromeFilter valueForKey:@"outputImage"] forKey:@"inputBackgroundImage"];
        
        CIImage *outputImage = [compositingFilter valueForKey:@"outputImage"];
        
        [outputImage drawAtPoint:NSZeroPoint
                        fromRect:bounds
                       operation:NSCompositeCopy
                        fraction:1.0];
        
        [tintedImage unlockFocus];
        
        return tintedImage;
    }
    else {
        DLog(@"tint is nil");
        return [srcImage copy];
    }
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
    [blink1 enumerate];
    [self updateUI];

    if( ![_window isVisible] ) {
        NSString* confURL = [NSString stringWithFormat: confURLbase, http_port];
    
        // Load the HTML content.
        //[[[_webView mainFrame] frameView] setAllowsScrolling:NO];
        [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:confURL]]];
        [_window display];
        [_window setIsVisible:YES];
    }
    [NSApp activateIgnoringOtherApps:YES];
}

// GUI action: open up 'play' page (currently used for testing alternate interface)
- (IBAction) playIt: (id) sender
{
    DLog(@"Play!");

    NSString* playURL = [NSString stringWithFormat: playURLbase, http_port];

    [[_webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:playURL]]];
    [_window display];
    [_window setIsVisible:YES];
    [NSApp activateIgnoringOtherApps:YES];
}

// GUI action: turn off blink1
- (IBAction) allOff: (id) sender
{
    DLog(@"allOff");
    [self stopAllPatterns];
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
    DLog(@"Quit!");
    [self stopAllPatterns];
    [self savePrefs];
    [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
}



@end

/*

 //[self performSelectorOnMainThread:@selector(updateWatchFile:)
 //                       withObject:fpath
 //                    waitUntilDone:NO];
 //DLog(@"watching file %@",fpath);
 //}
 //else {
 //path = watchPath;
 //}
 
 *
 NSString* filecontents = @"";
 if( watchFileChanged ) {
 filecontents = [NSString stringWithContentsOfFile:path
 encoding:NSUTF8StringEncoding error:nil];
 watchFileChanged = false;
 }
 
 // set up file watcher
 myVDKQ = [[VDKQueue alloc] init];
 [myVDKQ setDelegate:self];
 [self updateWatchFile:@"/Users/tod/tmp/blink1-colors.txt"];  //FIXME: test
 

 // for "watchfile" functionality, should be put in its own class
 -(void) VDKQueue:(VDKQueue *)queue receivedNotification:(NSString*)noteName forPath:(NSString*)fpath;
 {
 DLog(@"watch file: %@ %@", noteName, fpath);
 if( [noteName isEqualToString:@"VDKQueueFileWrittenToNotification"] ) {
 DLog(@"watcher: file written %@ %@", noteName, fpath);
 watchFileChanged = true;
 }
 // FIXME: this doesn't work
 if( [noteName isEqualToString:@"VDKQueueLinkCountChangedNotification"]) {
 DLog(@"re-adding deleted file");
 [self updateWatchFile:fpath];
 }
 }
 
 // for "watchfile" functionality, should be put in its own class
 - (void)updateWatchFile:(NSString*)wPath
 {
 DLog(@"updateWatchFile %@",wPath);
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
 
*/


