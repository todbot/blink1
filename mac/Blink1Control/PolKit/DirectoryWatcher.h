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

#import <Foundation/Foundation.h>

@class DirectoryWatcher;

@protocol DirectoryWatcherDelegate <NSObject>
@required
- (void) directoryWatcherRootDidChange:(DirectoryWatcher*)watcher;
- (void) directoryWatcher:(DirectoryWatcher*)watcher didUpdate:(NSString*)path recursively:(BOOL)recursively eventID:(FSEventStreamEventId)eventID;
@optional
- (void) directoryWatcherDidCompleteHistory:(DirectoryWatcher*)watcher;
- (void) directoryWatcherWillStart:(DirectoryWatcher*)watcher;
- (void) directoryWatcherDidStop:(DirectoryWatcher*)watcher;
@end

@interface DirectoryWatcher : NSObject
{
@private
	NSString*						_rootDirectory;
	id<DirectoryWatcherDelegate>	_delegate;
	FSEventStreamRef				_eventStream;
	BOOL							_running;
}
- (id) initWithRootDirectory:(NSString*)rootDirectory latency:(NSTimeInterval)latency lastEventID:(FSEventStreamEventId)eventID;

@property(nonatomic, readonly) NSString* rootDirectory;
@property(nonatomic, assign) id<DirectoryWatcherDelegate> delegate;
@property(nonatomic, readonly, getter=isWatching) BOOL watching;

- (void) startWatching;
- (void) stopWatching;
@end
