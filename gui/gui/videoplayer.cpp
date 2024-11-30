#include "videoplayer.h"
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsVideoItem>

//
VideoPlayer::VideoPlayer( Utils* u, QW* p ) : utils( u ), QW{ p }
{
    setFixedSize( 990, 564 );
    audio_output = new QAudioOutput( this );
    media_player = new QMediaPlayer( this );
    //video_widget = new QVideoWidget( this );
    //video_widget->setFixedSize( 970, 546 );

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


    audio_output->setVolume( 0.5 );

    media_player->setVideoOutput( video_item );
    media_player->setAudioOutput( audio_output );

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
    gradient->setPixmap( QPixmap( "res/gradient.png" ) );
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

    layout->addWidget( bot_line_qw  );
    layout->addWidget( ks_notch_qw  );
    layout->addWidget( gradient_qw );
    layout->addWidget( view );

    // maybe: kps legend
    // kps graph

    // off image

    // bottom line + key strokes

    // key strokes circle frame

    // Connections :
    connect( media_player, &QMediaPlayer::mediaStatusChanged, this, &VideoPlayer::handle_media_status );
    connect( media_player, &QMediaPlayer::errorOccurred, this, &VideoPlayer::handle_error );

    // VideoWidget startup render fix :
    //video_widget->hide();

    //QTimer::singleShot( 1000, this, [=]( void ){
    //    video_widget->show();
    //} );
}

//
void VideoPlayer::play_video( CQS& video_path )
{
    media_player->setSource( QUrl::fromLocalFile( video_path ) );
    media_player->play();
}

//
void VideoPlayer::stop_video( void )
{
    if ( !media_player->isPlaying() ) { return; }
    media_player->stop();
    media_player->setSource( QUrl( nullptr ) );
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
