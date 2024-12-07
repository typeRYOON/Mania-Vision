#ifndef MANIAPLAYER_H
#define MANIAPLAYER_H

#include "shorthand.hpp"
#include <QObject>

class ManiaPlayer : public QO {
    Q_OBJECT

public:
    explicit ManiaPlayer( const QVector<quint8>&, CQS&, QO* = nullptr );

public slots:
    void runner( void );
    void stop_runner( void );

private:
    QS mfile;
    bool cont = true;
    quint8 key_n = 0;

    void read_mfile( void );

    QVector<QVector<QPair<quint32, quint32>>> hit_chart;
    QVector<quint8> vks;
    qint32 set_start_delays( void );

signals:
    void finished_mania( void );
    void delay( const qint16& );
};

#endif // MANIAPLAYER_H
