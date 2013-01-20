#import <Foundation/Foundation.h>
#import "HTTPResponse.h"

// Wraps an HTTPResponse object to allow setting a custom status code
// without needing to create subclasses of every response.
@interface HTTPResponseProxy : NSObject <HTTPResponse> {
	NSObject<HTTPResponse> *response;
	NSInteger status;
}

@property (nonatomic, retain) NSObject<HTTPResponse> *response;
@property (nonatomic) NSInteger status;

- (NSInteger)customStatus;

@end
