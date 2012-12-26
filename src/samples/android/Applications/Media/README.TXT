How to build the vlc server for MediaControlServerProfile:
0. prereqs: everything that is needed to build iviLink for Android, and: autoconf, (TODO)
1. Add the following lines to your ~/.bashrc file:
export ANDROID_SDK=/path/to/android/sdk (your actual path)
export ANDROID_NDK=/path/to/android/ndk (your actual path)
export ANDROID_ABI=armeabi-v7a (most common, see http://wiki.videolan.org/AndroidCompile for other options)
1. clone https://github.com/elenabratanova/vlc-ports-android-fork
2. cd vlc-ports-android-fork
3. run "sh compile.sh"
4. cd vlc-android/libs/armeabi-v7a (this path changes depending on what you set as the ANDROID_ABI variable)
5. take all the *.so files and put them into /path/to/your/iviLink/repo/src/framework/libraries/AndroidPrebuilt/vlc
6. cd /path/to/your/iviLink/repo/src
7. run "ant build-android" to rebuild all, including the Android Media sample
