#import <Cocoa/Cocoa.h>
@class RoutingHTTPServer;

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) IBOutlet NSWindow *window;
@property (strong) RoutingHTTPServer *http;

- (void)setupRoutes;

@end
