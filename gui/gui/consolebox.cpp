#include "consolebox.h"

//
ConsoleBox::ConsoleBox( Utils* u, QW* p ) : utils( u ), QF{ p }
{
    setFixedSize( 1000, 232 );
    setObjectName( "ConsoleBox" );
    setStyleSheet(
        "#ConsoleBox { background-color: #000000; border-radius: 16px;"
        "border: 2px solid #0b0b0b }"
    );

    // History Label Row :
    QF* vline = utils->make_vline( QSize( 4, 16 ), QColor( 0xFFFFFF ), this );
    vline->setStyleSheet(
        "background-color: white; border: 0px; border-radius: 2px" );

    QL* history_ql = new QL( "History", this );
    history_ql->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    history_ql->setFont( utils->get_font( 12 ) );
    history_ql->setStyleSheet( "color: #FFFFFF;");
    history_ql->setGraphicsEffect(
        utils->make_dropshadow( QColor( 0xFFFFFF ), history_ql )
    );

    // Layout construction :
    QHL* history_ql_hl = new QHL();
    history_ql_hl->setContentsMargins( 18, 0, 0, 0 );
    history_ql_hl->setSpacing( 3 );
    history_ql_hl->setAlignment( Qt::AlignLeft );
    history_ql_hl->addWidget( vline );
    history_ql_hl->addWidget( history_ql );

    QVL* history_vl = new QVL();
    history_vl->setAlignment( Qt::AlignTop );
    history_vl->setSpacing( 0 );
    history_vl->addLayout( history_ql_hl );
    history_vl->addWidget( make_history_log() );

    QHL* layout_hl = new QHL( this );
    layout_hl->setContentsMargins( 16, 6, 0, 0 );
    layout_hl->setSpacing( 12 );
    layout_hl->setAlignment( Qt::AlignLeft );
    layout_hl->addLayout( history_vl );
    layout_hl->addWidget( make_page_sl() );
}

//
void ConsoleBox::add_log( const quint32& sec, CQS& text )
{
    for ( quint8 i = 0; i < 3; ++i )
    {
        log_labels.at(i).first ->setText( log_labels.at( i + 1 ).first ->text() );
        log_labels.at(i).second->setText( log_labels.at( i + 1 ).second->text() );
    }
    log_labels.at(3).first ->setText(
        QS( "%1:%2" )
            .arg( sec / 60, 2, 10, QChar( '0' ) )
            .arg( sec % 60, 2, 10, QChar( '0' ) )
    );
    log_labels.at(3).second->setText( text );
}

//
QW* ConsoleBox::make_history_log( void )
{
    auto make_qf = []( CQS& style )
    {
        QF* ret = new QF();
        ret->setObjectName( "logqf" );
        ret->setStyleSheet( style );
        ret->setFixedHeight( 40 );
        return ret;
    };
    auto make_ql = [this]( const bool time )
    {
        QL* ret = new QL( time ? "00:00" : "..." );
        if ( time )
        {
            ret->setStyleSheet( "QLabel { color: #4F4F4F; }" );
            ret->setFont( utils->get_font( 11 ) );
            ret->setFixedWidth( 60 );
            ret->setGraphicsEffect(
                utils->make_dropshadow( QColor( 0x4F4F4F ), ret )
            );
        } else
        {
            ret->setStyleSheet( "QLabel { color: #FFFFFF; }" );
            ret->setFont( utils->get_font( 14 ) );
            ret->setFixedWidth( 400 );
            ret->setGraphicsEffect(
                utils->make_dropshadow( QColor( 0xFFFFFF ), ret )
            );
        }
        return ret;
    };

    QW* ret_qw = new QW();
    ret_qw->setFixedSize( 480, 160 );

    QVL* layout_vl = new QVL( ret_qw );
    QF*  qf;
    for ( quint8 i = 0; i < 4; ++i )
    {

        if ( i & 0x1 ) { qf = make_qf( " #logqf { background-color: transparent; }" );       }
        else { qf = make_qf( "#logqf { border-radius: 8px; background-color: #0b0b0b; }" ); }

        QHL* log_row_hl = new QHL( qf );
        log_row_hl->setAlignment( Qt::AlignLeft );
        log_row_hl->addSpacing( 4 );

        const QPair<QL*, QL*> row_ql = { make_ql( true ), make_ql( false ) };
        log_row_hl->addWidget( row_ql.first );
        log_row_hl->addWidget( row_ql.second );
        layout_vl->addWidget( qf );

        log_labels << row_ql;
    }
    return ret_qw;
}

//
QW* ConsoleBox::make_page_sl( void )
{
    QW* bounding_qw = new QW();
    bounding_qw->setFixedSize( 466, 190 );
    bounding_qw->setStyleSheet( "background-color: transparent;" );

    //
    QF* vline = utils->make_vline( QSize( 4, 16 ), QColor( 0xFFFFFF ), this );
    vline->setStyleSheet(
        "background-color: white; border: 0px; border-radius: 2px" );

    QF* box = new QF();
    box->setFixedSize( 466, 156 );
    box->setStyleSheet(
        "QFrame { border-radius: 8px; background-color: #000000;"
        "border: 2px solid #0b0b0b }"
    );

    QW*  static_qw = new QW();
    QVL* static_vl = new QVL( static_qw );
    static_vl->setSpacing( 12 );
    static_vl->setAlignment( Qt::AlignTop );
    static_vl->setContentsMargins( 0, 0, 0, 0 );

    QHL* line_shift = new QHL();
    line_shift->setAlignment( Qt::AlignLeft );
    line_shift->setContentsMargins( 10, 0, 0, 0 );
    line_shift->addWidget( vline );

    static_vl->addLayout( line_shift );
    static_vl->addWidget( box );

    QSL* ret_sl = new QSL( bounding_qw );
    ret_sl->setStackingMode( QStackedLayout::StackAll );
    ret_sl->addWidget( static_qw );

    return bounding_qw;
}
//  End. :: - -                                                                        - -
