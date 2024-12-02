#include "titlebar.h"
#include <QDesktopServices>
#include <QSoundEffect>
#include <QPainter>
#include <QDir>

// TitleBar Widget Constructor :: - -                                                  - -
TitleBar::TitleBar( Utils* u, QWidget* p ) : utils( u ), QFrame{ p }
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setFixedHeight( 46 );
    setObjectName( "TitleBar" );
    setStyleSheet(
        "#TitleBar { background-color: #181818 }"
    );

    se = new QSoundEffect( this );
    se->setSource( QUrl::fromLocalFile( "assets/sounds/titlebar.wav" ) );
    se->setVolume( 0.05f );

    // Time labels:
    auto make_time_ql = [&u]
    ( CQS& text, CQS& style, const QColor c, const quint8 f_sz, const quint8 h, QW* p )
    {
        QL* ret = new QL( text, p );
        ret->setStyleSheet( style );
        ret->setFont( u->get_font( f_sz ) );
        ret->setGraphicsEffect(
           u->make_dropshadow( c, ret )
        );
        ret->setFixedHeight( h );
        return ret;
    };
    prog_time = make_time_ql(
        "00:00", "color: #FFFFFF", QColor( 0xFFFFFF ), 11, 13, this );

    prog_timer = new QTimer( this );
    connect( prog_timer, &QTimer::timeout, this, [&](){
        prog_time->setText(
            QString( "%1:%2" )
                .arg( ++prog_elapsed / 60, 2, 10, QChar( '0' ) )
                .arg(   prog_elapsed % 60, 2, 10, QChar( '0' ) )
        );
    } );
    prog_timer->start( 60000 );

    QL* run_text = make_time_ql(
        "Running: ", "color: #FF5353", QColor( 0xFF5353 ), 9, 16, this );
    run_time     =
        make_time_ql( "00:00"  ,   "color: #FF5353", QColor( 0xFF5353 ), 9, 16, this );

    // Layout construction :
    QW* qsl_qw = new QW();
    qsl_qw->setFixedWidth( 110 );

    QSL* time_labels_sl = new QSL(qsl_qw);
    time_labels_sl->setStackingMode( QStackedLayout::StackAll );

    QW*  prog_qw = new QW();
    QVL* prog_vl = new QVL( prog_qw );

    prog_vl->setContentsMargins( 14, 0, 0, 10 );
    prog_vl->addWidget( prog_time );
    time_labels_sl->addWidget( prog_qw );

    QW*  label_row_qw = new QW();
    label_row_qw->setFixedWidth( 110 );
    QHL* time_run_hl = new QHL( label_row_qw );
    time_run_hl->setSpacing( 0 );
    time_run_hl->setContentsMargins( 14, 14, 0, 0 );
    time_run_hl->addWidget( run_text );
    run_time->setContentsMargins( 0, 2, 0, 0 );
    time_run_hl->addWidget( run_time );
    time_labels_sl->addWidget( label_row_qw );

    QHL* layout_hl = new QHL( this );
    layout_hl->setSpacing( 0 );
    layout_hl->setAlignment( Qt::AlignLeft );
    layout_hl->setContentsMargins( 0, 0, 0, 0 );

    layout_hl->addSpacing( 30 );
    layout_hl->addWidget( qsl_qw );
    layout_hl->addSpacing( 15 );
    layout_hl->addWidget( make_slide_box() );
    layout_hl->addSpacing( 526 );

    auto make_pb = [this]( const QPixmap& pixmap, const QColor& c )
    {
        QPB* ret = new QPB();
        ret->setStyleSheet( "background-color: transparent; border: 0px" );
        ret->setFixedSize( 24, 24 );
        ret->setIconSize( QSize( 24, 24 ) );
        ret->setIcon( pixmap );
        ret->setCursor( Qt::PointingHandCursor );
        ret->setGraphicsEffect(
            utils->make_dropshadow( c, ret )
        );
        return ret;
    };

    //
    QPB* codebase_pb = make_pb(
        QPixmap( "assets/icons/codebase.png" ), QColor( 0xFFFFFF ) );
    QPB* sounds_pb = make_pb(
        QPixmap( "assets/icons/sounds.png" ),   QColor( 0xFFFFFF ) );
    QPB* explorer_pb = make_pb(
        QPixmap( "assets/icons/explorer.png" ), QColor( 0xFFFFFF ) );
    QPB* releases_pb = make_pb(
        QPixmap( "assets/icons/releases.png" ), QColor( 0xFFFFFF ) );

    connect( codebase_pb, &QPB::clicked, this, [this]( void )
    {
        QDesktopServices::openUrl( QUrl( "https://github.com/typeRYOON/Mania-Vision" ) );
        if ( sound_on ) { se->play(); }
    } );
    connect( sounds_pb, &QPB::clicked, this, [sounds_pb, this]( void )
    {
        se->play();
        sound_on = !sound_on;
        sounds_pb->setIcon( recolor_pixmap(
            QPixmap( "assets/icons/sounds.png" ),
            sound_on ? QColor( 0xFFFFFF ) : QColor( 0xA0A0A0 ) )
        );
    } );
    connect( explorer_pb, &QPB::clicked, this, [this]( void ) {
        if ( sound_on ) { se->play(); }
        QDesktopServices::openUrl( QUrl::fromLocalFile( QDir::currentPath() ) );
    } );
    connect( releases_pb, &QPB::clicked, this, [this]( void ) {
        if ( sound_on ) { se->play(); }
        QDesktopServices::openUrl( QUrl( "https://github.com/typeRYOON/Mania-Vision/releases" ) );
    } );

    layout_hl->addWidget( codebase_pb );
    layout_hl->addSpacing( 24 );
    layout_hl->addWidget( sounds_pb );
    layout_hl->addSpacing( 24 );
    layout_hl->addWidget( explorer_pb );
    layout_hl->addSpacing( 24 );
    layout_hl->addWidget( releases_pb );
    layout_hl->addSpacing( 24 );
    layout_hl->addWidget(
        utils->make_vline( QSize( 2, 24 ), QColor( 0xFFFFFF ), this )
    );
    layout_hl->addSpacing( 6 );

    QL* title_ql = new QL( "Mania Vision", this );
    title_ql->setFixedWidth( 110 );
    title_ql->setFont( utils->get_font( 12 ) );
    title_ql->setStyleSheet( "color: #FFFFFF" );
    title_ql->setGraphicsEffect(
        utils->make_dropshadow( QColor( 0xFFFFFF ), title_ql ) );
    layout_hl->addWidget( title_ql );

    QL* icon_ql = new QL();
    icon_ql->setFixedSize( 38, 38 );
    icon_ql->setPixmap( QPixmap( "assets/icons/icon.png" ) );
    icon_ql->setGraphicsEffect(
        utils->make_dropshadow( QColor( 0xFFFFFF ), icon_ql )
    );
    layout_hl->addWidget( icon_ql );
    layout_hl->addSpacing( 54 );

    QPB* minimize_pb = make_pb(
        QPixmap( "assets/icons/minimize.png" ), QColor( 0xFFFFFF ) );
    QPB* close_pb = make_pb(
        QPixmap( "assets/icons/close.png" ), QColor( 0xFFFFFF ) );
    minimize_pb->setIconSize( QSize( 18, 18 ) );
    close_pb->setIconSize( QSize( 18, 18 ) );

    layout_hl->addWidget( minimize_pb );
    layout_hl->addSpacing( 16 );
    layout_hl->addWidget( close_pb );

    connect( minimize_pb, &QPB::clicked, this, [this]( void ) {
        if ( sound_on ) { se->play(); }
        emit minimize_sig();
    } );
    connect( close_pb, &QPB::clicked, this, [this]( void ) {
        if ( sound_on ) { se->play(); }
        emit close_sig();
    } );

}

QF* TitleBar::make_slide_box( void )
{
    QF* ret = new QF();
    ret->setFixedWidth( 162 );
    ret->setStyleSheet(
        "QFrame { background-color: #131313; }"
    );

    // QHL of buttons:
    QHL* button_row = new QHL();
    button_row->setAlignment( Qt::AlignLeft );
    button_row->addSpacing( 8 );
    button_row->setSpacing( 26 );

    //
    auto make_pb = [this]( const QPixmap& pixmap, const QColor& c )
    {
        QPB* ret = new QPB();
        ret->setStyleSheet( "background-color: transparent; border: 0px" );
        ret->setFixedSize( 26, 26 );
        ret->setIconSize( QSize( 26, 26 ) );
        ret->setIcon( pixmap );
        ret->setCursor( Qt::PointingHandCursor );
        ret->setGraphicsEffect(
            utils->make_dropshadow( c, ret )
        );
        return ret;
    };

    QPB* graph_pb = make_pb(
        recolor_pixmap( QPixmap( "assets/icons/graph.png" ), QColor( 0xFFFFFF ) ),
        QColor( 0xFFFFFF )
    );
    QPB* settings_pb = make_pb(
        recolor_pixmap( QPixmap( "assets/icons/settings.png" ), QColor( 0xA0A0A0 ) ),
        QColor( 0xA0A0A0 )
    );
    QPB* credits_pb  = make_pb(
        recolor_pixmap( QPixmap( "assets/icons/credits.png" ), QColor( 0xA0A0A0 ) ),
        QColor( 0xA0A0A0 )
    );

    pressed = graph_pb;
    button_map.insert( graph_pb,    { 0, "assets/icons/graph.png"    } );
    button_map.insert( settings_pb, { 1, "assets/icons/settings.png" } );
    button_map.insert( credits_pb,  { 2, "assets/icons/credits.png"  } );

    connect( graph_pb,    &QPB::clicked, this, [=](){ handle_slider_press( graph_pb );    } );
    connect( settings_pb, &QPB::clicked, this, [=](){ handle_slider_press( settings_pb ); } );
    connect( credits_pb,  &QPB::clicked, this, [=](){ handle_slider_press( credits_pb );  } );

    button_row->addWidget( graph_pb );
    button_row->addWidget( settings_pb );
    button_row->addWidget( credits_pb );

    // Widget/QHL slider:
    QW*  slider_box = new QW();
    slider_box->setFixedHeight( 40 );
    slider_box->setAttribute( Qt::WA_TransparentForMouseEvents );

    slider = new QF();
    slider->setFixedSize( 25, 4 );
    slider->setStyleSheet(
        "background-color: white; border: 0px; border-radius: 2px"
    );
    slider_a = new QPA( slider, "geometry" );
    slider_a->setDuration( 400 );
    slider_a->setEasingCurve( QEasingCurve::InOutQuad );

    QHL* slider_hl  = new QHL( slider_box );
    slider_hl->setAlignment( Qt::AlignLeft );
    slider_hl->setContentsMargins( 0, 0, 0, 8 );
    slider_hl->addSpacing( 8 );
    slider_hl->addWidget( slider );

    QVL* ret_vl = new QVL( ret );
    ret_vl->setAlignment( Qt::AlignTop );
    ret_vl->setSpacing( 0 );
    ret_vl->addLayout( button_row );
    ret_vl->addWidget( slider_box );

    return ret;
}

QPixmap TitleBar::recolor_pixmap( const QPixmap& og, const QColor c )
{
    QPixmap ret( og.size() );
    ret.fill( Qt::transparent );

    QPainter painter( &ret );
    painter.setCompositionMode( QPainter::CompositionMode_Source );
    painter.drawPixmap( 0, 0, og );
    painter.setCompositionMode( QPainter::CompositionMode_SourceIn );
    painter.fillRect( ret.rect(), c );
    painter.end();
    return ret;
}

//
void TitleBar::handle_slider_press( QPB* new_press )
{
    if ( new_press == pressed ) { return; }
    const QPair<quint8, QS> new_pair = button_map.value( new_press );

    new_press->setIcon(
        recolor_pixmap( QPixmap( new_pair.second ), QColor( 0xFFFFFF ) ) );
    pressed->setIcon(
        recolor_pixmap(
            QPixmap( button_map.value( pressed ).second ),
            QColor( 0xA0A0A0 )
        ) );

    if ( sound_on ) { se->play(); }

    pressed = new_press;
    slider_a->setStartValue( slider->geometry() );
    slider_a->setEndValue( origin.translated( 52 * new_pair.first, 0 ) );
    slider_a->start();

    emit opt_change( new_pair.first );
}
//  End. :: - -                                                                        - -
