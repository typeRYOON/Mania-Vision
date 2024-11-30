#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "../utils/utils.h"

class TitleBar : public QF {
    Q_OBJECT
public:
    explicit TitleBar( Utils*, QWidget* = nullptr );

private:
    Utils* utils;

signals:
};

#endif // TITLEBAR_H :: - -                                                            - -
