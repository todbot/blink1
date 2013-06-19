#import <Foundation/Foundation.h>
#import "RoutingHTTPServer.h"


@interface Route : NSObject {
	NSString *path;
	RequestHandler handler;
	id target;
	SEL selector;
	NSArray *keys;
}

@property (nonatomic, retain) NSString *path;
@property (nonatomic, copy) RequestHandler handler;
@property (nonatomic, assign) id target;
@property (nonatomic, assign) SEL selector;
@property (nonatomic, retain) NSArray *keys;

@end
