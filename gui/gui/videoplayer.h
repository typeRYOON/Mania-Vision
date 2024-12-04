#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "../utils/utils.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QGraphicsVideoItem>

class VideoPlayer : public QW {
    Q_OBJECT

public:
    explicit VideoPlayer( Utils*, QW* = nullptr );

public slots:
    void play_video( CQS& );
    void stop_video( void );

private:
    Utils* utils;

    //
    QAudioOutput* audio_output;
    QMediaPlayer* media_player;
    QVideoWidget* video_widget;

    //
    QTimer* fade_timer = nullptr;
    QPA* inactive_a    = nullptr;
    bool active        = false;

private slots:
    void handle_media_status( QMediaPlayer::MediaStatus );
    void handle_error( QMediaPlayer::Error );

signals:
    void stop_sig( void );
    void start_sig( void );
};

#endif // VIDEOPLAYER_H
