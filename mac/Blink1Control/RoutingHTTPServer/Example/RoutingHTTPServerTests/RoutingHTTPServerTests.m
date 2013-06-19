#import "RoutingHTTPServerTests.h"
#import "RoutingHTTPServer.h"
#import "HTTPMessage.h"
#import "HTTPDataResponse.h"

@interface RoutingHTTPServerTests ()

- (void)setupRoutes;
- (void)verifyRouteWithMethod:(NSString *)method path:(NSString *)path;
- (void)verifyRouteNotFoundWithMethod:(NSString *)method path:(NSString *)path;
- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response;
- (void)verifyMethod:(NSString *)method path:(NSString *)path contentType:(NSString *)contentType inputString:(NSString *)inputString responseString:(NSString *)expectedResponseString;

@end

@implementation RoutingHTTPServerTests

- (void)setUp {
	[super setUp];
	http = [[RoutingHTTPServer alloc] init];
	[self setupRoutes];
}

- (void)tearDown {
	[super tearDown];
}

- (void)testRoutes {
	RouteResponse *response;
	NSDictionary *params = [NSDictionary dictionary];
	HTTPMessage *request = [[HTTPMessage alloc] initEmptyRequest];

	response = [http routeMethod:@"GET" withPath:@"/null" parameters:params request:request connection:nil];
	STAssertNil(response, @"Received response for path that does not exist");

	[self verifyRouteWithMethod:@"GET" path:@"/hello"];
	[self verifyRouteWithMethod:@"GET" path:@"/hello/you"];
	[self verifyRouteWithMethod:@"GET" path:@"/page/3"];
	[self verifyRouteWithMethod:@"GET" path:@"/files/test.txt"];
	[self verifyRouteWithMethod:@"GET" path:@"/selector"];
	[self verifyRouteWithMethod:@"POST" path:@"/form"];
	[self verifyRouteWithMethod:@"POST" path:@"/users/bob"];
	[self verifyRouteWithMethod:@"POST" path:@"/users/bob/dosomething"];

	[self verifyRouteNotFoundWithMethod:@"POST" path:@"/hello"];
	[self verifyRouteNotFoundWithMethod:@"POST" path:@"/selector"];
	[self verifyRouteNotFoundWithMethod:@"GET" path:@"/page/a3"];
	[self verifyRouteNotFoundWithMethod:@"GET" path:@"/page/3a"];
	[self verifyRouteNotFoundWithMethod:@"GET" path:@"/form"];
}

- (void)testPost {
	NSError *error = nil;
	if (![http start:&error]) {
		STFail(@"HTTP server failed to start");
	}

	NSString *xmlString = @"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<greenLevel>supergreen</greenLevel>";

	[self verifyMethod:@"POST"
				  path:@"/xml"
		   contentType:@"text/xml"
		   inputString:xmlString
		responseString:@"supergreen"];
}

- (void)setupRoutes {
	[http get:@"/hello" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:@"/hello"];
	}];

	[http get:@"/hello/:name" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"/hello/%@", [request param:@"name"]]];
	}];

	[http post:@"/form" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:@"/form"];
	}];

	[http post:@"/users/:name" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"/users/%@", [request param:@"name"]]];
	}];

	[http post:@"/users/:name/:action" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"/users/%@/%@",
									 [request param:@"name"],
									 [request param:@"action"]]];
	}];

	[http get:@"{^/page/(\\d+)$}" withBlock:^(RouteRequest *request, RouteResponse *response) {
		[response respondWithString:[NSString stringWithFormat:@"/page/%@",
									 [[request param:@"captures"] objectAtIndex:0]]];
	}];

	[http get:@"/files/*.*" withBlock:^(RouteRequest *request, RouteResponse *response) {
		NSArray *wildcards = [request param:@"wildcards"];
		[response respondWithString:[NSString stringWithFormat:@"/files/%@.%@",
									 [wildcards objectAtIndex:0],
									 [wildcards objectAtIndex:1]]];
	}];

	[http handleMethod:@"GET" withPath:@"/selector" target:self selector:@selector(handleSelectorRequest:withResponse:)];

	[http post:@"/xml" withBlock:^(RouteRequest *request, RouteResponse *response) {
		NSData *bodyData = [request body];
		NSString *xml = [[NSString alloc] initWithBytes:[bodyData bytes] length:[bodyData length] encoding:NSUTF8StringEncoding];

		// Green?
		NSRange tagRange = [xml rangeOfString:@"<greenLevel>"];
		if (tagRange.location != NSNotFound) {
			NSUInteger start = tagRange.location + tagRange.length;
			NSUInteger end = [xml rangeOfString:@"<" options:0 range:NSMakeRange(start, [xml length] - start)].location;
			if (end != NSNotFound) {
				NSString *greenLevel = [xml substringWithRange:NSMakeRange(start, end - start)];
				[response respondWithString:greenLevel];
			}
		}
	}];
}

- (void)handleSelectorRequest:(RouteRequest *)request withResponse:(RouteResponse *)response {
	[response respondWithString:@"/selector"];
}

- (void)verifyRouteWithMethod:(NSString *)method path:(NSString *)path {
	RouteResponse *response;
	NSDictionary *params = [NSDictionary dictionary];
	HTTPMessage *request = [[HTTPMessage alloc] initEmptyRequest];

	response = [http routeMethod:method withPath:path parameters:params request:request connection:nil];
	STAssertNotNil(response.proxiedResponse, @"Proxied response is nil for %@ %@", method, path);

	NSUInteger length = [response.proxiedResponse contentLength];
	NSData *data = [response.proxiedResponse readDataOfLength:length];
	NSString *responseString = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
	STAssertEqualObjects(responseString, path, @"Unexpected response for %@ %@", method, path);
}

- (void)verifyRouteNotFoundWithMethod:(NSString *)method path:(NSString *)path {
	RouteResponse *response;
	NSDictionary *params = [NSDictionary dictionary];
	HTTPMessage *request = [[HTTPMessage alloc] initEmptyRequest];

	response = [http routeMethod:method withPath:path parameters:params request:request connection:nil];
	STAssertNil(response, @"Response should have been nil for %@ %@", method, path);
}

- (void)verifyMethod:(NSString *)method path:(NSString *)path contentType:(NSString *)contentType inputString:(NSString *)inputString responseString:(NSString *)expectedResponseString {
	NSError *error = nil;
	NSData *data = [inputString dataUsingEncoding:NSUTF8StringEncoding];

	NSString *baseURLString = [NSString stringWithFormat:@"http://127.0.0.1:%d", [http listeningPort]];

	NSString *urlString = [baseURLString stringByAppendingString:path];
	NSURL *url = [NSURL URLWithString:urlString];

	NSMutableURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url];
	[request setHTTPMethod:method];
	[request addValue:contentType forHTTPHeaderField:@"Content-Type"];
	[request addValue:[NSString stringWithFormat:@"%ld", [data length]] forHTTPHeaderField:@"Content-Length"];
	[request setHTTPBody:data];

	NSURLResponse *response;
	NSHTTPURLResponse *httpResponse;
	NSData *responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&response error:&error];
	STAssertNotNil(response, @"No response received for %@ %@", method, path);
	STAssertNotNil(responseData, @"No response data received for %@ %@", method, path);
	STAssertTrue([response isKindOfClass:[NSHTTPURLResponse class]], @"Response is not an NSHTTPURLResponse");

	httpResponse = (NSHTTPURLResponse *)response;
	STAssertEquals([httpResponse statusCode], 200L, @"Unexpected status code for %@ %@", method, path);

	NSString *responseString = [[NSString alloc] initWithBytes:[responseData bytes] length:[responseData length] encoding:NSUTF8StringEncoding];
	STAssertEqualObjects(responseString, expectedResponseString, @"Unexpected response for %@ %@", method, path);
}

@end
