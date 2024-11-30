#ifndef CONSOLEBOX_H
#define CONSOLEBOX_H

#include "../utils/utils.h"
#include <QWidget>

class ConsoleBox : public QF {
    Q_OBJECT

public:
    explicit ConsoleBox( Utils* u, QW* = nullptr );

private:
    Utils* utils;

signals:
};

#endif // CONSOLEBOX_H
