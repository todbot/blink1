#import "Route.h"


@implementation Route

@synthesize path;
@synthesize handler;
@synthesize target;
@synthesize selector;
@synthesize keys;

- (void)dealloc {
	self.path = nil;
	self.keys = nil;
	self.handler = nil;
	[super dealloc];
}

@end
