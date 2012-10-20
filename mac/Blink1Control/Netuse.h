//
//  Netuse.h
//  Blink1Control
//
// from: http://www.ragingmenace.com/software/menumeters/
//
//  Created by Tod E. Kurt on 10/20/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <sys/types.h>
#import <sys/sysctl.h>
#import <sys/socket.h>
#import <net/if.h>
#import <net/if_dl.h>
#import <net/if_var.h>
#import <net/route.h>
#import <limits.h>

@interface Netuse : NSObject {
    
    // Old data for containing prior reads
	NSMutableDictionary		*lastData;
	// Buffer we keep around
	size_t					sysctlBufferSize;
	uint8_t					*sysctlBuffer;
    
}

// return a kByte 'netuse' value
// this is a sum of delta in & out of all network interfaces
- (int) getNetuse;

// Net usage info
- (NSDictionary *)netStatsForInterval:(NSTimeInterval)sampleInterval;

@end
