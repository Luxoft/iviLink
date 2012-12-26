TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

DEFINES += USELOG4CPLUS
INCLUDEPATH += "../../../../src_products/3rd_party/include"
INCLUDEPATH += "../../.."

HEADERS += \
    ScreenSharingViewer.hpp

SOURCES += \
    ScreenSharingViewer.cpp \
    main.cpp
