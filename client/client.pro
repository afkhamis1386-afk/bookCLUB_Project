QT += widgets

QT +=network

CONFIG += c++17

INCLUDEPATH += ../common

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../common/Admin.cpp \
    ../common/Author.cpp \
    ../common/Book.cpp \
    ../common/Cart.cpp \
    ../common/Category.cpp \
    ../common/Genre.cpp \
    ../common/Notification.cpp \
    ../common/Payment.cpp \
    ../common/Rating.cpp \
    ../common/Request.cpp \
    ../common/Response.cpp \
    ../common/Review.cpp \
    ../common/Shelf.cpp \
    ../common/TimedDiscount.cpp \
    ../common/Order.cpp \
    ../common/publisher.cpp \
    AdminController.cpp \
    BookReaderController.cpp \
    BookStoreController.cpp \
    CartController.cpp \
    ClientSocket.cpp \
    GenreSelectionWindow_c.cpp \
    LibraryController.cpp \
    LoginController.cpp \
    NetworkManager.cpp \
    NotificationController.cpp \
    ProfileController.cpp \
    PublisherBookController.cpp \
    PublisherDashboardController.cpp \
    RatingController.cpp \
    ReviewController.cpp \
    SavedBookController.cpp \
    bookcardwidget.cpp \
    forgotpasswordwindow_c.cpp \
    homewindow_c.cpp \
    main.cpp \
    loginwindow_c.cpp \
    registerwindow_c.cpp

HEADERS += \
    ../common/Admin.h \
    ../common/Author.h \
    ../common/Book.h \
    ../common/Cart.h \
    ../common/Category.h \
    ../common/Enums.h \
    ../common/Genre.h \
    ../common/Notification.h \
    ../common/Payment.h \
    ../common/Rating.h \
    ../common/Request.h \
    ../common/Response.h \
    ../common/Review.h \
    ../common/Shelf.h \
    ../common/TimedDiscount.h \
    ../common/Order.h \
    ../common/publisher.h \
    AdminController.h \
    BookReaderController.h \
    BookStoreController.h \
    CartController.h \
    ClientSocket.h \
    GenreSelectionWindow_c.h \
    LibraryController.h \
    LoginController.h \
    NetworkManager.h \
    NotificationController.h \
    ProfileController.h \
    PublisherBookController.h \
    PublisherDashboardController.h \
    RatingController.h \
    ReviewController.h \
    SavedBookController.h \
    bookcardwidget.h \
    forgotpasswordwindow_c.h \
    homewindow_c.h \
    loginwindow_c.h \
    registerwindow_c.h

FORMS += \
    GenreSelectionWindow_c.ui \
    forgotpasswordwindow_c.ui \
    homewindow_c.ui \
    loginwindow_c.ui \
    registerwindow_c.ui

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

RESOURCES += \
    resources.qrc