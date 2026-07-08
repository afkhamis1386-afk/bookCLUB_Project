QT += widgets
QT += widgets sql
CONFIG += c++17
QT += core network

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
    ../common/Order.cpp \
    ../common/Payment.cpp \
    ../common/Rating.cpp \
    ../common/Request.cpp \
    ../common/Response.cpp \
    ../common/Review.cpp \
    ../common/Shelf.cpp \
    ../common/TimedDiscount.cpp \
    ../common/publisher.cpp \
    AccessControl.cpp \
    AdminManager.cpp \
    AdminRepository.cpp \
    AuthManager.cpp \
    AuthorRepository.cpp \
    BookManager.cpp \
    BookRepository.cpp \
    CartManager.cpp \
    CartRepository.cpp \
    CategoryRepository.cpp \
    ClientHandler.cpp \
    DatabaseManager.cpp \
    GenreRepository.cpp \
    NotificationManager.cpp \
    NotificationRepository.cpp \
    OrderManager.cpp \
    OrderRepository.cpp \
    PaymentManager.cpp \
    PaymentRepository.cpp \
    PriceCalculator.cpp \
    PublisherManager.cpp \
    PublisherRepository.cpp \
    RatingManager.cpp \
    RatingRepository.cpp \
    ReviewManager.cpp \
    ReviewRepository.cpp \
    ShelfManager.cpp \
    ShelfRepository.cpp \
    TimedDiscountRepository.cpp \
    UserRepository.cpp \
    main.cpp \
    loginwindow_s.cpp

HEADERS += \
    ../common/Admin.h \
    ../common/Author.h \
    ../common/Book.h \
    ../common/Cart.h \
    ../common/Category.h \
    ../common/Enums.h \
    ../common/Genre.h \
    ../common/Notification.h \
    ../common/Order.h \
    ../common/Payment.h \
    ../common/Rating.h \
    ../common/Request.h \
    ../common/Response.h \
    ../common/Review.h \
    ../common/Shelf.h \
    ../common/TimedDiscount.h \
    ../common/publisher.h \
    AccessControl.h \
    AdminManager.h \
    AdminRepository.h \
    AuthManager.h \
    AuthorRepository.h \
    BookManager.h \
    BookRepository.h \
    CartManager.h \
    CartRepository.h \
    CategoryRepository.h \
    ClientHandler.h \
    DatabaseManager.h \
    GenreRepository.h \
    NotificationManager.h \
    NotificationRepository.h \
    OrderManager.h \
    OrderRepository.h \
    PaymentManager.h \
    PaymentRepository.h \
    PriceCalculator.h \
    PublisherManager.h \
    PublisherRepository.h \
    RatingManager.h \
    RatingRepository.h \
    ReviewManager.h \
    ReviewRepository.h \
    ShelfManager.h \
    ShelfRepository.h \
    TimedDiscountRepository.h \
    UserRepository.h \
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