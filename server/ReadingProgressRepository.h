#ifndef READINGPROGRESSREPOSITORY_H
#define READINGPROGRESSREPOSITORY_H

class ReadingProgressRepository {
public:
    ReadingProgressRepository();
    bool upsertProgress(int userId, int bookId, int lastPage);
    int getLastPage(int userId, int bookId);
};

#endif // READINGPROGRESSREPOSITORY_H
