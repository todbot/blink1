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

@class Task;

@protocol TaskDelegate <NSObject>
- (BOOL) shouldAbortTask:(Task*)task;
@end

@interface Task : NSObject
{
@private
	NSString*				_taskPath;
	NSDictionary*			_environment;
	NSString*				_directoryPath;
	id<TaskDelegate>		_delegate;
	NSTimeInterval			_timeOut;
	NSUInteger				_dataCapacity;
	
	NSFileHandle*			_outFileHandle;
	NSMutableData*			_outData;
	NSFileHandle*			_errorFileHandle;
	NSMutableData*			_errorData;
}
@property(nonatomic, assign) id<TaskDelegate> delegate;
@property(nonatomic) NSTimeInterval abortTimeOut; //0 by default
@property(nonatomic) NSUInteger outputDataCapacity; //0 by default (automatic)

+ (NSString*) runWithToolPath:(NSString*)toolPath arguments:(NSArray*)arguments inputString:(NSString*)string timeOut:(NSTimeInterval)timeOut;

- (id) initWithToolPath:(NSString*)toolPath currentDirectoryPath:(NSString*)directoryPath;
- (id) initWithToolPath:(NSString*)toolPath environment:(NSDictionary*)environment currentDirectoryPath:(NSString*)directoryPath;
- (NSString*) runWithArguments:(NSArray*)arguments inputString:(NSString*)string;
- (NSError*) runWithInputData:(NSData*)inputData arguments:(NSArray*)arguments outputData:(NSData**)outputData;
- (int) runWithInputData:(NSData*)inputData arguments:(NSArray*)arguments outputData:(NSData**)outputData errorData:(NSData**)errorData;
@end
