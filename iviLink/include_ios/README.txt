How to use iOS version of iviLink:

Seting up:
1) Import "iviLink Framework.xcodeproj" into your Xcode workspace.
2) Drag and drop the iviLink project onto your project.
3) Open your project file, there open your target.
4) In the "Build Phases" tab add "iviLink" in "Target Dependencies"
5) In the "Build Phases" tab add "libiviLink.a" in "Link Binary With Libraries"
6) If you try to build right away, you will get linker errors, see "Key differences with other versions" part 0.
7) In your project's "Build Settings" set "C++ Standard Library" to libstdc++

Using the SDK:
Please refer to general documentation for information on such iviLink concepts as profiles, services and applications.

Enabling logging:
By default, logging is turned off (because it severely affects performance). To enable logging,
in the iviLink's project properties in "Build Settings"->"Apple LLVM compiler - Preprocessing" add "LOG_ENABLED=1" to
preprocessor macros.

Key differences with other versions:

0) You must implement the ServicesInfoProvider class (it is used instead of xml files on other platforms, its main purpose 
is to inform iviLink of known services and profiles and create them when requested). If service is not listed in the 
ServicesInfoProvider's map (getKnownServices()), you won't be able to load it.  You should not try optimizing by creating a 
profile instance only once and then just returning the pointer to a previously created object - it will most likely crash 
your application as soon as the profile is requested for the second time. No default implementation is provided in the library.

1) As Apple does not allow dynamic linking, all profile implementations are contained inside your binary, and not as separate 
dynamic libraries.

2) Framework's headers are included like <iviLink/foo.hpp>, not "foo.hpp".

3) All components run in the same process.

4) First call to initInIVILink() causes the start of the core stack (when the core stack becomes ready, all inited Applications 
will receive the onLinkUp() callback. To check if connection is available, call Application's isLinkAlive())

5) If physical connection is lost, all services are unloaded automatically and onServiceDropped is not called in this case (you
will receive onPhysicalLinkDown instead).

6) Application's onInitDone is never called, and getLaunchInfo always returns STARTED_BY_USER (these methods are left for API 
compatibility). A general pattern on other platforms is to load service from the onInitDone(..) call if the application has been 
started by user. We recommend using onLinkUp() as a signal to load all those services you want to be loaded right off the bat.

7) iOS's implementation has no means to create an instance of Application for an incoming service. However, if an application 
has been initialized in iviLink and has the requested service enabled, it will receive the incoming service.

8) You cannot load/unload a service from the onIncomingServiceBeforeLoading (an error will be returned if you try, as well as 
if you try to dispatch_ on the callback's queue), and you shouldn't perform any lengthy operations on this callback's queue.
 
