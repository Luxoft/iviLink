#!/usr/bin/env bash
# setup_env_ubuntu.sh - script setup iviLink build environment for ubuntu development
# Run it from root or using sudo command please.

set -e #Stop on any error

if [ "$1" == "--debug" ]
then
    set -x #Debug this script
    APT_INSTALL_FLAGS=${APT_INSTALL_FLAGS}" -s" #Simulate install (for debuging)
    DEBUG_MODE="On"
fi


APT_INSTALL_FLAGS=${APT_INSTALL_FLAGS}" --quiet"

DEV_TOOLS_LIST="gcc g++ make git"
QT4_TOOLS_LIST="qt4-dev-tools qt4-qmake"
PHONON_LIST="libphonon-dev"
PROTOBUF_LIST="libprotobuf-dev"
MEDIA_LIST="libavcodec-extra-53  libvlc-dev vlc phonon-backend-gstreamer"
ANDROID_DOWNLOAD_LIST="wget tar openjdk-7-jre openjdk-7-jdk ant"

ok="[Ok]"

function test-root() {
    echo "Testing root access..."
    if [[ -n "$DEBUG_MODE" ]]; then return; fi
    if [[ $EUID -eq 0 ]]; then
        echo "This script must not be run as root" 1>&2
        exit 1
    fi
    echo ${OK}
}

function test-inet-availability() {
    TEST_SITE="google.com"
    if [[ -n "$1" ]]; then
        TEST_SITE="$1"
    fi
    echo "Testing internet availability..."
    set +e
    ping -c 1 "${TEST_SITE}" > /dev/null
    if [[ $? -ne 0 ]]; then
        echo "Warning: Failed to detect Internet access!"
        read -n 1 -i "n" -p "Do you whish to continue running setup script [y/N]?" yn
        case $yn in
           [Yy] ) ;;
               *) echo; exit 2;;
        esac
    fi
    echo ${OK}
    set -e
}

function apt-install() {
    if [ -z "$1" ]
    then
        echo "warning: apt-install() function called without parameters"
        return 1;
    fi
    set -x #Show install command to user"
    sudo apt-get install --yes ${APT_INSTALL_FLAGS} $*
    set +x
}

function apt-remove() {
    if [ -z "$1" ]
    then
        echo "warning: apt-install() function called without parameters"
        return 1;
    fi
    set -x #Show remove command to user"
    sudo apt-get remove --yes ${APT_INSTALL_FLAGS} $*
    set +x
}


install-protobuf-system() {
    apt-install ${PROTOBUF_LIST}
}

checkout-from()
{
    if [ -z "$1" ]; then
        echo "warning: checkout-from function called without parameters"
        return 1;
    fi
    if [[ -d "$2" ]]; then
        echo "Directory $2 exists. checkout step skipped"
        return;
    fi
    git clone "$1" "$2"
}

build-in() {
    if [ -z "$1" ]; then
        echo "warning: build-in function called without parameters"
        return 1;
    fi
    pushd .
    cd "$1"
    ./configure
    make
    popd
}

remove-packets() {
    if [ -z "$1" ]; then
        echo "warning: remove-packets function called without parameters"
        return 1;
    fi

    DETECTED_PACKEST=`dpkg -l "$1" | grep "^ii" | awk '{ print $2}'`
    if [[ -n "$DETECTED_PACKETS" ]]; then
        echo "It's recommended to remove this packets from the system:"
        echo "$DETECTED_PACKETS"
        read -n 1 -i "n" -p "Are you agree to remove them [y/N]?" yn
        case $yn in
            [Yy] ) apt-remove ${DETECTED_PACKETS};;
        esac
    fi
}


### start working ###
echo "Installing iviLink environment for ubuntu"
test-root

echo "Installng development tools"
apt-install ${DEV_TOOLS_LIST}
echo $OK

echo "Installing Qt framework"
apt-install  ${QT4_TOOLS_LIST}
echo $OK

echo "Installing Phonon"
apt-install ${PHONON_LIST}
echo $OK

echo "Installing media libraries"
apt-install ${MEDIA_LIST}
echo $OK

echo "Installing iviLink environment for Android"
apt-install ${ANDROID_DOWNLOAD_LIST}
echo $OK

echo "Downloading Android SDK"
wget http://dl.google.com/android/android-sdk_r20.0.3-linux.tgz
echo $OK

echo "Installing Android SDK"
tar xvzf android-sdk_r20.0.3-linux.tgz 
cp -r android-sdk-linux/ ~/android-sdk-linux/
rm -rf android-sdk-linux/
rm android-sdk_r20.0.3-linux.tgz 
~/android-sdk-linux/tools/android update sdk -u --filter platform-tools,android-16,extra-android-support
~/android-sdk-linux/tools/android update sdk -u --filter 24
echo $OK

echo "Configuring your jre environment"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
echo "Enter the number corresponding to /usr/lib/jvm/java-7-openjdk-i386/jre/bin/java"
echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
sudo update-alternatives --config java
echo $OK

echo "Downloading Android NDK"
wget http://dl.google.com/android/ndk/android-ndk-r8b-linux-x86.tar.bz2
echo $OK

echo "Installing Android NDK"
tar xvf android-ndk-r8b-linux-x86.tar.bz2 
cp -r android-ndk-r8b/ ~/android-ndk-r8b/
rm -rf android-ndk-r8b/
rm android-ndk-r8b-linux-x86.tar.bz2
echo $OK

echo "Setting your PATH variable"
echo "export PATH=$PATH:~/android-sdk-linux/platform-tools/:~/android-sdk-linux/tools/:~/android-ndk-r8b/" >> ${HOME}/.bashrc
echo $OK

echo "Removing protobuf packets (the correct ones will be installed with ant setup)"
remove-packets "*protobuf*"
echo $OK

echo "Please restart your terminal to run ant setup and ant build to build the SDK"

