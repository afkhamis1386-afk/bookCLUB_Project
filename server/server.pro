QT += widgets
QT += widgets sql
CONFIG += c++17

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
    AdminRepository.cpp \
    AuthorRepository.cpp \
    BookRepository.cpp \
    CartRepository.cpp \
    CategoryRepository.cpp \
    DatabaseManager.cpp \
    GenreRepository.cpp \
    NotificationRepository.cpp \
    OrderRepository.cpp \
    PaymentRepository.cpp \
    PublisherRepository.cpp \
    RatingRepository.cpp \
    ReviewRepository.cpp \
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
    AdminRepository.h \
    AuthorRepository.h \
    BookRepository.h \
    CartRepository.h \
    CategoryRepository.h \
    DatabaseManager.h \
    GenreRepository.h \
    NotificationRepository.h \
    OrderRepository.h \
    PaymentRepository.h \
    PublisherRepository.h \
    RatingRepository.h \
    ReviewRepository.h \
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