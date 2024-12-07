#ifndef MANIAPLAYERKEY_H
#define MANIAPLAYERKEY_H

#include "shorthand.hpp"

class ManiaPlayerKey : public QO {
    Q_OBJECT

public:
    explicit ManiaPlayerKey(
        const QVector<QPair<quint32, quint32>>&,
        const quint32&,
        const quint8&,
        QO* = nullptr
    );
public slots:
    void delay( const qint16& );
    void stop_runner( void );
    void runner( void );

private:
    const QVector<QPair<quint32, quint32>>& hit_map;
    const quint32& start_delay;
    const quint8&  vk;
    qint16         shift = 0;
    bool           cont = true;
};



#endif // MANIAPLAYERKEY_H
