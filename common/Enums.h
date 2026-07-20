#ifndef ENUMS_H
#define ENUMS_H
#include <QDataStream>
#include <QString>
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
    GetBookFile,
    SaveReadingProgress,
    SearchBooks,
    GetBookDetails,
    GetBookCoverImage,
    GetBooksByGenre,
    GetBooksByCategory,
    GetNewestBooks,
    GetFreeBooks,
    GetRecommendedBooks,
    AddToCart,
    RemoveFromCart,
    GetCart,
    Checkout,
    SubmitReview,
    EditReview,
    DeleteReview,
    DeleteReviewByAdmin,
    GetReviewsForBook,
    SubmitRating,
    GetBookRatingSummary,
    SaveBook,
    UnsaveBook,
    GetSavedBooks,
    GetShelf,
    GetPurchasedBooks,
    CreateShelf,
    RenameShelf,
    DeleteShelf,
    AddBookToShelf,
    MoveBookBetweenShelves,
    AddBook,
    UpdateBook,
    DeactivateBook,
    ReactivateBook,
    ApplyDiscount,
    GetPublisherStats,
    GetAllUsers,
    GetNormalUserDetails,
    GetPublisherDetails,
    BlockUser,
    UnblockUser,
    DeleteUser,
    GetAllBooksAdmin,
    GetBookDetailsForReview,
    DeleteBook,
    GetAllReviews,
    GetNotifications,
    MarkNotificationRead,
    BootstrapFirstAdmin,
    CreateAdditionalAdmin,
    GetUnreadNotificationCount,
    GetAllGenres,
    GetAllCategories,
    SetFavoriteGenres,
    SetUserActiveStatus,
    GetAccountInfo,
    GetOrderHistory
};
inline QString requestTypeToString(RequestType type) {
    switch (type) {
    case RequestType::Register: return "Register";
    case RequestType::Login: return "Login";
    case RequestType::Logout: return "Logout";
    case RequestType::ChangePassword: return "ChangePassword";
    case RequestType::RecoverPassword: return "RecoverPassword";
    case RequestType::GetBooks: return "GetBooks";
    case RequestType::GetBookFile: return "GetBookFile";
    case RequestType::SaveReadingProgress: return "SaveReadingProgress";
    case RequestType::SearchBooks: return "SearchBooks";
    case RequestType::GetBookDetails: return "GetBookDetails";
    case RequestType::GetBookCoverImage: return "GetBookCoverImage";
    case RequestType::GetBooksByGenre: return "GetBooksByGenre";
    case RequestType::GetBooksByCategory: return "GetBooksByCategory";
    case RequestType::GetNewestBooks: return "GetNewestBooks";
    case RequestType::GetFreeBooks: return "GetFreeBooks";
    case RequestType::GetRecommendedBooks: return "GetRecommendedBooks";
    case RequestType::AddToCart: return "AddToCart";
    case RequestType::RemoveFromCart: return "RemoveFromCart";
    case RequestType::GetCart: return "GetCart";
    case RequestType::Checkout: return "Checkout";
    case RequestType::SubmitReview: return "SubmitReview";
    case RequestType::EditReview: return "EditReview";
    case RequestType::DeleteReview: return "DeleteReview";
    case RequestType::DeleteReviewByAdmin: return "DeleteReviewByAdmin";
    case RequestType::GetReviewsForBook: return "GetReviewsForBook";
    case RequestType::SubmitRating: return "SubmitRating";
    case RequestType::GetBookRatingSummary: return "GetBookRatingSummary";
    case RequestType::SaveBook: return "SaveBook";
    case RequestType::UnsaveBook: return "UnsaveBook";
    case RequestType::GetSavedBooks: return "GetSavedBooks";
    case RequestType::GetShelf: return "GetShelf";
    case RequestType::GetPurchasedBooks: return "GetPurchasedBooks";
    case RequestType::CreateShelf: return "CreateShelf";
    case RequestType::RenameShelf: return "RenameShelf";
    case RequestType::DeleteShelf: return "DeleteShelf";
    case RequestType::AddBookToShelf: return "AddBookToShelf";
    case RequestType::MoveBookBetweenShelves: return "MoveBookBetweenShelves";
    case RequestType::AddBook: return "AddBook";
    case RequestType::UpdateBook: return "UpdateBook";
    case RequestType::DeactivateBook: return "DeactivateBook";
    case RequestType::ReactivateBook: return "ReactivateBook";
    case RequestType::ApplyDiscount: return "ApplyDiscount";
    case RequestType::GetPublisherStats: return "GetPublisherStats";
    case RequestType::GetAllUsers: return "GetAllUsers";
    case RequestType::GetNormalUserDetails: return "GetNormalUserDetails";
    case RequestType::GetPublisherDetails: return "GetPublisherDetails";
    case RequestType::BlockUser: return "BlockUser";
    case RequestType::UnblockUser: return "UnblockUser";
    case RequestType::DeleteUser: return "DeleteUser";
    case RequestType::GetAllBooksAdmin: return "GetAllBooksAdmin";
    case RequestType::GetBookDetailsForReview: return "GetBookDetailsForReview";
    case RequestType::DeleteBook: return "DeleteBook";
    case RequestType::GetAllReviews: return "GetAllReviews";
    case RequestType::GetNotifications: return "GetNotifications";
    case RequestType::MarkNotificationRead: return "MarkNotificationRead";
    case RequestType::BootstrapFirstAdmin: return "BootstrapFirstAdmin";
    case RequestType::CreateAdditionalAdmin: return "CreateAdditionalAdmin";
    case RequestType::GetUnreadNotificationCount: return "GetUnreadNotificationCount";
    case RequestType::GetAllGenres: return "GetAllGenres";
    case RequestType::GetAllCategories: return "GetAllCategories";
    case RequestType::SetFavoriteGenres: return "SetFavoriteGenres";
    case RequestType::SetUserActiveStatus: return "SetUserActiveStatus";
    case RequestType::GetAccountInfo: return "GetAccountInfo";
    case RequestType::GetOrderHistory: return "GetOrderHistory";
    }
    return "Unknown";
}
enum class ResponseStatus {
    Success,
    Error,
    Unauthorized,
    NotFound,
    ValidationFailed,
    PushNotification
};
inline QString responseStatusToString(ResponseStatus status) {
    switch (status) {
    case ResponseStatus::Success: return "Success";
    case ResponseStatus::Error: return "Error";
    case ResponseStatus::Unauthorized: return "Unauthorized";
    case ResponseStatus::NotFound: return "NotFound";
    case ResponseStatus::ValidationFailed: return "ValidationFailed";
    case ResponseStatus::PushNotification: return "PushNotification";
    }
    return "Unknown";
}
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