# Add more folders to ship with the application, here
folder_01.source = qml/ClimateClient_App
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE4FD1D69

# Smart Installer package's UID
# This UID is from the protected range and therefore the package will
# fail to install if self-signed. By default qmake uses the unprotected
# range value if unprotected UID is defined for the application and
# 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    crequestprocessor.cpp \
    cstateupdater.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    crequestprocessor.h \
    cstateupdater.h \
    cclimateclientprofilecallbacks.hpp


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../json/lib/release/ -lj
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../json/lib/debug/ -lj
#else:symbian: LIBS += -lj
#else:unix:
#LIBS += -L$(RESULT_DIR)/utils/json/ -ljson
#LIBS += -lrt
#LIBS += -L$(RESULT_DIR)/3rd_party/lib -llog4cplus



INCLUDEPATH += utils/json/include
INCLUDEPATH += utils/threads/include
INCLUDEPATH += utils/misc/include
INCLUDEPATH += framework/components/ChannelSupervisor/Tube/include
INCLUDEPATH += framework/components/ChannelSupervisor/common/include
INCLUDEPATH += $(RESULT_DIR)/3rd_party/include


DEPENDPATH += utils/json/include
DEPENDPATH += utils/threads/include
DEPENDPATH += utils/misc/include
DEPENDPATH += framework/components/ChannelSupervisor/Tube/include
DEPENDPATH += framework/components/ChannelSupervisor/common/include
DEPENDPATH += $(RESULT_DIR)/3rd_party/include
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/jsoncpp/lib/release/ -lj
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/jsoncpp/lib/debug/ -lj
#else:symbian: LIBS += -lj
##else:unix: LIBS += -L$$PWD/jsoncpp/lib/ -lj


#INCLUDEPATH += $$PWD/ClimateClient_App/jsoncpp/include
#DEPENDPATH += $$PWD/ClimateClient_App/jsoncpp/include

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../jsoncpp/lib/release/j.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../jsoncpp/lib/debug/j.lib
# #else:unix:!symbian: PRE_TARGETDEPS += $$PWD/ClimateClient_App/jsoncpp/lib/libj.a