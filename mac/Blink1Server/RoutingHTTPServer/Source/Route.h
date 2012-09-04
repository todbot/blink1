#import <Foundation/Foundation.h>
#import "RoutingHTTPServer.h"

@interface Route : NSObject

@property (nonatomic) NSRegularExpression *regex;
@property (nonatomic, copy) RequestHandler handler;
@property (nonatomic, weak) id target;
@property (nonatomic, assign) SEL selector;
@property (nonatomic) NSArray *keys;

@end
