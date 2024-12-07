#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "../utils/utils.h"
#include <QTimer>
#include <QSoundEffect>

class TitleBar : public QF {
    Q_OBJECT
public:
    explicit TitleBar( Utils*, QWidget* = nullptr );

public slots:
    void video_started( void );
    void video_ended( void );

private:
    Utils*  utils;

    // Timing
    quint16 prog_elapsed = 0;
    quint16 run_elapsed  = 0;
    QTimer* prog_timer;
    QTimer* run_timer;
    QL*     prog_time;
    QL*     run_time;

    //
    QF*  slider   = nullptr;
    QPA* slider_a = nullptr;
    QPB* pressed  = nullptr;
    QRect origin  = QRect( 8, 14, 25, 4 );
    QMap<QPB*, QPair<quint8, QS>> button_map;

    //
    bool          sound_on = true;
    QSoundEffect* se       = nullptr;


    // funcs
    QF*     make_slide_box( void );
    QPixmap recolor_pixmap( const QPixmap&, const QColor );

private slots:
    void handle_slider_press( QPB* );

signals:
    void opt_change( const quint8& );
    void minimize_sig( void );
    void close_sig( void );
    void add_log( const quint16&, CQS& );
};

#endif // TITLEBAR_H :: - -                                                            - -
