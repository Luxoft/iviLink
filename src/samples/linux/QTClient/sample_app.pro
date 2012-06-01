# Add more folders to ship with the application, here
folder_01.source = qml/test06_seat_app
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xEBD8E77C

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# use for media player
QT += declarative
# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

    CONFIG += mobility
    MOBILITY += multimedia


# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components
# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    writeMessage.cpp \
    VideoContainer.cpp \
    Profiles.cpp \
    CStateUpdater.cpp \
    CBtnProcessor.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    btnprocessor.h \
    common.h \
    writeMessage.h \
    VideoContainer.h \
    ColorFilterPrWidget.h \
    Profiles.hpp \
    CStateUpdater.hpp \
    CBtnProcessor.hpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../json/lib/release/ -lj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../json/lib/debug/ -lj
else:symbian: LIBS += -lj
else:unix:
LIBS += -Wl,--start-group
LIBS += -L$$PWD/../../json/lib/ -ljson
LIBS += -L$$PWD/ -llog4cplus
LIBS += -L$$PWD/ -lUtils
LIBS += -L$$PWD/ -lIpc
LIBS += -L$$PWD/ -lThreads
LIBS += -L$$PWD/ -lHAL
LIBS += -L$$PWD/ -lL0
LIBS += -L$$PWD/ -lmessages
LIBS += -L$$PWD/ -lConnectivityAgent
LIBS += -L$$PWD/ -ltube
LIBS += -L$$PWD/ -lConnectivityAgent
LIBS += -L$$PWD/ -lnegotiator_client
LIBS += -Wl,--end-group
LIBS+= -Wl,-rpath=./

#LIBS += -L$$PWD/ -lL0



INCLUDEPATH += $$PWD/../../json/include
INCLUDEPATH += $$PWD/../../../../../utils/threads/include
INCLUDEPATH += $$PWD/../../../../../utils/misc/include
INCLUDEPATH += $$PWD/../../../../../framework/components/ChannelSupervisor/Tube/include
INCLUDEPATH += $$PWD/../../../../../framework/components/ChannelSupervisor/common/include
INCLUDEPATH += $$PWD/../../../../../../src_products/3rd/include

DEPENDPATH += $$PWD/../../json/include
DEPENDPATH += $$PWD/../../../../../utils/threads/include
DEPENDPATH += $$PWD/../../../../../utils/misc/include
DEPENDPATH += $$PWD/../../../../../framework/components/ChannelSupervisor/Tube/include
DEPENDPATH += $$PWD/../../../../../framework/components/ChannelSupervisor/common/include

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../json/lib/release/j.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../json/lib/debug/j.lib
else:unix:!symbian: PRE_TARGETDEPS += $$PWD/../../json/lib/libjson.a
