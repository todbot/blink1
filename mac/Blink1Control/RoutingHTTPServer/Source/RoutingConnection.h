#import <Foundation/Foundation.h>
#import "HTTPConnection.h"
@class RoutingHTTPServer;

@interface RoutingConnection : HTTPConnection {
	RoutingHTTPServer *http;
	NSDictionary *headers;
}

@end
