#include "AuthManager.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "AdminRepository.h"
#include "../common/user.h"
#include "../common/normaluser.h"
#include "../common/publisher.h"
#include "../common/Admin.h"
#include "GenreRepository.h"
#include "CategoryRepository.h"
#include "../common/Genre.h"
#include "../common/Category.h"
#include <QRegularExpression>
#include <memory>
#include <exception>
AuthManager::AuthManager() {}
Response AuthManager::validateNormalUserRegistration(const QString &username, const QString &plainPassword, const QString &plainAnswer) const {
    const QString cleanUsername = username.trimmed();
    const QString cleanAnswer = plainAnswer.trimmed();
    if (!User::isValidUsername(cleanUsername)) {
        return Response(ResponseStatus::ValidationFailed, "نام کاربری نامعتبر است. طول مجاز ۳ تا ۱۵ کاراکتر و فقط حروف انگلیسی، اعداد، خط تیره و زیرخط");
    }
    if (!User::isStrongPassword(plainPassword)) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور ضعیف است. باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
    }
    if (cleanAnswer.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "پاسخ سوال امنیتی نمی تواند خالی باشد");
    }
    return Response(ResponseStatus::Success, "");
}
Response AuthManager::validatePublisherRegistration(const QString &username, const QString &plainPassword, const QString &plainAnswer, const QString &firstName,
                                                    const QString &lastName, const QString &email, const QString &publicationName,
                                                    const QString &licenseNumber) const {
    const QString cleanUsername = username.trimmed();
    const QString cleanAnswer = plainAnswer.trimmed();
    const QString cleanFirstName = firstName.trimmed();
    const QString cleanLastName = lastName.trimmed();
    const QString cleanEmail = email.trimmed();
    const QString cleanPublicationName = publicationName.trimmed();
    const QString cleanLicenseNumber = licenseNumber.trimmed();
    if (!User::isValidUsername(cleanUsername)) {
        return Response(ResponseStatus::ValidationFailed, "نام کاربری نامعتبر است. طول مجاز ۳ تا ۱۵ کاراکتر و فقط حروف انگلیسی، اعداد، خط تیره و زیرخط");
    }
    if (!User::isStrongPassword(plainPassword)) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور ضعیف است. باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
    }
    if (cleanAnswer.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "پاسخ سوال امنیتی نمی تواند خالی باشد");
    }
    if (cleanFirstName.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "نام نمی تواند خالی باشد");
    }
    if (cleanLastName.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "نام خانوادگی نمی تواند خالی باشد");
    }
    if (cleanEmail.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "ایمیل نمی تواند خالی باشد");
    }
    if (!isValidEmail(cleanEmail)) {
        return Response(ResponseStatus::ValidationFailed, "فرمت ایمیل نامعتبر است");
    }
    if (cleanPublicationName.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "نام انتشارات نمی تواند خالی باشد");
    }
    if (cleanLicenseNumber.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "شماره پروانه نشر نمی تواند خالی باشد");
    }
    return Response(ResponseStatus::Success, "");
}
Response AuthManager::validatePasswordChangeInput(const QString &oldPassword, const QString &newPassword) const {
    if (oldPassword.isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور فعلی را وارد کنید");
    }
    if (!User::isStrongPassword(newPassword)) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور جدید ضعیف است. باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
    }
    if (oldPassword == newPassword) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور جدید نباید با رمز عبور فعلی یکسان باشد");
    }
    return Response(ResponseStatus::Success, "");
}
Response AuthManager::validateRecoverPasswordInput(const QString &username, const QString &securityAnswer, const QString &newPassword) const {
    if (username.trimmed().isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "نام کاربری را وارد کنید");
    }
    if (securityAnswer.trimmed().isEmpty()) {
        return Response(ResponseStatus::ValidationFailed, "پاسخ سوال امنیتی نمی تواند خالی باشد");
    }
    if (!User::isStrongPassword(newPassword)) {
        return Response(ResponseStatus::ValidationFailed, "رمز عبور جدید ضعیف است. باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
    }
    return Response(ResponseStatus::Success, "");
}
bool AuthManager::isValidEmail(const QString &email) const {
    static const QRegularExpression emailRegex( R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)" );
    return emailRegex.match(email.trimmed()).hasMatch();
}
Response AuthManager::buildLoginSuccessResponse(int userId, const QString &username, UserRole role, const QString &publicationName) const {
    QVariantMap data;
    data["userId"] = userId;
    data["username"] = username;
    data["role"] = static_cast<int>(role);
    if (role == UserRole::Publisher) {
        data["publicationName"] = publicationName;
    }
    if (role == UserRole::NormalUser) {
        UserRepository userRepo;
        data["hasFavoriteGenres"] = !userRepo.getFavoriteGenreIds(userId).isEmpty();
    }
    return Response(ResponseStatus::Success, "ورود موفقیت آمیز بود", data);
}
Response AuthManager::registerNormalUser(const QString &username, const QString &plainPassword, const QString &plainAnswer) {
    try {
        Response validation = validateNormalUserRegistration(username, plainPassword, plainAnswer);
        if (validation.getStatus() != ResponseStatus::Success) {
            return validation;
        }
        const QString cleanUsername = username.trimmed();
        const QString cleanAnswer = plainAnswer.trimmed();
        const QString encryptedUsername = User::encryptString(cleanUsername);
        UserRepository userRepo;
        int existingUserId = -1;
        UserRole existingRole = UserRole::NormalUser;
        if (userRepo.findUserRoleAndId(encryptedUsername, existingUserId, existingRole)) {
            return Response(ResponseStatus::Error, "این نام کاربری قبلاً استفاده شده است");
        }
        NormalUser newUser(cleanUsername, plainPassword, cleanAnswer);
        const int newUserId = userRepo.insertNormalUser(newUser);
        if (newUserId == -1) {
            return Response(ResponseStatus::Error, "خطا در ثبت نام. لطفاً دوباره تلاش کنید");
        }
        QVariantMap data;
        data["userId"] = newUserId;
        data["role"] = static_cast<int>(UserRole::NormalUser);
        return Response(ResponseStatus::Success, "ثبت نام با موفقیت انجام شد", data);
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error, "خطای داخلی در ثبت نام کاربر");
    }
    catch (...) {
        return Response(ResponseStatus::Error, "خطای ناشناخته در ثبت نام کاربر");
    }
}
Response AuthManager::registerPublisher(const QString &username,const QString &plainPassword, const QString &plainAnswer,const QString &firstName,
                                        const QString &lastName, const QString &email, const QString &publicationName, const QString &licenseNumber, const QString &shortDescription)
{
    try {
        Response validation = validatePublisherRegistration(username, plainPassword,plainAnswer, firstName, lastName, email, publicationName, licenseNumber);
        if (validation.getStatus() != ResponseStatus::Success) {
            return validation;
        }
        const QString cleanUsername = username.trimmed();
        const QString cleanAnswer = plainAnswer.trimmed();
        const QString cleanFirstName = firstName.trimmed();
        const QString cleanLastName = lastName.trimmed();
        const QString cleanEmail = email.trimmed();
        const QString cleanPublicationName = publicationName.trimmed();
        const QString cleanLicenseNumber = licenseNumber.trimmed();
        const QString cleanShortDescription = shortDescription.trimmed();
        const QString encryptedUsername = User::encryptString(cleanUsername);
        UserRepository userRepo;
        int existingUserId = -1;
        UserRole existingRole = UserRole::NormalUser;
        if (userRepo.findUserRoleAndId(encryptedUsername, existingUserId, existingRole)) {
            return Response(ResponseStatus::Error, "این نام کاربری قبلاً استفاده شده است");
        }
        PublisherRepository publisherRepo;
        if (publisherRepo.isEmailTaken(cleanEmail)) {
            return Response(ResponseStatus::Error, "این ایمیل قبلاً ثبت شده است");
        }
        if (publisherRepo.isLicenseNumberTaken(cleanLicenseNumber)) {
            return Response(ResponseStatus::Error, "این شماره پروانه نشر قبلاً ثبت شده است");
        }
        Publisher newPublisher(cleanUsername, plainPassword, cleanAnswer, cleanFirstName, cleanLastName, cleanEmail, cleanPublicationName, cleanLicenseNumber,cleanShortDescription);
        const int newUserId = publisherRepo.insertPublisher(newPublisher);
        if (newUserId == -1) {
            return Response(ResponseStatus::Error, "خطا در ثبت نام ناشر. لطفاً دوباره تلاش کنید");
        }
        QVariantMap data;
        data["userId"] = newUserId;
        data["role"] = static_cast<int>(UserRole::Publisher);

        return Response(ResponseStatus::Success, "ثبت نام ناشر با موفقیت انجام شد", data);
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error, "خطای داخلی در ثبت نام ناشر");
    }
    catch (...) {
        return Response(ResponseStatus::Error, "خطای ناشناخته در ثبت نام ناشر");
    }
}
Response AuthManager::login(const QString &username, const QString &plainPassword) {
    try {
        const QString cleanUsername = username.trimmed();
        if (cleanUsername.isEmpty() || plainPassword.isEmpty()) {
            return Response(ResponseStatus::ValidationFailed, "نام کاربری و رمز عبور را وارد کنید");
        }
        const QString encryptedUsername = User::encryptString(cleanUsername);
        UserRepository userRepo;
        int userId = -1;
        UserRole role = UserRole::NormalUser;
        if (!userRepo.findUserRoleAndId(encryptedUsername, userId, role)) {
            return Response(ResponseStatus::NotFound, "نام کاربری یا رمز عبور اشتباه است");
        }
        if (role == UserRole::NormalUser) {
            std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
            if (!user) {
                return Response(ResponseStatus::Error, "خطا در بارگذاری اطلاعات کاربر");
            }
            if (!user->verifyPassword(plainPassword)) {
                return Response(ResponseStatus::Unauthorized, "نام کاربری یا رمز عبور اشتباه است");
            }
            if (user->getIsBlocked()) {
                return Response(ResponseStatus::Unauthorized, "حساب کاربری شما مسدود شده است");
            }
            if (!user->getIsActive()) {
                return Response(ResponseStatus::Unauthorized, "حساب کاربری شما غیرفعال شده است");
            }
            return buildLoginSuccessResponse( user->getUserId(), user->getUsername(), UserRole::NormalUser);
        }
        if (role == UserRole::Publisher) {
            PublisherRepository publisherRepo;
            std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
            if (!publisher) {
                return Response(ResponseStatus::Error, "خطا در بارگذاری اطلاعات ناشر");
            }
            if (!publisher->verifyPassword(plainPassword)) {
                return Response(ResponseStatus::Unauthorized, "نام کاربری یا رمز عبور اشتباه است");
            }
            if (publisher->getIsBlocked()) {
                return Response(ResponseStatus::Unauthorized, "حساب کاربری شما مسدود شده است");
            }
            if (!publisher->getIsActive()) {
                return Response(ResponseStatus::Unauthorized, "حساب کاربری شما غیرفعال شده است");
            }
            return buildLoginSuccessResponse( publisher->getUserId(), publisher->getUsername(), UserRole::Publisher, publisher->getPublicationName());
        }
        AdminRepository adminRepo;
        std::unique_ptr<Admin> admin(adminRepo.loadAdminById(userId));
        if (!admin) {
            return Response(ResponseStatus::Error, "خطا در بارگذاری اطلاعات ادمین");
        }
        if (!admin->verifyPassword(plainPassword)) {
            return Response(ResponseStatus::Unauthorized, "نام کاربری یا رمز عبور اشتباه است");
        }
        if (admin->getIsBlocked()) {
            return Response(ResponseStatus::Unauthorized, "حساب کاربری شما مسدود شده است");
        }

        return buildLoginSuccessResponse(admin->getUserId(), admin->getUsername(), UserRole::Admin);
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error, "خطای داخلی در ورود به سیستم");
    }
    catch (...) {
        return Response(ResponseStatus::Error, "خطای ناشناخته در ورود به سیستم");
    }
}
Response AuthManager::changePassword(int userId, UserRole role, const QString &oldPassword, const QString &newPassword){
    try {
        Response validation = validatePasswordChangeInput(oldPassword, newPassword);
        if (validation.getStatus() != ResponseStatus::Success) {
            return validation;
        }
        UserRepository userRepo;
        if (role == UserRole::NormalUser) {
            std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
            if (!user) {
                return Response(ResponseStatus::NotFound,"کاربر یافت نشد");
            }
            if (user->getIsBlocked()) {
                return Response(ResponseStatus::Unauthorized,"حساب کاربری مسدود است");
            }
            if (!user->changePassword(oldPassword, newPassword)) {
                return Response(ResponseStatus::Unauthorized,"رمز عبور فعلی اشتباه است");
            }
            const bool updated = userRepo.updatePasswordHash(userId, user->getPasswordHash());
            if (!updated) {
                return Response(ResponseStatus::Error, "خطا در به روزرسانی رمز عبور");
            }
            return Response(ResponseStatus::Success,"رمز عبور با موفقیت تغییر یافت");
        }
        if (role == UserRole::Publisher) {
            PublisherRepository publisherRepo;
            std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
            if (!publisher) {
                return Response(ResponseStatus::NotFound,"ناشر یافت نشد");
            }
            if (publisher->getIsBlocked()) {
                return Response(ResponseStatus::Unauthorized,"حساب کاربری مسدود است");
            }
            if (!publisher->changePassword(oldPassword, newPassword)) {
                return Response(ResponseStatus::Unauthorized,"رمز عبور فعلی اشتباه است");
            }
            const bool updated = userRepo.updatePasswordHash(userId, publisher->getPasswordHash());
            if (!updated) {
                return Response(ResponseStatus::Error,"خطا در به روزرسانی رمز عبور");
            }
            return Response(ResponseStatus::Success,"رمز عبور با موفقیت تغییر یافت");
        }
        AdminRepository adminRepo;
        std::unique_ptr<Admin> admin(adminRepo.loadAdminById(userId));
        if (!admin) {
            return Response(ResponseStatus::NotFound,"ادمین یافت نشد");
        }
        if (admin->getIsBlocked()) {
            return Response(ResponseStatus::Unauthorized,"حساب کاربری مسدود است");
        }
        if (!admin->changePassword(oldPassword, newPassword)) {
            return Response(ResponseStatus::Unauthorized,"رمز عبور فعلی اشتباه است");
        }
        const bool updated = userRepo.updatePasswordHash(userId, admin->getPasswordHash());
        if (!updated) {
            return Response(ResponseStatus::Error,"خطا در به روزرسانی رمز عبور");
        }

        return Response(ResponseStatus::Success,"رمز عبور با موفقیت تغییر یافت");
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error,"خطای داخلی در تغییر رمز عبور");
    }
    catch (...) {
        return Response(ResponseStatus::Error,"خطای ناشناخته در تغییر رمز عبور");
    }
}
Response AuthManager::recoverPassword(const QString &username, const QString &securityAnswer, const QString &newPassword) {
    try {
        Response validation = validateRecoverPasswordInput(username, securityAnswer, newPassword);
        if (validation.getStatus() != ResponseStatus::Success) {
            return validation;
        }
        const QString cleanUsername = username.trimmed();
        const QString cleanAnswer = securityAnswer.trimmed();
        const QString encryptedUsername = User::encryptString(cleanUsername);
        UserRepository userRepo;
        int userId = -1;
        UserRole role = UserRole::NormalUser;
        if (!userRepo.findUserRoleAndId(encryptedUsername, userId, role)) {
            return Response(ResponseStatus::NotFound,"اطلاعات وارد شده صحیح نیست");
        }
        if (role == UserRole::Admin) {
            return Response(ResponseStatus::Error,"بازیابی رمز برای حساب ادمین از این طریق ممکن نیست");
        }
        if (role == UserRole::NormalUser) {
            std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
            if (!user) {
                return Response(ResponseStatus::Error,"خطا در بارگذاری اطلاعات کاربر");
            }
            if (user->getIsBlocked()) {
                return Response(ResponseStatus::Unauthorized,"حساب کاربری مسدود است");
            }
            if (!user->getIsActive()) {
                return Response(ResponseStatus::Unauthorized, "حساب کاربری شما غیرفعال شده است");
            }
            if (!user->recoverPassword(cleanAnswer, newPassword)) {
                return Response(ResponseStatus::Unauthorized, "اطلاعات وارد شده صحیح نیست");
            }
            const bool updated = userRepo.updatePasswordHash(userId, user->getPasswordHash());
            if (!updated) {
                return Response(ResponseStatus::Error,"خطا در به روزرسانی رمز عبور");
            }
            return Response(ResponseStatus::Success,"رمز عبور با موفقیت بازیابی شد");
        }
        PublisherRepository publisherRepo;
        std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
        if (!publisher) {
            return Response(ResponseStatus::Error,"خطا در بارگذاری اطلاعات ناشر");
        }
        if (publisher->getIsBlocked()) {
            return Response(ResponseStatus::Unauthorized,"حساب کاربری مسدود است");
        }
        if (!publisher->getIsActive()) {
            return Response(ResponseStatus::Unauthorized, "حساب کاربری شما غیرفعال شده است");
        }
        if (!publisher->recoverPassword(cleanAnswer, newPassword)) {
            return Response(ResponseStatus::Unauthorized, "اطلاعات وارد شده صحیح نیست");
        }
        const bool updated = userRepo.updatePasswordHash(userId, publisher->getPasswordHash());
        if (!updated) {
            return Response(ResponseStatus::Error,"خطا در به روزرسانی رمز عبور");
        }
        return Response(ResponseStatus::Success,"رمز عبور با موفقیت بازیابی شد");
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error,"خطای داخلی در بازیابی رمز عبور");
    }
    catch (...) {
        return Response(ResponseStatus::Error,"خطای ناشناخته در بازیابی رمز عبور");
    }
}
Response AuthManager::getAllGenres() {
    try {
        GenreRepository genreRepo;
        QVector<Genre> genres = genreRepo.getAllGenres();
        QVariantList list;
        for (const Genre &g : qAsConst(genres)) {
            QVariantMap item;
            item["genreId"] = g.getGenreId();
            item["genreTitle"] = g.getGenreTitle();
            list.append(item);
        }
        QVariantMap data;
        data["genres"] = list;
        return Response(ResponseStatus::Success, "لیست ژانرها بازیابی شد", data);
    } catch (...) {
        return Response(ResponseStatus::Error, "خطا در دریافت لیست ژانرها");
    }
}
Response AuthManager::getAllCategories() {
    try {
        CategoryRepository categoryRepo;
        QVector<Category> categories = categoryRepo.getAllCategories();
        QVariantList list;
        for (const Category &c : qAsConst(categories)) {
            QVariantMap item;
            item["categoryId"] = c.getCategoryId();
            item["categoryTitle"] = c.getCategoryTitle();
            list.append(item);
        }
        QVariantMap data;
        data["categories"] = list;
        return Response(ResponseStatus::Success, "لیست دسته بندی ها بازیابی شد", data);
    } catch (...) {
        return Response(ResponseStatus::Error, "خطا در دریافت لیست دسته بندی ها");
    }
}
Response AuthManager::setFavoriteGenres(int userId, const QVector<int> &genreIds) {
    try {
        QVector<int> unique;
        for (int id : genreIds) {
            if (!unique.contains(id)) unique.append(id);
        }
        if (unique.size() < 1 || unique.size() > 3) {
            return Response(ResponseStatus::ValidationFailed, "باید بین ۱ تا ۳ ژانر انتخاب کنید");
        }
        GenreRepository genreRepo;
        QVector<Genre> allGenres = genreRepo.getAllGenres();
        for (int id : unique) {
            bool found = false;
            for (const Genre &g : qAsConst(allGenres)) {
                if (g.getGenreId() == id) { found = true; break; }
            }
            if (!found) {
                return Response(ResponseStatus::ValidationFailed, "یکی از ژانرهای انتخاب شده معتبر نیست");
            }
        }
        UserRepository userRepo;
        if (!userRepo.setFavoriteGenreIds(userId, unique)) {
            return Response(ResponseStatus::Error, "خطا در ذخیره ژانرهای مورد علاقه");
        }
        return Response(ResponseStatus::Success, "ژانرهای مورد علاقه با موفقیت ثبت شد");
    } catch (...) {
        return Response(ResponseStatus::Error, "خطای داخلی سیستم در ثبت ژانرهای مورد علاقه");
    }
}
Response AuthManager::getAccountInfo(int userId, UserRole role) {
    try {
        if (role == UserRole::NormalUser) {
            UserRepository userRepo;
            std::unique_ptr<NormalUser> user(userRepo.loadNormalUserById(userId));
            if (!user) {
                return Response(ResponseStatus::NotFound, "کاربر یافت نشد");
            }
            QVariantMap data;
            data["userId"] = user->getUserId();
            data["username"] = user->getUsername();
            data["registerDate"] = user->getRegisterDate();
            data["purchasedCount"] = user->getPurchasedCount();
            QVariantList genreList;
            for (int genreId : user->getFavoriteGenres())
                genreList.append(genreId);
            data["favoriteGenres"] = genreList;
            return Response(ResponseStatus::Success, "اطلاعات حساب بازیابی شد", data);
        }
        if (role == UserRole::Publisher) {
            PublisherRepository publisherRepo;
            std::unique_ptr<Publisher> publisher(publisherRepo.loadPublisherById(userId));
            if (!publisher) {
                return Response(ResponseStatus::NotFound, "ناشر یافت نشد");
            }
            QVariantMap data;
            data["userId"] = publisher->getUserId();
            data["username"] = publisher->getUsername();
            data["firstName"] = publisher->getFirstName();
            data["lastName"] = publisher->getLastName();
            data["email"] = publisher->getEmail();
            data["shortDescription"] = publisher->getShortDescription();
            data["publicationName"] = publisher->getPublicationName();
            data["publisherLicenseNumber"] = publisher->getPublisherLicenseNumber();
            data["registerDate"] = publisher->getRegisterDate();
            return Response(ResponseStatus::Success, "اطلاعات حساب بازیابی شد", data);
        }
        AdminRepository adminRepo;
        std::unique_ptr<Admin> admin(adminRepo.loadAdminById(userId));
        if (!admin) {
            return Response(ResponseStatus::NotFound, "ادمین یافت نشد");
        }
        QVariantMap data;
        data["userId"] = admin->getUserId();
        data["username"] = admin->getUsername();
        data["firstName"] = admin->getFirstName();
        data["lastName"] = admin->getLastName();
        data["registerDate"] = admin->getRegisterDate();
        return Response(ResponseStatus::Success, "اطلاعات حساب بازیابی شد", data);
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error, "خطای داخلی در بازیابی اطلاعات حساب");
    }
    catch (...) {
        return Response(ResponseStatus::Error, "خطای ناشناخته در بازیابی اطلاعات حساب");
    }
}

Response AuthManager::updateAccount(int userId, UserRole role, const QVariantMap &accountData) {
    try {
        if (role != UserRole::NormalUser && role != UserRole::Publisher) {
            return Response(ResponseStatus::Unauthorized, "ویرایش حساب برای این نقش مجاز نیست");
        }

        const QString username = accountData.value("username").toString().trimmed();
        const QString newPassword = accountData.value("password").toString();
        const QString securityAnswer = accountData.value("securityAnswer").toString().trimmed();

        if (!User::isValidUsername(username)) {
            return Response(ResponseStatus::ValidationFailed,
                            "نام کاربری نامعتبر است. طول مجاز ۳ تا ۱۵ کاراکتر و فقط حروف انگلیسی، اعداد، خط تیره و زیرخط است");
        }
        if (!newPassword.isEmpty() && !User::isStrongPassword(newPassword)) {
            return Response(ResponseStatus::ValidationFailed,
                            "رمز عبور جدید ضعیف است. باید حداقل ۸ کاراکتر و شامل حروف بزرگ، کوچک و عدد باشد");
        }

        const QString encryptedUsername = User::encryptString(username);
        const QString passwordHash = newPassword.isEmpty() ? QString() : User::hashString(newPassword);
        const QString securityAnswerHash = securityAnswer.isEmpty() ? QString() : User::hashString(securityAnswer);

        UserRepository userRepo;
        if (userRepo.isUsernameTakenByOther(encryptedUsername, userId)) {
            return Response(ResponseStatus::Error, "این نام کاربری قبلاً استفاده شده است");
        }

        if (role == UserRole::NormalUser) {
            std::unique_ptr<NormalUser> currentUser(userRepo.loadNormalUserById(userId));
            if (!currentUser) {
                return Response(ResponseStatus::NotFound, "کاربر یافت نشد");
            }
            if (!userRepo.updateNormalUserAccount(userId, encryptedUsername, passwordHash, securityAnswerHash)) {
                return Response(ResponseStatus::Error, "خطا در به روزرسانی حساب کاربری");
            }

            QVariantMap data;
            data["userId"] = userId;
            data["role"] = static_cast<int>(role);
            data["username"] = username;
            return Response(ResponseStatus::Success, "حساب کاربری با موفقیت ویرایش شد", data);
        }

        const QString firstName = accountData.value("firstName").toString().trimmed();
        const QString lastName = accountData.value("lastName").toString().trimmed();
        const QString email = accountData.value("email").toString().trimmed();
        const QString publicationName = accountData.value("publicationName").toString().trimmed();
        const QString licenseNumber = accountData.value("licenseNumber").toString().trimmed();
        const QString shortDescription = accountData.value("shortDescription").toString().trimmed();

        if (firstName.isEmpty() || firstName.length() > 30) {
            return Response(ResponseStatus::ValidationFailed, "نام باید بین ۱ تا ۳۰ کاراکتر باشد");
        }
        if (lastName.isEmpty() || lastName.length() > 30) {
            return Response(ResponseStatus::ValidationFailed, "نام خانوادگی باید بین ۱ تا ۳۰ کاراکتر باشد");
        }
        if (email.isEmpty() || email.length() > 120 || !isValidEmail(email)) {
            return Response(ResponseStatus::ValidationFailed, "ایمیل نامعتبر است");
        }
        if (publicationName.isEmpty() || publicationName.length() > 50) {
            return Response(ResponseStatus::ValidationFailed, "نام انتشارات باید بین ۱ تا ۵۰ کاراکتر باشد");
        }
        if (licenseNumber.isEmpty() || licenseNumber.length() > 13) {
            return Response(ResponseStatus::ValidationFailed, "شماره پروانه نشر باید حداکثر ۱۳ کاراکتر باشد");
        }
        if (shortDescription.length() > 500) {
            return Response(ResponseStatus::ValidationFailed, "توضیحات کوتاه باید حداکثر ۵۰۰ کاراکتر باشد");
        }

        PublisherRepository publisherRepo;
        std::unique_ptr<Publisher> currentPublisher(publisherRepo.loadPublisherById(userId));
        if (!currentPublisher) {
            return Response(ResponseStatus::NotFound, "ناشر یافت نشد");
        }
        if (publisherRepo.isEmailTaken(email, userId)) {
            return Response(ResponseStatus::Error, "این ایمیل قبلاً ثبت شده است");
        }
        if (publisherRepo.isLicenseNumberTaken(licenseNumber, userId)) {
            return Response(ResponseStatus::Error, "این شماره پروانه نشر قبلاً ثبت شده است");
        }

        if (!publisherRepo.updateAccount(userId, encryptedUsername, passwordHash, securityAnswerHash,
                                         firstName, lastName, email, shortDescription,
                                         publicationName, licenseNumber)) {
            return Response(ResponseStatus::Error, "خطا در به روزرسانی حساب ناشر");
        }

        QVariantMap data;
        data["userId"] = userId;
        data["role"] = static_cast<int>(role);
        data["username"] = username;
        data["firstName"] = firstName;
        data["lastName"] = lastName;
        data["email"] = email;
        data["shortDescription"] = shortDescription;
        data["publicationName"] = publicationName;
        data["publisherLicenseNumber"] = licenseNumber;
        return Response(ResponseStatus::Success, "حساب ناشر با موفقیت ویرایش شد", data);
    }
    catch (const std::exception &) {
        return Response(ResponseStatus::Error, "خطای داخلی در ویرایش حساب کاربری");
    }
    catch (...) {
        return Response(ResponseStatus::Error, "خطای ناشناخته در ویرایش حساب کاربری");
    }
}
