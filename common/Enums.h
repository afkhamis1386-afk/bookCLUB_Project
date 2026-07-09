#ifndef ENUMS_H
#define ENUMS_H
#include <QDataStream>
enum class UserRole {
    NormalUser = 0,
    Publisher  = 1,
    Admin      = 2
};
enum class RequestType {
    Register,
    Login,
    Logout,
    ChangePassword,
    RecoverPassword,
    GetBooks,
    SearchBooks,
    GetBookDetails,
    AddToCart,
    RemoveFromCart,
    Checkout,
    SubmitReview,
    SubmitRating,
    SaveBook,
    GetShelf,
    AddBook,
    UpdateBook,
    DeactivateBook,
    GetPublisherStats,
    GetAllUsers,
    BlockUser,
    DeleteUser,
    DeleteBook,
    GetNotifications,
    MarkNotificationRead
};
enum class ResponseStatus {
    Success,
    Error,
    Unauthorized,
    NotFound,
    ValidationFailed,
    PushNotification
};
inline QDataStream &operator<<(QDataStream &out, UserRole role) {
    out << static_cast<int>(role);
    return out;
}
inline QDataStream &operator>>(QDataStream &in, UserRole &role) {
    int v;
    in >> v;
    role = static_cast<UserRole>(v);
    return in;
}
inline QDataStream &operator<<(QDataStream &out, RequestType t) {
    out << static_cast<int>(t);
    return out;
}
inline QDataStream &operator>>(QDataStream &in, RequestType &t) {
    int v;
    in >> v;
    t = static_cast<RequestType>(v);
    return in;
}
inline QDataStream &operator<<(QDataStream &out, ResponseStatus s) {
    out << static_cast<int>(s);
    return out;
}
inline QDataStream &operator>>(QDataStream &in, ResponseStatus &s) {
    int v;
    in >> v;
    s = static_cast<ResponseStatus>(v);
    return in;
}
#endif // ENUMS_H
