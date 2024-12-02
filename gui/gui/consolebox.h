#ifndef CONSOLEBOX_H
#define CONSOLEBOX_H

#include "../utils/utils.h"
#include <QWidget>

class ConsoleBox : public QF {
    Q_OBJECT

public:
    explicit ConsoleBox( Utils* u, QW* = nullptr );

public slots:
    void add_log( const quint32&, CQS& );

private:
    Utils* utils;

    //
    QList<QPair<QL*, QL*>> log_labels;

    // funcs :
    QW* make_history_log( void );
    QW* make_page_sl( void );

signals:
};

#endif // CONSOLEBOX_H
