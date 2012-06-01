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
QT += phonon
QT += declarative
# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

    CONFIG += mobility
    MOBILITY += multimedia

#QMAKE_CXXFLAGS += -DUSELOG4CPLUS
#QMAKE_LFLAGS += -L$(RESULT_DIR)/3rd_party/lib -llog4cplus
# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# Add dependency to Symbian components
# CONFIG += qt-components
# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += QTClient/main.cpp \
    QTClient/writeMessage.cpp \
    QTClient/VideoContainer.cpp \
    QTClient/CStateUpdater.cpp \
    QTClient/CBtnProcessor.cpp

# Please do not modify the following two lines. Required for deployment.
include(QTClient/qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    QTClient/common.h \
    QTClient/writeMessage.h \
    QTClient/VideoContainer.h \
    QTClient/ColorFilterPrWidget.h \
    QTClient/CStateUpdater.hpp \
    QTClient/CBtnProcessor.hpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../json/lib/release/ -lj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../json/lib/debug/ -lj
else:symbian: LIBS += -lj
else:unix:
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
