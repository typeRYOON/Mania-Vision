#ifndef SELECTBOX_H
#define SELECTBOX_H

#include "../utils/utils.h"
#include <QWidget>

class SelectBox : public QF {
    Q_OBJECT
public:
    explicit SelectBox( Utils*, QW* = nullptr );

private:
    Utils* utils;
signals:
};

#endif // SELECTBOX_H
