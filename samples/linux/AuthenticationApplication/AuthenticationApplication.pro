#TEMPLATE    = app
#FORMS       = auth.ui
#SOURCES     = main.cpp \
#              AuthenticationDialog.cpp
#HEADERS     = AuthenticationDialog.hpp

#INCLUDEPATH = $(RESULT_DIR)/3rd_party/include





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

FORMS       = auth.ui

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
           AuthenticationDialog.cpp \
           CPINCode.cpp \

HEADERS  += AuthenticationDialog.hpp \
            CPINCode.hpp \
 
            
RESOURCES   = auth_style_sheet.qrc

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
