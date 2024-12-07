#ifndef KEYHANDLER_H
#define KEYHANDLER_H

#include "shorthand.hpp"
#include <QObject>

class KeyHandler : public QO {
    Q_OBJECT

public:
    explicit KeyHandler( QO* = nullptr );

public slots:
    void runner( void );
    void mania_change( const bool&, CQS& );

private:
    bool mania_ready = false;
    QS   mfile = "";

signals:
    void begin_play( CQS& );
    void end_play( void );
};

#endif // KEYHANDLER_H
