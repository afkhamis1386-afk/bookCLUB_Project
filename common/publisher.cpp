#ifndef PUBLISHER_CPP
#include "publisher.h"
using namespace std;
#include <stdexcept>
#include <algorithm>
#define PUBLISHER_CPP
bool Publisher::isValidName(const QString& name) const{
    static const QRegularExpression nameRegex(R"(^[a-zA-Z\s\x{0600}-\x{06FF}]+$)");
    return !name.trimmed().isEmpty() && nameRegex.match(name).hasMatch();
}
Publisher::Publisher()
    : User(),
    firstName(""),
    lastName(""),
    publicationName(""),
    email(""),
    description(""),
    licenseNumber("") {}
Publisher::Publisher(const QString& username,
                     const QString& password,
                     const QString& securityAnswer,
                     const QString& firstName,
                     const QString& lastName,
                     const QString& publicationName,
                     const QString& email,
                     const QString& description,
                     const QString& licenseNumber)
    : User(username, password, securityAnswer){
    setFirstName(firstName);
    setLastName(lastName);
    setPublicationName(publicationName);
    setEmail(email);
    setDescription(description);
    setLicenseNumber(licenseNumber);
}
Publisher::~Publisher(){}
QString Publisher::getRole() const { return "Publisher"; }
QString Publisher::getFirstName() const { return firstName; }
void Publisher::setFirstName(const QString& fName){
    if (!isValidName(fName))
        throw invalid_argument("!نام نامعتبراست");
    firstName = fName.trimmed();
}
QString Publisher::getLastName() const { return lastName; }
void Publisher::setLastName(const QString& lName){
    if (!isValidName(lName))
        throw invalid_argument("!نام خانوادگی نامعتبراست");
    lastName = lName.trimmed();
}
QString Publisher::getPublicationName() const { return publicationName; }
void Publisher::setPublicationName(const QString& name){
    if (!isValidName(name))
        throw invalid_argument("!نام انتشارات نامعتبراست");
    publicationName = name.trimmed();
}
QString Publisher::getEmail() const { return email; }
void Publisher::setEmail(const QString& emailValue) { email = emailValue; }
QString Publisher::getDescription() const { return description; }
void Publisher::setDescription(const QString& desc) { description = desc; }
QString Publisher::getLicenseNumber() const { return licenseNumber; }
void Publisher::setLicenseNumber(const QString& license){
    QString cleanLicense = license.trimmed();
    if (cleanLicense.isEmpty()){
        throw invalid_argument("!شماره پروانه نشر نمی‌تواند خالی باشد");
    }
    QRegularExpression regex(R"(^\d{10}$)");
    if (!regex.match(cleanLicense).hasMatch()){
        throw invalid_argument("!شماره پروانه نشر باید دقیقاً یک عدد ۱۰ رقمی باشد");
    }
    licenseNumber = cleanLicense;
}
void Publisher::addBook(const shared_ptr<Book>& book){
    if (!book)
        throw invalid_argument("!کتاب نامعتبر است");
    if (findBook(book->get_BookId()))
        throw runtime_error("!این کتاب قبلاً اضافه شده است");
    publishedBooks.push_back(book);
}
void Publisher::deactivateBook(int bookId){
    for (auto& book : publishedBooks){
        if (book && book->get_BookId() == bookId){
            book->setIsActive(false);
            return;
        }
    }
    throw runtime_error("!کتاب یافت نشد");
}
void Publisher::activateBook(int bookId){
    for (auto& book : publishedBooks){
        if (book && book->get_BookId() == bookId){
            book->setIsActive(true);
            return;
        }
    }
     throw runtime_error("!کتاب یافت نشد");
}
const QVector<shared_ptr<Book>>& Publisher::getPublishedBooks() const{ return publishedBooks; }
int Publisher::getPublishedBooksCount() const{ return publishedBooks.size(); }
double Publisher::getTotalRevenue() const{
    double total = 0;
    for (const auto& book : publishedBooks) {
        if (book) {
            total += book->getPrice() * book->getSoldCopies();
        }
    }
    return total;
}
int Publisher::getTotalSoldCopies() const{
    int total = 0;
    for (const auto& book : publishedBooks){
        if (book){
            total += book->getSoldCopies();
        }
    }
    return total;
}
QVector<shared_ptr<Book>> Publisher::getTopSellingBooks(int limit) const{
    QVector<shared_ptr<Book>> books;
    for (const auto& book : publishedBooks){
        if (book && book->getIsActive()){
            books.push_back(book);
        }
    }
    sort(books.begin(), books.end(),
              [](const shared_ptr<Book>& a, const shared_ptr<Book>& b){
                  return a->getSoldCopies() > b->getSoldCopies();
              });
    if (books.size() > limit)
        books.resize(limit);
    return books;
}
QVector<shared_ptr<Book>> Publisher::getLeastSellingBooks(int limit) const{
    QVector<shared_ptr<Book>> books;
    for (const auto& book : publishedBooks){
        if (book && book->getIsActive()){
            books.push_back(book);
        }
    }
    sort(books.begin(), books.end(),
              [](const shared_ptr<Book>& a, const shared_ptr<Book>& b){
                  return a->getSoldCopies() < b->getSoldCopies();
              });
    if (books.size() > limit)
        books.resize(limit);
    return books;
}
double Publisher::calculateAverageBooksRating() const{
    if (publishedBooks.isEmpty())
        return 0;
    double sum = 0;
    int validBooks = 0;
    for (const auto& book : publishedBooks){
        if (book && book->getIsActive()){
            sum += book->getAverageRating();
            validBooks++;
        }
    }
    if (validBooks == 0)
        return 0;
    return sum / validBooks;
}
shared_ptr<Book> Publisher::findBook(int bookId) const {
    for (const auto& book : publishedBooks){
        if (book && book->get_BookId() == bookId){
            return book;
        }
    }
    return nullptr;
}
void Publisher::updateBook(int bookId,
                           const QString& title,
                           const QString& author,
                           const QString& genre,
                           const QString& description,
                           double price,
                           double discount){
    auto book = findBook(bookId);
    if (!book)
        throw runtime_error("!کتاب مورد نظر یافت نشد");
    book->setTitle(title);
    book->setAuthor(author);
    book->setGenre(genre);
    book->setDescription(description);
    book->setPrice(price);
    book->setDiscount(discount);
}
QDataStream &operator<<(QDataStream &out, const Publisher &p){
    out << static_cast<const User&>(p);
    out << p.firstName
        << p.lastName
        << p.publicationName
        << p.email
        << p.description
        << p.licenseNumber;
    out << p.publishedBooks.size();
    for (const auto &bk : p.publishedBooks) {
        out << (bk ? bk->get_BookId() : -1);
    }
    return out;
}
QDataStream &operator>>(QDataStream &in, Publisher &p){
    in >> static_cast<User&>(p);
    in >> p.firstName
        >> p.lastName
        >> p.publicationName
        >> p.email
        >> p.description
        >> p.licenseNumber;
    int n = 0;
    in >> n;
    p.publishedBooks.clear();
    for (int i = 0; i < n; ++i) {
        int id;
        in >> id;
        p.publishedBooks.push_back(nullptr);
    }
    return in;
}

#endif // PUBLISHER_CPP
