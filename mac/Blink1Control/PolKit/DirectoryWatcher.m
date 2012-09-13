/*
	This file is part of the PolKit library.
	Copyright (C) 2008-2009 Pierre-Olivier Latour <info@pol-online.net>
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#import "DirectoryWatcher.h"

static void _FSEventCallback(ConstFSEventStreamRef streamRef, void* clientCallBackInfo, size_t numEvents, void* eventPaths, const FSEventStreamEventFlags eventFlags[], const FSEventStreamEventId eventIds[])
{
	NSAutoreleasePool*		pool = [NSAutoreleasePool new];
	DirectoryWatcher*		watcher = (DirectoryWatcher*)clientCallBackInfo;
	size_t					i;
	CFStringRef				path;
	
	for(i = 0; i < numEvents; ++i) {
		if(eventFlags[i] & (kFSEventStreamEventFlagMount | kFSEventStreamEventFlagUnmount))
		continue;
		
		path = CFArrayGetValueAtIndex((CFArrayRef)eventPaths, i);
		if(eventFlags[i] & kFSEventStreamEventFlagRootChanged)
		[[watcher delegate] directoryWatcherRootDidChange:watcher];
		else {
			if(eventFlags[i] & kFSEventStreamEventFlagMustScanSubDirs)
			[[watcher delegate] directoryWatcher:watcher didUpdate:(NSString*)path recursively:YES eventID:eventIds[i]];
			else
			[[watcher delegate] directoryWatcher:watcher didUpdate:(NSString*)path recursively:NO eventID:eventIds[i]];
			
			if(eventFlags[i] & kFSEventStreamEventFlagHistoryDone) {
				if([[watcher delegate] respondsToSelector:@selector(directoryWatcherDidCompleteHistory:)])
				[[watcher delegate] directoryWatcherDidCompleteHistory:watcher];
			}
		}
	}
	
	[pool drain];
}

@implementation DirectoryWatcher

@synthesize rootDirectory=_rootDirectory, watching=_running, delegate=_delegate;

- (id) initWithRootDirectory:(NSString*)rootDirectory latency:(NSTimeInterval)latency lastEventID:(FSEventStreamEventId)eventID
{
	FSEventStreamContext	context = {0, self, NULL, NULL, NULL};
	
	if(rootDirectory == nil) {
		[self release];
		return nil;
	}
	
	if((self = [super init])) {
		_rootDirectory = [[rootDirectory stringByStandardizingPath] copy];
		
		_eventStream = FSEventStreamCreate(kCFAllocatorDefault, _FSEventCallback, &context, (CFArrayRef)[NSArray arrayWithObject:_rootDirectory], eventID, latency, kFSEventStreamCreateFlagUseCFTypes | kFSEventStreamCreateFlagWatchRoot);
		if(_eventStream == NULL) {
			[self release];
			return nil;
		}
		FSEventStreamScheduleWithRunLoop(_eventStream, [[NSRunLoop currentRunLoop] getCFRunLoop], kCFRunLoopCommonModes);
	}
	
	return self;
}

- (void) _cleanUp_DirectoryWatcher
{
	[self stopWatching];
	
	if(_eventStream) {
		FSEventStreamUnscheduleFromRunLoop(_eventStream, [[NSRunLoop currentRunLoop] getCFRunLoop], kCFRunLoopCommonModes);
		FSEventStreamInvalidate(_eventStream);
		FSEventStreamRelease(_eventStream);
	}
}

- (void) finalize
{
	[self _cleanUp_DirectoryWatcher];
	
	[super finalize];
}

- (void) dealloc
{
	[self _cleanUp_DirectoryWatcher];
	
	[_rootDirectory release];
	
	[super dealloc];
}

- (void) startWatching
{
	if(!_running) {
		if([_delegate respondsToSelector:@selector(directoryWatcherWillStart:)])
		[_delegate directoryWatcherWillStart:self];
		
		if(FSEventStreamStart(_eventStream))
		_running = YES;
	}
}

- (void) stopWatching
{
	if(_running) {
		FSEventStreamFlushSync(_eventStream);
		FSEventStreamStop(_eventStream);
		
		_running = NO;
		if([_delegate respondsToSelector:@selector(directoryWatcherDidStop:)])
		[_delegate directoryWatcherDidStop:self];
	}
}

@end
