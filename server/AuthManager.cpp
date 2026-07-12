#include "AuthManager.h"
#include "UserRepository.h"
#include "PublisherRepository.h"
#include "AdminRepository.h"
#include "../common/user.h"
#include "../common/normaluser.h"
#include "../common/publisher.h"
#include "../common/Admin.h"
#include <QRegularExpression>
#include <memory>
#include <exception>
AuthManager::AuthManager() {}
Response AuthManager::validateNormalUserRegistration(const QString &username, const QString &plainPassword, const QString &plainAnswer) const {
    const QString cleanUsername = username.trimmed();
    const QString cleanAnswer = plainAnswer.trimmed();
    if (!User::isValidUsername(cleanUsername)) {
        return Response(ResponseStatus::ValidationFailed, "نام کاربری نامعتبر است. طول مجاز ۳ تا ۲۰ کاراکتر و فقط حروف انگلیسی، اعداد و خط تیره");
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
        return Response(ResponseStatus::ValidationFailed, "نام کاربری نامعتبر است. طول مجاز ۳ تا ۲۰ کاراکتر و فقط حروف انگلیسی، اعداد و خط تیره");
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
