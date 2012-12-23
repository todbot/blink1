//
//  Blink1Input.m
//  Blink1Control
//
//  Created by Tod E. Kurt on 10/18/12.
//  Copyright (c) 2012 ThingM. All rights reserved.
//

#import "Blink1Input.h"

@implementation Blink1Input

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

