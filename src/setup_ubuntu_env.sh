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
MEDIA_LIST="libavcodec-extra-53  libvlc-dev phonon-backend-gstreamer"

ok="[Ok]"

function test-root() {
    echo "Testing root access..."
    if [[ -n "$DEBUG_MODE" ]]; then return; fi
    if [[ $EUID -ne 0 ]]; then
        echo "This script must be run as root" 1>&2
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
    apt-get install --yes ${APT_INSTALL_FLAGS} $*
    set +x
}

function apt-remove() {
    if [ -z "$1" ]
    then
        echo "warning: apt-install() function called without parameters"
        return 1;
    fi
    set -x #Show remove command to user"
    apt-get remove --yes ${APT_INSTALL_FLAGS} $*
    set +x
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

    DETECTED_PACKETS=`dpkg -l "$1" | grep "^ii" | awk '{ print $2}'`
    if [[ -n "$DETECTED_PACKETS" ]]; then
        echo "It's recommended to remove this packets from the system:"
        echo "$DETECTED_PACKETS"
        read -n 1 -i "n" -p "Are you agree to remove them [y/N]?" yn
        case $yn in
            [Yy] ) apt-remove ${DETECTED_PACKETS};;
        esac
    fi
}

cleanup-folder() {
    if [[ -n "$1" ]]; then
        rm -rf "$1"
    fi
}

install-from() {
    if [ -z "$1" ]; then
        echo "warning: install-from function called without parameters"
        return 1;
    fi
    pushd .
    cd "$1"
    make install
    popd
}

update-bashrc(){
    read -n 1 -i "y" -p "Add \"export LD_LIBRARY_PATH=/usr/local/lib\" to ~/.bashrc [Y/n]?" yn
    case $yn in
        [Yy] ) echo "export LD_LIBRARY_PATH=/usr/local/lib" >> ${HOME}/.bashrc ;;
    esac
}


### start working ###

test-root
test-inet-availability "github.com"

echo "Installing iviLink environment for ubuntu"

echo "Installing development tools"
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
