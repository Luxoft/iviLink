# Add more folders to ship with the application, here
folder_01.source = qml/AuthenticationApplication
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
           PINCode.cpp \
           AuthenticationStateMachine.cpp \
           AuthState.cpp \
           InitialState.cpp \
           PINSentState.cpp \
           PINReceivedState.cpp \
           ComparePINsState.cpp \
           AuthSuccessfulState.cpp \
           ReadyToExitState.cpp \
           PrivateKeyGenerationThread.cpp \
           QMLAuthenticationDialog.cpp

HEADERS  += \
            PINCode.hpp \
            AuthenticationStateMachine.hpp \
            AuthState.hpp \
            InitialState.hpp \
            PINSentState.hpp \
            PINReceivedState.hpp \
            ComparePINsState.hpp \
            AuthSuccessfulState.hpp \
            ReadyToExitState.hpp \
            PrivateKeyGenerationThread.hpp \
            IAuthenticationDialog.hpp \
            QMLAuthenticationDialog.hpp
            
RESOURCES   =

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../json/lib/release/ -lj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../json/lib/debug/ -lj
else:symbian: LIBS += -lj
else:unix:

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

OTHER_FILES += \
    qml/AuthenticationApplication/authentication.qml
