#include "../utils/keyhandler.h"
#include "videoplayer.h"
#include "mainwindow.h"
#include "consolebox.h"
#include "selectbox.h"
#include "titlebar.h"
#include "qevent.h"
#include <QSoundEffect>
#include <QTimer>

//
MainWindow::MainWindow( QWidget* p ) : QMainWindow( p )
{
    utils = new Utils();

    resize( 1340, 894 );
    setFixedSize( size() );
    setWindowIcon( QIcon( "assets/program/taskbar.png" ) );
    setWindowFlags( Qt::FramelessWindowHint );
    setAttribute( Qt::WA_TranslucentBackground );

    setCentralWidget( make_main_widget() );

    QTimer::singleShot( 500, this, [this]()
    {
        show();
        window_fade_to( 1.0, 400 );
    } );
}

void MainWindow::minimize( void )
{
    window_fade_to( 0.0, 250 );
    QTimer::singleShot( 500, this, &QW::showMinimized );
}

//
QF* MainWindow::make_main_widget()
{
    QF* main_widget = new QF();
    main_widget->setObjectName( "MainWidget" );
    main_widget->setContentsMargins( 0, 0, 0, 0 );
    main_widget->setFixedSize( 1340, 894 );
    main_widget->setStyleSheet(
        "#MainWidget { background-color: #060606;border-radius: 0px;"
        "border: 2px solid #b5d2ff; color: white }"
    );

    TitleBar*    title_bar    = new TitleBar( this->utils, this );
    VideoPlayer* video_player = new VideoPlayer( this->utils, this );

    ConsoleBox* console_box = new ConsoleBox( utils, this );
    SelectBox*  select_box  = new SelectBox( utils, this );

    // Main Layout :
    QVL* left_vl = new QVL();
    left_vl->setAlignment( Qt::AlignTop );
    video_player->setContentsMargins( 14, 12, 0, 0 );
    left_vl->setContentsMargins( 0, 22, 0, 0 );
    left_vl->addWidget( video_player );
    left_vl->addWidget( console_box );

    QHL* main_hl = new QHL();
    main_hl->setContentsMargins( 20, 0, 0, 0 );
    main_hl->setAlignment( Qt::AlignLeft );
    main_hl->setSpacing( 10 );
    main_hl->addLayout( left_vl );
    main_hl->addWidget( select_box );

    QVL* main_vl = new QVL( main_widget );
    main_vl->setContentsMargins( 0, 0, 0, 0 );
    main_vl->setAlignment( Qt::AlignTop );
    main_vl->setSpacing( 0 );

    main_vl->addWidget( title_bar );
    main_vl->addLayout( main_hl );

    // Connections :
    connect( title_bar, &TitleBar::minimize_sig, this, &MainWindow::minimize );
    connect( title_bar, &TitleBar::close_sig, this, [this]( void )
    {
        connect( window_fade_to( 0, 400 ), &QPA::finished, this, [this]( void ) {
            this->close();
        } );
    } );

    connect( this,       &MainWindow::add_log, console_box, &ConsoleBox::add_log );
    connect( title_bar,  &TitleBar  ::add_log, console_box, &ConsoleBox::add_log );
    connect( select_box, &SelectBox ::add_log, console_box, &ConsoleBox::add_log );

    connect( video_player, &VideoPlayer::start_sig, title_bar, &TitleBar::video_started );
    connect( video_player, &VideoPlayer::stop_sig,  title_bar, &TitleBar::video_ended );

    QThread*    key_handler_t = new QThread( this );
    KeyHandler* key_handler   = new KeyHandler( );

    connect( key_handler, &KeyHandler::begin_play, video_player, &VideoPlayer::play_video );
    connect( key_handler, &KeyHandler::end_play,   video_player, &VideoPlayer::stop_video );

    connect( select_box, &SelectBox::mania_change, key_handler,  &KeyHandler::mania_change,
            Qt::DirectConnection );
    connect( key_handler_t, &QThread::started, key_handler, &KeyHandler::runner );
    key_handler->moveToThread( key_handler_t );
    key_handler_t->start();

    return main_widget;
}

// Fade animation maker/starter :: - -                                                 - -
QPA* MainWindow::window_fade_to(
    const float& target, const quint16& duration, const QEasingCurve& curve )
{
    QPA* window_a = utils->make_animation(
        duration, windowOpacity(), target, curve, "windowOpacity", this, this
    );
    window_a->start( QAbstractAnimation::DeleteWhenStopped );
    return window_a;
}

//
void MainWindow::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton ) {
        drag_position = e->globalPosition().toPoint() - frameGeometry().topLeft();
        e->accept();
        window_fade_to( 0.5, 300 );
    }
}

// Overloaded mouse drag fade window function :: - -                                   - -
void MainWindow::mouseMoveEvent( QMouseEvent* e )
{
    if ( e->buttons() & Qt::LeftButton ) {
        move( e->globalPosition().toPoint() - drag_position );
        e->accept();
    }
}

// Overloaded mouse press fade window function :: - -                                  - -
void MainWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if ( e->button() == Qt::LeftButton ) {
        window_fade_to( 1, 300 );
        e->accept();
    }
}

// Ctrl+W -> Exit, Alt+T -> Always on top toggle :: - -                                - -
void MainWindow::keyPressEvent( QKeyEvent* e )
{
    if ( e->modifiers() == Qt::ControlModifier && e->key() == Qt::Key_W ) {
        connect( window_fade_to( 0, 400 ), &QPropertyAnimation::finished, this, [&](){
            this->close();
        } );
    }
    else if ( e->key() == Qt::Key_3 ) { minimize(); }
    else if ( e->modifiers() == Qt::AltModifier && e->key() == Qt::Key_T )
    {
        is_force_focused = !is_force_focused;
        e->accept();

        QPA* fade = window_fade_to( 0, 500, QEasingCurve::OutCubic );
        connect( fade, &QPropertyAnimation::finished, this, [&]()
        {
            if ( is_force_focused ) {
                setWindowFlags(
                    windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint
                );
            }
            else {
                setWindowFlags(
                    windowFlags() ^ ( Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint )
                );
            }
            show();
            window_fade_to( 1, 500, QEasingCurve::InCubic );
        } );
    }
    else { QMainWindow::keyPressEvent( e ); }
}

// Overloaded function that allows for unminimized state to go to focused :: - -       - -
void MainWindow::changeEvent( QEvent* e )
{
    if ( e->type() == QEvent::WindowStateChange )
    {
        QWindowStateChangeEvent* sce = static_cast<QWindowStateChangeEvent*>( e );
        if ( ( sce->oldState() & Qt::WindowMinimized ) &&
            !( windowState() & Qt::WindowMinimized ) )
        { window_fade_to( 1, 500 ); }
    }
    QMainWindow::changeEvent( e );
}
// End. :: - -                                                                         - -
