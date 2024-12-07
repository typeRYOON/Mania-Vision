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
    python_runner = new QProcess( this );
    python_runner->setProgram( "python" );
    py_timer = new QElapsedTimer();

    connect( python_runner, &QProcess::readyReadStandardError,
            this, &SelectBox::handle_error );
    connect(
        python_runner,
        QOverload<qint32, QProcess::ExitStatus>::of( &QProcess::finished ),
        this,
        &SelectBox::handle_finished
    );

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
        this, &SelectBox::run_python
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

    //
    plot = new QCustomPlot();
    plot->setFixedSize( QSize( 226, 50 ) );

    plot->axisRect()->setMinimumMargins( QMargins( 0, 0, 0, 0 ) );
    plot->axisRect()->setMargins( QMargins( 0, 0, 0, 0 ) );
    plot->setAttribute( Qt::WA_NoSystemBackground, false );
    plot->setAttribute( Qt::WA_TransparentForMouseEvents );
    plot->setAttribute( Qt::WA_TranslucentBackground );
    plot->axisRect()->setBackground( Qt::transparent );
    plot->xAxis->grid()->setPen( QPen( Qt::NoPen ) );
    plot->yAxis->grid()->setPen( QPen( Qt::NoPen ) );
    plot->setBackground( Qt::transparent );
    plot->setInteractions( QCP::iNone );
    plot->xAxis->setTickLabels( false );
    plot->yAxis->setTickLabels( false );
    plot->xAxis->setLabelPadding( 0 );
    plot->xAxis->setVisible( false );
    plot->yAxis->setVisible( false );
    plot->xAxis->setTicks( false );
    plot->yAxis->setTicks( false );
    plot->xAxis->setRange( 0, 29 );
    plot->yAxis->setRange( 0, 80 );


    for ( quint8 i = 0; i < 30; ++i ) {
        default_x << i;
    }
    graph = plot->addGraph();
    graph->setData(
        default_x,
        process_handler->name_to_ptr["NULL"]->kps_graph
    );

    // Pen, line :: - -
    QPen pen;
    pen.setColor( QColor( 181, 210, 255 ) );
    pen.setWidth( 2 );
    graph->setPen( pen );

    // Fill gradient :: - -
    QLinearGradient fillGradient( 0, 0, 0, plot->height() );
    fillGradient.setColorAt( 0, QColor(181, 210, 255, 80 ) );
    fillGradient.setColorAt( 1, QColor(0,   255, 255, 0   ) );
    graph->setBrush( fillGradient );

    // Labels :
    auto make_ql = [this]( CQS& text, const QSize& s, bool large )
    {
        QL* ret = new QL( text, this );
        ret->setFixedSize( s );

        if ( large ) {
            ret->setStyleSheet(
                "QLabel{ color: #FFFFFF; background-color: transparent;"
                "border: 0px transparent; }" );
            ret->setGraphicsEffect(
                utils->make_dropshadow( QColor( 0xFFFFFF ), ret ) );
            ret->setFont( utils->get_font( 12 ) );
        } else {
            ret->setStyleSheet(
                "QLabel{ color: #6F6F6F; background-color: transparent;"
                "border: 0px transparent; }" );
            ret->setGraphicsEffect(
                utils->make_dropshadow( QColor( 0x6F6F6F ), ret )
            );
            ret->setFont( utils->get_font( 10 ) );
        }
        return ret;
    };

    file_name_ql = make_ql( "No files", { 234, 20 }, true );
    fps_ql       = make_ql( "0 fps", { 98, 16 }, false );
    id_ql        = make_ql( "0000",  { 70, 16 }, false );
    duration_ql  = make_ql( "0:00",  { 60, 16 }, false );
    duration_ql->setAlignment( Qt::AlignRight );

    //
    QW*  plot_qw = new QW();
    QVL* plot_vl = new QVL( plot_qw );
    plot_vl->setAlignment( Qt::AlignTop );
    plot_vl->setContentsMargins( 12, 12, 0, 0 );
    plot_vl->addWidget( plot );

    //
    QW*  backing_qw = new QW();
    QVL* backing_vl = new QVL( backing_qw );
    backing_vl->setAlignment( Qt::AlignTop );
    QL* backing = new QL();
    backing->setStyleSheet(
        "QLabel { background-color: transparent; border: 0px transparent }" );
    backing->setFixedSize( 230, 50 );
    backing->setPixmap( QPixmap( "assets/misc/select-back.png" ) );
    backing_vl->addWidget( backing );

    QW*  plot_gradient_qw = new QW();
    QVL* plot_gradient_vl = new QVL( plot_gradient_qw );
    plot_gradient_vl->setAlignment( Qt::AlignTop );
    QL* plot_gradient = new QL();
    plot_gradient->setStyleSheet(
        "QLabel { background-color: transparent; border: 0px transparent }" );
    plot_gradient->setFixedSize( 232, 54 );
    plot_gradient->setPixmap( QPixmap( "assets/misc/select-gradient.png" ) );
    plot_gradient_vl->addWidget( plot_gradient );

    //
    QHL* label_hl = new QHL();
    label_hl->setAlignment( Qt::AlignLeft );
    label_hl->setContentsMargins( 2, 0, 0, 0 );
    label_hl->setSpacing( 0 );
    label_hl->addWidget( fps_ql );
    label_hl->addWidget( id_ql );
    label_hl->addWidget( duration_ql );

    //
    QW*  labels_qw = new QW();
    QVL* labels_vl = new QVL( labels_qw );
    labels_vl->setAlignment( Qt::AlignTop);
    labels_vl->setContentsMargins( 10, 58, 0, 0 );
    labels_vl->setSpacing( 0 );
    labels_vl->addWidget( file_name_ql );
    labels_vl->addLayout( label_hl );

    //
    QSL* layout_sl = new QSL( ret );
    layout_sl->setStackingMode( QStackedLayout::StackAll );


    layout_sl->addWidget( labels_qw );
    layout_sl->addWidget( plot_gradient_qw );
    layout_sl->addWidget( plot_qw );
    layout_sl->addWidget( backing_qw );

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
    // No updates while running something else :
    if ( python_runner->isOpen() ) { return; }




    current = process_handler->name_to_ptr.value( file_name );

    file_name_ql->setText( file_name );
    fps_ql->setText( current->fps );
    id_ql->setText( current->id );
    duration_ql->setText( current->duration );

    if ( current->id == "0000" ) {
        emit mania_change( false, "" );
        graph->setData(
            default_x,
            process_handler->name_to_ptr["NULL"]->kps_graph
        );
    } else {
        emit mania_change( true, current->id + ".txt" );
        graph->setData( default_x, current->kps_graph );
    }
}

//
void SelectBox::run_python( void )
{
    if ( current == nullptr )      { return; }
    if ( python_runner->isOpen() ) { return; }

    expected_file_name = QS( "%1" ).arg( process_handler->top_id, 4, 10, QChar( '0' ) );
    if ( QFile::exists( QS( "mania-files/__mania__/%1.txt" ).arg( expected_file_name ) ) ) {
        return;
    }

    //
    python_runner->setArguments(
        QStringList() << "python/mania-vision-gui.py" << current->file_name << expected_file_name
    );
    emit py_start();
    python_runner->start();
    py_timer->start();
}

//
void SelectBox::handle_error( void ) {
    qDebug() << "Err:" << python_runner->readAllStandardError();
}

//
void SelectBox::handle_finished( qint32 exit_code, QProcess::ExitStatus status )
{
    emit py_start();

    const qint64 ms = py_timer->elapsed() / 1000;
    if ( status == QProcess::NormalExit ) {
        emit add_log( ms, QS( "Generated %1" ).arg( expected_file_name ) );

        QFile file( QS( "mania-files/__mania__/%1.txt" ).arg( expected_file_name ) );
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) ) { return; }

        QTextStream in( &file );
        in.readLine(); // Header
        while ( !in.atEnd() )
        {
            CQS line = in.readLine().trimmed();
            if ( line == "!" ) { break; }
            current->kps_graph << line.toDouble();
        }
        file.close();
        graph->setData( default_x, current->kps_graph );
        plot->yAxis->setRange(
            0, *std::max_element( current->kps_graph.begin(), current->kps_graph.end() )
        );
        plot->replot();
        current->id = process_handler->top_id;
        process_handler->top_id++;

        id_ql->setText( expected_file_name );
        emit mania_change( true, expected_file_name );
    }
    else { emit add_log( ms, "Vision Program Crashed!" ); }
}
