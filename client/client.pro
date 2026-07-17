QT += widgets

QT +=network

QT += pdf pdfwidgets

CONFIG += c++17

INCLUDEPATH += ../common
QT += pdf pdfwidgets

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
    adminmainwindow.cpp \
    bookcardwidget.cpp \
    bookdetailswindow_c.cpp \
    bookreaderwindow_c.cpp \
    cartwindow_c.cpp \
    forgotpasswordwindow_c.cpp \
    homewindow_c.cpp \
    librarywindow_c.cpp \
    main.cpp \
    loginwindow_c.cpp \
    notificationwindow_c.cpp \
    profilewindow_c.cpp \
    publisheraddbookwindow_c.cpp \
    publishermainwindow.cpp \
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
    adminmainwindow.h \
    bookcardwidget.h \
    bookdetailswindow_c.h \
    bookreaderwindow_c.h \
    cartwindow_c.h \
    forgotpasswordwindow_c.h \
    homewindow_c.h \
    librarywindow_c.h \
    loginwindow_c.h \
    notificationwindow_c.h \
    profilewindow_c.h \
    publisheraddbookwindow_c.h \
    publishermainwindow.h \
    registerwindow_c.h

FORMS += \
    GenreSelectionWindow_c.ui \
    adminmainwindow.ui \
    bookdetailswindow_c.ui \
    bookreaderwindow_c.ui \
    cartwindow_c.ui \
    forgotpasswordwindow_c.ui \
    homewindow_c.ui \
    librarywindow_c.ui \
    loginwindow_c.ui \
    notificationwindow_c.ui \
    profilewindow_c.ui \
    publisheraddbookwindow_c.ui \
    publishermainwindow.ui \
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


win32-msvc*: QMAKE_CXXFLAGS += /utf-8

