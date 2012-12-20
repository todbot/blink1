#import "AppDelegate.h"
#import "RoutingHTTPServer.h"

@interface AppDelegate ()

- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response;

@end

@implementation AppDelegate

@synthesize window = _window;
@synthesize http;

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
}

- (void)setupRoutes {
	[http get:@"/hello" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:@"Hello!"];
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

- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response {
	[response respondWithString:@"Handled through selector"];
}

@end
