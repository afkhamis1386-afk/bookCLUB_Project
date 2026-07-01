QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../common/Admin.cpp \
    ../common/Book.cpp \
    ../common/Cart.cpp \
    ../common/publisher.cpp \
    main.cpp \
    loginwindow_s.cpp

HEADERS += \
    ../common/Admin.h \
    ../common/Book.h \
    ../common/Cart.h \
    ../common/publisher.h \
    loginwindow_s.h

FORMS += \
    loginwindow_s.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
HEADERS += \
    ../common/user.h \
    ../common/normaluser.h

SOURCES += \
    ../common/user.cpp \
    ../common/normaluser.cpp