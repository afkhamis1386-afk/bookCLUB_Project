#ifndef WINDOWNAV_H
#define WINDOWNAV_H

#include <QWidget>
inline void showFollowingState(QWidget *next, const QWidget *previous) {
    if(previous && previous->isMaximized())
        next->showMaximized();
    else
        next->show();
}

#endif // WINDOWNAV_H
