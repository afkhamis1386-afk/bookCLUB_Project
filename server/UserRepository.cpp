#include "DatabaseManager.h"
#include "UserRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
UserRepository::UserRepository() {}
int UserRepository::insertNormalUser(const NormalUser &user) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    if (!db.transaction()) {
        qWarning() << "خطا در شروع تراکنش:" << db.lastError().text();
        return -1;
    }
    QSqlQuery insertUser(db);
    insertUser.prepare(
        "INSERT INTO Users (Username, PasswordHash, IsBlocked, IsDeleted, RegisterDate, RoleID) "
        "OUTPUT INSERTED.UserID "
        "VALUES (:username, :passwordHash, :isBlocked, :isDeleted, :registerDate, :roleId)"
        );
    insertUser.bindValue(":username", user.getEncryptedUsername());
    insertUser.bindValue(":passwordHash", user.getPasswordHash());
    insertUser.bindValue(":isBlocked", user.getIsBlocked());
    insertUser.bindValue(":isDeleted", user.getIsDeleted());
    insertUser.bindValue(":registerDate", user.getRegisterDate());
    insertUser.bindValue(":roleId", static_cast<int>(UserRole::NormalUser) + 1);
    if (!insertUser.exec() || !insertUser.next()) {
        qWarning() << "خطا در ثبت کاربر جدید:" << insertUser.lastError().text();
        db.rollback();
        return -1;
    }
    int newUserId = insertUser.value(0).toInt();
    QSqlQuery insertNormal(db);
    insertNormal.prepare( "INSERT INTO NormalUsers (UserID, SecurityAnswerHash) VALUES (:userId, :answerHash)" );
    insertNormal.bindValue(":userId", newUserId);
    insertNormal.bindValue(":answerHash", user.getHashedSecurityAnswer());
    if (!insertNormal.exec()) {
        qWarning() << "خطا در ثبت NormalUser:" << insertNormal.lastError().text();
        db.rollback();
        return -1;
    }
    if (!db.commit()) {
        qWarning() << "خطا در نهایی‌سازی تراکنش ثبت نام:" << db.lastError().text();
        db.rollback();
        return -1;
    }
    return newUserId;
}
NormalUser* UserRepository::loadNormalUserById(int userId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID, u.Username, u.PasswordHash, nu.SecurityAnswerHash, "
        "u.IsBlocked, u.IsDeleted, u.IsActive, u.RegisterDate "
        "FROM Users u JOIN NormalUsers nu ON u.UserID = nu.UserID "
        "WHERE u.UserID = :userId"
        );
    query.bindValue(":userId", userId);
    if (!query.exec() || !query.next()) {
        qWarning() << "کاربر عادی یافت نشد:" << query.lastError().text();
        return nullptr;
    }
    NormalUser *user = new NormalUser(
        query.value(0).toInt(),
        query.value(1).toString(),
        query.value(2).toString(),
        query.value(3).toString(),
        query.value(4).toBool(),
        query.value(5).toBool(),
        query.value(6).toBool(),
        query.value(7).toDateTime()
        );
    user->setFavoriteGenres(getFavoriteGenreIds(userId));
    user->setPurchasedBooks(getPurchasedBookIds(userId));
    user->setSavedBooks(getSavedBookIds(userId));
    user->setReadingProgress(getReadingProgress(userId));
    return user;
}
bool UserRepository::findUserRoleAndId(const QString &encryptedUsername, int &outUserId, UserRole &outRole) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT UserID, RoleID FROM Users WHERE Username = :username AND IsDeleted = 0");
    query.bindValue(":username", encryptedUsername);
    if (!query.exec() || !query.next()) {
        return false;
    }
    outUserId = query.value(0).toInt();
    int roleId = query.value(1).toInt();
    outRole = static_cast<UserRole>(roleId - 1);
    return true;
}
bool UserRepository::updateBlockedStatus(int userId, bool isBlocked) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Users SET IsBlocked = :isBlocked WHERE UserID = :userId");
    query.bindValue(":isBlocked", isBlocked);
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qWarning() << "خطا در تغییر وضعیت Block:" << query.lastError().text();
        return false;
    }
    return true;
}
bool UserRepository::updateDeletedStatus(int userId, bool isDeleted) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Users SET IsDeleted = :isDeleted WHERE UserID = :userId");
    query.bindValue(":isDeleted", isDeleted);
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کاربر:" << query.lastError().text();
        return false;
    }
    return true;
}
bool UserRepository::updateActiveStatus(int userId, bool active) {
    QSqlQuery query(DatabaseManager::getInstance()->getConnection());
    query.prepare("UPDATE Users SET IsActive = :active WHERE UserID = :userId");
    query.bindValue(":active", active);
    query.bindValue(":userId", userId);
    return query.exec();
}
bool UserRepository::updatePasswordHash(int userId, const QString &newPasswordHash) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("UPDATE Users SET PasswordHash = :hash WHERE UserID = :userId");
    query.bindValue(":hash", newPasswordHash);
    query.bindValue(":userId", userId);
    if (!query.exec()) {
        qWarning() << "خطا در تغییر رمز عبور:" << query.lastError().text();
        return false;
    }
    return true;
}
QVector<int> UserRepository::getAllUserIds() {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT UserID FROM Users WHERE IsDeleted = 0");
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> UserRepository::getAllNormalUserIds() {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "SELECT u.UserID FROM Users u JOIN NormalUsers nu ON u.UserID = nu.UserID "
        "WHERE u.IsDeleted = 0"
        );
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> UserRepository::getFavoriteGenreIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT GenreID FROM FavouriteGenre WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool UserRepository::setFavoriteGenreIds(int userId, const QVector<int> &genreIds) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery deleteQuery(db);
    deleteQuery.prepare("DELETE FROM FavouriteGenre WHERE UserID = :userId");
    deleteQuery.bindValue(":userId", userId);
    if (!deleteQuery.exec()) {
        qWarning() << "خطا در حذف ژانرهای قبلی:" << deleteQuery.lastError().text();
        return false;
    }
    for (int genreId : genreIds) {
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO FavouriteGenre (UserID, GenreID) VALUES (:userId, :genreId)");
        insertQuery.bindValue(":userId", userId);
        insertQuery.bindValue(":genreId", genreId);
        if (!insertQuery.exec()) {
            qWarning() << "خطا در ثبت ژانر مورد علاقه:" << insertQuery.lastError().text();
            return false;
        }
    }
    return true;
}
QVector<int> UserRepository::getPurchasedBookIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT BookID FROM UserLibrary WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
QVector<int> UserRepository::getSavedBookIds(int userId) {
    QVector<int> ids;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT BookID FROM SavedBooks WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            ids.append(query.value(0).toInt());
    }
    return ids;
}
bool UserRepository::addSavedBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("INSERT INTO SavedBooks (UserID, BookID) VALUES (:userId, :bookId)");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در ذخیره کتاب:" << query.lastError().text();
        return false;
    }
    return true;
}
bool UserRepository::removeSavedBook(int userId, int bookId) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("DELETE FROM SavedBooks WHERE UserID = :userId AND BookID = :bookId");
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    if (!query.exec()) {
        qWarning() << "خطا در حذف کتاب ذخیره شده:" << query.lastError().text();
        return false;
    }
    return true;
}
QMap<int, int> UserRepository::getReadingProgress(int userId) {
    QMap<int, int> progress;
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT BookID, LastPage FROM ReadingProgress WHERE UserID = :userId");
    query.bindValue(":userId", userId);
    if (query.exec()) {
        while (query.next())
            progress[query.value(0).toInt()] = query.value(1).toInt();
    }
    return progress;
}
bool UserRepository::setLastReadPage(int userId, int bookId, int pageNumber) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare(
        "MERGE INTO ReadingProgress AS target "
        "USING (SELECT :userId AS UserID, :bookId AS BookID) AS src "
        "ON target.UserID = src.UserID AND target.BookID = src.BookID "
        "WHEN MATCHED THEN UPDATE SET LastPage = :page "
        "WHEN NOT MATCHED THEN INSERT (UserID, BookID, LastPage) VALUES (:userId, :bookId, :page);"
        );
    query.bindValue(":userId", userId);
    query.bindValue(":bookId", bookId);
    query.bindValue(":page", pageNumber);
    if (!query.exec()) {
        qWarning() << "خطا در ذخیره پیشرفت مطالعه:" << query.lastError().text();
        return false;
    }
    return true;
}
bool UserRepository::findRoleById(int userId, UserRole &outRole) {
    QSqlDatabase db = DatabaseManager::getInstance()->getConnection();
    QSqlQuery query(db);
    query.prepare("SELECT RoleID FROM Users WHERE UserID = :userId AND IsDeleted = 0");
    query.bindValue(":userId", userId);
    if (!query.exec() || !query.next()) { return false; }
    int roleId = query.value(0).toInt();
    outRole = static_cast<UserRole>(roleId - 1);
    return true;
}