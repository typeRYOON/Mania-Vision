#include "selectbox.h"
#include <QScrollArea>
#include <QProcess>

//
SelectBox::SelectBox( Utils* u, QW* p ) : utils( u ), QF{ p }
{
    setFixedSize( 288, 806 );
    setObjectName( "SelectBox" );
    setStyleSheet(
        "#SelectBox { background-color: #0b0b0b; border-radius: 16px;"
        "border: 1px solid #0d0d0d; }"
    );

    //
    process_handler = new ProcessHandler( this );

    //
    se = new QSoundEffect( this );
    se->setSource( QUrl::fromLocalFile( "assets/sounds/titlebar.wav" ) );
    se->setVolume( 0.05f );

    // Mini buttons :
    auto make_pb = [this]( CQS& text, const quint8 shift )
    {
        QPB* ret = new QPB();
        ret->setCursor( Qt::PointingHandCursor );
        ret->setFixedSize( 125, 28 );
        ret->setStyleSheet(
            "QPushButton { background-color: #0D0D0D; border-radius: 6px;"
            "border: 2px solid #4F4F4F; }"
        );
        QL* text_ql = new QL( text, ret );
        text_ql->setContentsMargins( 30+shift, 4, 0, 0 );
        text_ql->setStyleSheet( "QLabel{ color: white; }" );
        text_ql->setGraphicsEffect(
            utils->make_dropshadow( QColor( 0xFFFFFF ), text_ql )
        );
        text_ql->setFont( utils->get_font( 11 ) );
        return ret;
    };
    QPB* refresh_pb  = make_pb( "Refresh", 3 );
    QPB* generate_pb = make_pb( "Generate", 0 );

    QW* waves_qw = new QW();
    waves_qw->setAttribute( Qt::WA_TransparentForMouseEvents );
    QVL* waves_qvl = new QVL( waves_qw );
    waves_qvl->setContentsMargins( 10, 670, 0, 0 );

    QL* waves = new QL();
    waves->setFixedSize( 266, 115 );
    waves->setPixmap( QPixmap( "assets/misc/waves.png") );
    waves_qvl->addWidget( waves );

    // Layout construction :
    QW* wrap = new QW();
    QVL* layout = new QVL( wrap );
    layout->setAlignment( Qt::AlignTop );
    layout->setSpacing( 0 );
    layout->setContentsMargins( 14, 14, 0, 0 );

    layout->addWidget( make_selected_box() );
    layout->addSpacing( 6 );

    QHL* button_hl = new QHL();
    button_hl->setAlignment( Qt::AlignLeft );
    button_hl->setSpacing( 6 );
    button_hl->addWidget( refresh_pb );
    button_hl->addWidget( generate_pb );

    layout->addLayout( button_hl );
    layout->addSpacing( 3 );
    layout->addWidget( make_scroll_qw() );

    QSL* stacked_sl = new QSL( this );
    stacked_sl->setStackingMode( QStackedLayout::StackAll );
    stacked_sl->addWidget( waves_qw );
    stacked_sl->addWidget( wrap );




    // Connections :
    connect(
        refresh_pb, &QPB::clicked,
        process_handler, &ProcessHandler::get_file_list );
    connect(
        generate_pb, &QPB::clicked,
        this, &SelectBox::tester
    );


    connect(
        process_handler, &ProcessHandler::update_ready,
        this, &SelectBox::update_list );
}

//
void SelectBox::update_list( void )
{
    current = nullptr;
    // TODO: Remember to clear select box to first element in mania_lib:



    //
    if ( layout_vl->count() > 0 )
    {
        while ( QLayoutItem* item = layout_vl->takeAt( 0 ) )
        {
            QLayout* qhl = item->layout();
            while ( QLayoutItem* qhl_item = qhl->takeAt( 0 ) ) {
                if ( qhl_item->widget() ) { qhl_item->widget()->deleteLater(); }
            }
            delete qhl;
        }
    }

    //
    foreach ( const auto& e, process_handler->mania_library ) {
        layout_vl->addLayout( make_entry_hl( e ) );
    }
}

//
QF* SelectBox::make_selected_box( void )
{
    QF* ret = new QF();
    ret->setFixedSize( 256, 105 );
    ret->setStyleSheet(
        "QFrame { background-color: #0D0D0D; border: 2px solid #4F4F4F;"
        "border-radius: 8px; }"
    );
    return ret;
}

//
QScrollArea* SelectBox::make_scroll_qw( void )
{
    // Widget create :
    QW* scroll_widget = new QW();
    scroll_widget->setStyleSheet("background-color: transparent");
    scroll_widget->setFixedSize( 248, 600 );

    layout_vl = new QVL( scroll_widget );
    layout_vl->setContentsMargins( 3, 0, 0, 0 );
    layout_vl->setAlignment( Qt::AlignTop );
    layout_vl->setSpacing( 0 );


    // Scroll area :
    QScrollArea* ret = new QScrollArea();
    ret->setMinimumSize( 248, 600 );
    ret->setMaximumSize( 248, 600 );
    ret->setWidgetResizable( true );
    ret->setWidget( scroll_widget );
    ret->setStyleSheet(
        "QScrollArea { border: 0px solid transparent; background-color: transparent; }"
        "QScrollBar:vertical { width: 0; }"
    );
    return ret;
}

//
QHL* SelectBox::make_entry_hl( const ManiaEntryPtr& e )
{
    const static QFont font   = utils->get_font( 9 );
   // const static QFont font_file = utils->get_font( 9 );

    QL* id_ql = new QL( e->id );
    id_ql->setFixedSize( 38, 14 );
    id_ql->setStyleSheet( "QLabel { color: #4F4F4F; }" );
    id_ql->setGraphicsEffect(
        utils->make_dropshadow( QColor( 0x1F1F1F ), id_ql )
    );
    id_ql->setFont( font );

    QPB* pb = new QPB();
    pb->setCursor( Qt::PointingHandCursor );
    pb->setFixedSize( 200, 14 );
    connect( pb, &QPB::clicked, this, [=]( void ) {
        update_selection( e->file_name );
    } );

    QL* file_ql = new QL( e->file_name, pb );
    file_ql->setFixedHeight( 14 );
    file_ql->setStyleSheet( "QLabel { color: #FFFFFF; }" );
    file_ql->setGraphicsEffect(
        utils->make_dropshadow( QColor( 0xFFFFFF ), file_ql )
    );
    file_ql->setFont( font );

    QHL* ret = new QHL();
    ret->setAlignment( Qt::AlignLeft );
    ret->addWidget( id_ql );
    ret->addWidget( pb );
    return ret;
}

void SelectBox::update_selection( CQS& file_name )
{
    current = process_handler->name_to_ptr.value( file_name );

    // TODO update select box :
}

void SelectBox::tester( void )
{
    if ( current == nullptr ) { return; }

    QProcess* process = new QProcess( this );
    process->setProgram( "python" );
    process->setArguments(QStringList() << "test.py" << current->file_name );
    process->start();
}
