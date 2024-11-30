#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "../utils/utils.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>

class VideoPlayer : public QW {
    Q_OBJECT

public:
    explicit VideoPlayer( Utils*, QW* = nullptr );

public slots:
    void play_video( CQS& );
    void stop_video( void );

private:
    Utils* utils;
    QAudioOutput* audio_output;
    QMediaPlayer* media_player;
    QVideoWidget* video_widget;

private slots:
    void handle_media_status( QMediaPlayer::MediaStatus );
    void handle_error( QMediaPlayer::Error );

signals:
};

#endif // VIDEOPLAYER_H
