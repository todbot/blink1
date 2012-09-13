PLWeakCompatibility
===================

Do you like ARC but need to support older OSes? Do you contemplate dropping support for Mac OS X 10.6 or iOS 4 just so you can use `__weak`? Good news! Now you can use `__weak` on those older OSes by just dropping a file into your project and adding a couple of compiler flags.

`PLWeakCompatibility` is a set of stubs that implement the Objective-C runtime functions the compiler uses to make `__weak` work. It automatically calls through to the real runtime functions if they're present (i.e. your app is running on iOS5+ or Mac OS X 10.7+) and uses its own implementation if they're not.

To use `PLWeakCompatibility`:

1. Drop `PLWeakCompatibilityStubs.m` into your project.
2. Add these flags to your Other C Flags in your Xcode target settings: `-Xclang -fobjc-runtime-has-weak`.
3. There is no step 3!

Note that, by default, `PLWeakCompatibility` uses `MAZeroingWeakRef` to handle `__weak` *if* `MAZeroingWeakRef` is present. If not, it uses its own, less sophisticated, internal implementation. If you are already using `MAZeroingWeakRef`, then `PLWeakCompatibility` will take advantage of it. If you're not, you don't need it. There is nothing you need to do to enable the use of `MAZeroingWeakRef`, it will simply be used if it's in your project.


Implementation Notes
--------------------

The built-in weak reference implementation is basic but serviceable. It works by swizzling out `-release` and `-dealloc` on target classes directly. This means that every instance of any weakly referenced class takes a performance hit for those operations, even for instances which are not themselves weakly referenced.

This swizzling *should* be benign, but as with all things runtime manipulation, problems may occur. In particular, I do not anticipate weak references to bridged CoreFoundation objects working at all, and there may be conflicts with Key-Value Observing. The good news is that, since the `PLWeakCompatibility` implementation is only active on older OSes, you have stable targets to test against, and can know that future updates won't affect compatibility.


Compatibility Notes
-------------------

`PLWeakCompatibility` *should* be fully compatible with any OS/architecture/compiler combination which supports ARC. Since the calls are generated at compile time, and the stubs simply call through to Apple's implementations when available, it's extremely unlikely that a future OS update will break an app that uses `PLWeakCompatibility`. All of the tricky business happens on OSes which will not receive further updates.

It is possible that a future version of Xcode will include a compiler which does not get along with these stubs. We consider this possibility to be unlikely, but it's possible in theory. If it does happen, you may continue to build using an old compiler for as long as you support iOS 4 or Mac OS 10.6, and we also hope to be able to fix up any incompatibilities in the unlikely event that this occurs.
