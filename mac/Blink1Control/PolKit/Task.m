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

#import <unistd.h>
#import <signal.h>

#import "Task.h"

#define kDefaultErrorCode	(-1)

@implementation Task

@synthesize delegate=_delegate, abortTimeOut=_timeOut, outputDataCapacity=_dataCapacity;

- (void) _killTimer:(NSTimer*)timer
{
	if(!_delegate || [_delegate shouldAbortTask:self]) {
		[(NSTask*)[timer userInfo] terminate];
		[(NSTask*)[timer userInfo] interrupt];
	}
}

- (void) _fileHandleDataAvailable:(NSNotification*)notification
{
	NSFileHandle*		fileHandle = [notification object];
	NSAutoreleasePool*	localPool;
	NSData*				data;
	
	localPool = [NSAutoreleasePool new];
	
	data = [fileHandle availableData];
	if(fileHandle == _outFileHandle) {
		if([data length])
		[_outData appendData:data];
		else
		_outFileHandle = nil;
	}
	else if(fileHandle == _errorFileHandle) {
		if([data length])
		[_errorData appendData:data];
		else
		_errorFileHandle = nil;
	}
	
	if([data length])
	[fileHandle waitForDataInBackgroundAndNotify];
	
	[localPool drain];
}

- (BOOL) _runTask:(NSArray*)arguments inData:(NSData*)inData outData:(NSData**)outData errorData:(NSData**)errorData status:(int*)status error:(NSString**)error
{
	NSPipe*				inPipe = nil;
	NSPipe*				outPipe = nil;
	NSPipe*				errorPipe = nil;
	BOOL				success = YES;
	NSTimer*			timer;
	NSFileHandle*		fileHandle;
	int					result;
	NSTask*				task;
	
	if(status)
	*status = 0;
	if(error)
	*error = nil;
	if(outData)
	*outData = nil;
	if(errorData)
	*errorData = nil;
	
	task = [NSTask new];
	[task setLaunchPath:_taskPath];
	if(_environment)
	[task setEnvironment:_environment];
	if(_directoryPath)
	[task setCurrentDirectoryPath:_directoryPath];
	if(arguments)
	[task setArguments:arguments];
	if(task == nil) {
		success = NO;
		goto Exit;
	}
	
	if([inData length]) {
		if((inPipe = [NSPipe new])) {
			[task setStandardInput:inPipe];
			[inPipe release];
		}
		else {
			if(status)
			*status = kDefaultErrorCode;
			if(error)
			*error = @"Failed creating standard input pipe";
			success = NO;
			goto Exit;
		}
	}
	
	if(outData) {
		if((outPipe = [NSPipe new])) {
			[task setStandardOutput:outPipe];
			[outPipe release];
		}
		else {
			if(status)
			*status = kDefaultErrorCode;
			if(error)
			*error = @"Failed creating standard output pipe";
			success = NO;
			goto Exit;
		}
	}
	
	if(errorData) {
		if((errorPipe = [NSPipe new])) {
			[task setStandardError:errorPipe];
			[errorPipe release];
		}
		else {
			if(status)
			*status = kDefaultErrorCode;
			if(error)
			*error = @"Failed creating standard error pipe";
			success = NO;
			goto Exit;
		}
	}
	
	@try {
		[task launch];
	}
	@catch(id exception) {
		if(status)
		*status = kDefaultErrorCode;
		if(error)
		*error = [NSString stringWithFormat:@"Exception raised when launching tool: %@", exception];
		success = NO;
	}
	if(success == NO)
	goto Exit;
	
	if((fileHandle = [inPipe fileHandleForWriting])) {
		@try {
			[fileHandle writeData:inData];
			[fileHandle closeFile];
		}
		@catch(id exception) {
			[task terminate];
			[task interrupt];
			if(status)
			*status = kDefaultErrorCode;
			if(error)
			*error = @"Failed writing data to standard input pipe";
			success = NO;
		}
	}
	if(success == NO)
	goto Exit;
	
	timer = (_timeOut > 0.0 ? [[NSTimer scheduledTimerWithTimeInterval:_timeOut target:self selector:@selector(_killTimer:) userInfo:task repeats:(_delegate ? YES : NO)] retain] : nil);
	if((_outFileHandle = [outPipe fileHandleForReading])) {
		*outData = _outData = [NSMutableData dataWithCapacity:_dataCapacity];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileHandleDataAvailable:) name:NSFileHandleDataAvailableNotification object:_outFileHandle];
		[_outFileHandle waitForDataInBackgroundAndNotify];
	}
	if((_errorFileHandle = [errorPipe fileHandleForReading])) {
		*errorData = _errorData = [NSMutableData data];
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(_fileHandleDataAvailable:) name:NSFileHandleDataAvailableNotification object:_errorFileHandle];
		[_errorFileHandle waitForDataInBackgroundAndNotify];
	}
	[task waitUntilExit];
	while(_outFileHandle || _errorFileHandle) {
		CFRunLoopRunInMode(kCFRunLoopDefaultMode, 1.0, true);
	}
	if((fileHandle = [errorPipe fileHandleForReading]))
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleDataAvailableNotification object:fileHandle];
	if((fileHandle = [outPipe fileHandleForReading]))
	[[NSNotificationCenter defaultCenter] removeObserver:self name:NSFileHandleDataAvailableNotification object:fileHandle];
	[timer invalidate];
	[timer release];
	if((result = [task terminationStatus])) {
		if(status)
		*status = result;
		if(error)
		*error = [NSString stringWithFormat:@"Non-zero termination status (%i - %s)", result, (result <= NSIG ? strsignal(result) : nil)];
		success = NO;
	}
	
Exit:
	[[inPipe fileHandleForReading] closeFile];
	[[inPipe fileHandleForWriting] closeFile];
	[[outPipe fileHandleForReading] closeFile];
	[[outPipe fileHandleForWriting] closeFile];
	[[errorPipe fileHandleForReading] closeFile];
	[[errorPipe fileHandleForWriting] closeFile];
	[task release];
	
	return success;
}

+ (NSString*) runWithToolPath:(NSString*)toolPath arguments:(NSArray*)arguments inputString:(NSString*)string timeOut:(NSTimeInterval)timeOut
{
	Task*			task;
	NSString*		output;
	
	task = [[self alloc] initWithToolPath:toolPath currentDirectoryPath:nil];
	[task setAbortTimeOut:timeOut];
	output = [task runWithArguments:arguments inputString:string];
	[task release];
	
	return output;
}

- (id) init
{
	return [self initWithToolPath:nil currentDirectoryPath:nil];
}

- (id) initWithToolPath:(NSString*)toolPath currentDirectoryPath:(NSString*)directoryPath
{
	return [self initWithToolPath:toolPath environment:nil currentDirectoryPath:directoryPath];
}

- (id) initWithToolPath:(NSString*)toolPath environment:(NSDictionary*)environment currentDirectoryPath:(NSString*)directoryPath
{
	if((self = [super init])) {
		_taskPath = [toolPath copy];
		_environment = [environment copy];
		_directoryPath = [directoryPath copy];
	}
	
	return self;
}

- (NSString*) runWithArguments:(NSArray*)arguments inputString:(NSString*)string
{
	NSError*			error;
	NSData*				data;
	
	error = [self runWithInputData:[string dataUsingEncoding:NSUTF8StringEncoding] arguments:arguments outputData:&data];
	if(error) {
#ifdef __DEBUG__
		NSLog(@"%s: %@ when executing \"%@\" with arguments:\n%@\n", __FUNCTION__, [error localizedDescription], _taskPath, [arguments description]);
		if([error localizedFailureReason])
		NSLog(@"%s: %@\n", __FUNCTION__, [error localizedFailureReason]);
		if([data length])
		NSLog(@"%s: %@\n", __FUNCTION__, [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]);
#endif
		return nil;
	}
	
	return [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
}

- (NSError*) runWithInputData:(NSData*)inputData arguments:(NSArray*)arguments outputData:(NSData**)outputData
{
	NSError*			error = nil;
	NSData*				errorData;
	NSString*			description;
	NSString*			string;
	int					status;
	
	if(![self _runTask:arguments inData:inputData outData:outputData errorData:&errorData status:&status error:&description]) {
		string = ([errorData length] ? [[[NSString alloc] initWithData:errorData encoding:NSUTF8StringEncoding] autorelease] : nil);
		error = [NSError errorWithDomain:NSPOSIXErrorDomain code:status userInfo:[NSDictionary dictionaryWithObjectsAndKeys:description, NSLocalizedDescriptionKey, string, NSLocalizedFailureReasonErrorKey, nil]];
	}
	
	return error;
}

- (int) runWithInputData:(NSData*)inputData arguments:(NSArray*)arguments outputData:(NSData**)outputData errorData:(NSData**)errorData
{
	int					status;
	
	if(![self _runTask:arguments inData:inputData outData:outputData errorData:errorData status:&status error:NULL] && !status)
	status = -1;
	
	return status;
}

- (void) dealloc
{
	[_directoryPath release];
	[_environment release];
	[_taskPath release];
	
	[super dealloc];
}

@end
