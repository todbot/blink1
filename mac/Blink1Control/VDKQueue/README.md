VDKQueue
=======

A modern, faster, better version of UKKQueue.

<http://incident57.com/codekit>


about
-----

VDKQueue is an Objective-C wrapper around kernel queues (kQueues).
It allows you to watch a file or folder for changes and be notified when they occur.

VDKQueue is a modern, streamlined and much faster version of UKKQueue, which was originally written in 2003 by Uli Kusterer.
Objective-C has come a long way in the past nine years and UKKQueue was long in the tooth. VDKQueue is better in several ways:

	-- The number of method calls is vastly reduced.
	-- Grand Central Dispatch is used in place of Uli's "threadProxy" notifications (much faster)
	-- Memory footprint is roughly halved, since VDKQueue creates less overhead
	-- Fewer locks are taken, especially in loops (faster)
	-- The @autoreleasepool construct is used in place of alloc/init-ing an NSAutoReleasePool (much faster)
	-- The code is *much* cleaner and simpler!
	-- There is only one .h and one .m file to include.
	
VDKQueue also fixes long-standing bugs in UKKQueue. For example: OS X limits the number of open file descriptors each process
may have to about 3,000. If UKKQueue fails to open a new file descriptor because it has hit this limit, it will crash. VDKQueue will not.
	
	
	
performance
-----------

Adding 1,945 file paths to a UKKQueue instance took, on average, 80ms. 
Adding those same files to a VDKQueue instance took, on average, 65ms.

VDKQueue processes and pushes out notifications about file changes roughly 50-70% faster than UKKQueue.

All tests conducted on a 2008 MacBook Pro 2.5Ghz with 4GB of RAM running OS 10.7.3 using Xcode and Instruments (time profiler).

	


requirements
------------

As published, VDKQueue requires that you use Xcode 4.2+ and link against the 10.7 framework (for the @autoreleasepool language feature).
However, if you want to use VDKQueue on 10.6, you can simply replace the @autoreleasepool with an alloc/init-ed NSAutoReleasePool. If you do,
VDKQueue will work just fine on 10.6+

VDKQueue does not support garbage collection. If you use garbage collection, you are lazy. Shape up. (Also, GC is deprecated in OS 10.8.)
VDKQueue does not currently use ARC (automatic reference counting), although it should be straightforward to convert if you wish.




license
-------

Copyright (c) 2012 Bryan D K Jones.
You are free to use, modify and redistribute this software subject to these conditions:
      1) I am not liable for anything that happens to you if you use this software --- including if it becomes sentient and eats your grandmother.
      2) You keep this notice in your derivative work.
      3) You keep Uli Kusterer's original copyright notice as well (this notice appears at the bottom of the header file.)
      4) You are awesome.