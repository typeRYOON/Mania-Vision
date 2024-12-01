#include "videoplayer.h"
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

//
VideoPlayer::VideoPlayer( Utils* u, QW* p ) : utils( u ), QW{ p }
{
    setFixedSize( 990, 564 );
    //audio_output = new QAudioOutput( this );
    media_player = new QMediaPlayer( this );

    QGraphicsScene* scene = new QGraphicsScene( this );
    QGraphicsView*  view = new QGraphicsView( scene, this );
    view->setAttribute( Qt::WA_TransparentForMouseEvents, true );
    view->setStyleSheet( "background: transparent;" );
    view->setFixedSize( 970, 546 );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setFrameStyle( QFrame::NoFrame );

    QGraphicsVideoItem* video_item = new QGraphicsVideoItem();
    video_item->setAspectRatioMode( Qt::KeepAspectRatioByExpanding );
    video_item->setSize( QSizeF( 970, 546 ) );
    scene->addItem( video_item );

    //audio_output->setVolume( 0.5 );

    media_player->setVideoOutput( video_item );
    //media_player->setAudioOutput( audio_output );

    QSL* layout = new QSL( this );
    layout->setStackingMode( QStackedLayout::StackAll );
    layout->setAlignment( Qt::AlignTop );

    // videoplayer


    // gradient
    QL* gradient = new QL();
    gradient->setFixedSize( 970, 250 );
    gradient->setStyleSheet(
        "background-color: transparent;"
    );
    gradient->setPixmap( QPixmap( "assets/misc/video-gradient.png" ) );
    QW*  gradient_qw = new QW();
    QVL* gradient_ql = new QVL( gradient_qw );
    gradient_ql->setContentsMargins( 0, 296, 0, 0 );
    gradient_ql->addWidget( gradient );

    QF* bot_line = new QF();
    bot_line->setFixedSize( 970, 2 );
    bot_line->setStyleSheet(
        "background-color: #b5d2ff; border: 0px;" );
    QW*  bot_line_qw = new QW();
    QVL* bot_line_ql = new QVL( bot_line_qw );
    bot_line_ql->setContentsMargins( 0, 539, 0, 0 );
    bot_line_ql->addWidget( bot_line );

    //
    QL* inactive_ql = new QL();
    inactive_ql->setFixedSize( 970, 546 );
    inactive_ql->setPixmap( QPixmap( "assets/misc/inactive.png" ) );

    QOE* inactive_e = new QOE( inactive_ql );
    inactive_e->setOpacity( 1 );
    inactive_ql->setGraphicsEffect( inactive_e );
    inactive_a = utils->make_animation(
        1000, 1, 0, QEasingCurve::InOutCubic, "opacity", inactive_e, inactive_ql );

    fade_timer = new QTimer( this );
    fade_timer->setSingleShot( true );
    connect( fade_timer, &QTimer::timeout, this, [this]()
    {
        active = false;
        inactive_a->setStartValue( 0 );
        inactive_a->setStartValue( 1 );
        inactive_a->start();
    } );

    connect( inactive_a, &QAbstractAnimation::finished, this, [this]()
    {
        // Fade out animation :
        if ( active ) { return; }

        // Fade in animation :
        if ( media_player->isPlaying() ) {
            media_player->stop();
        }
        media_player->setSource( QUrl( ) );
    } );

    //
    QF* ks_notch = new QF();
    ks_notch->setFixedSize( 100, 10 );
    ks_notch->setStyleSheet(
        "background-color: #b5d2ff; border: 0px;"
        "border-radius: 4px;"
    );
    QW*  ks_notch_qw = new QW();
    QVL* ks_notch_ql = new QVL( ks_notch_qw );
    ks_notch_ql->setContentsMargins( 840, 539, 0, 0 );
    ks_notch_ql->addWidget( ks_notch );

    // Front
    layout->addWidget( bot_line_qw  );
    layout->addWidget( ks_notch_qw  );
    layout->addWidget( inactive_ql );
    layout->addWidget( gradient_qw );
    layout->addWidget( view );
    // Back

    // key strokes

    // Connections :
    connect( media_player, &QMediaPlayer::mediaStatusChanged, this, &VideoPlayer::handle_media_status );
    connect( media_player, &QMediaPlayer::errorOccurred, this, &VideoPlayer::handle_error );
}

//
void VideoPlayer::play_video( CQS& video_path )
{
    active = true;
    inactive_a->setStartValue( 1 );
    inactive_a->setEndValue( 0 );
    inactive_a->start();

    media_player->setSource( QUrl::fromLocalFile( video_path ) );
    media_player->play();

    QTimer::singleShot( 100, this, [this](){
        fade_timer->start( qMax( media_player->duration() - 1300, 1000 ) );
    } );
}

//
void VideoPlayer::stop_video( void )
{
    // Already fading out / stopping video :
    if ( !active ) { return; }

    active = false;
    inactive_a->setStartValue( 0 );
    inactive_a->setEndValue( 1 );
    inactive_a->start();
}

void VideoPlayer::handle_media_status( QMediaPlayer::MediaStatus status ) {
    if ( status == QMediaPlayer::EndOfMedia ) { stop_video(); }
}

//
void VideoPlayer::handle_error( QMediaPlayer::Error err )
{
    if ( err != QMediaPlayer::NoError ) {
        qWarning() << media_player->errorString();
        stop_video();
    }
}

// End. :: - -                                                                         - -
