#ifndef SELECTBOX_H
#define SELECTBOX_H

#include "../utils/qcustomplot.h"
#include "../utils/utils.h"
#include "../utils/processhandler.h"

#include <QWidget>
#include <QSoundEffect>
#include <QScrollArea>
#include <QProcess>
#include <QElapsedTimer>

class SelectBox : public QF {
    Q_OBJECT

public:
    explicit SelectBox( Utils*, QW* = nullptr );
    ProcessHandler* process_handler = nullptr;

public slots:
    void update_list( void );

private:
    Utils* utils;

    //
    QSoundEffect* se;

    //
    QVL* layout_vl;
    ManiaEntryPtr current = nullptr;

    //
    QCustomPlot* plot;
    QCPGraph*    graph;
    QPA*         plotOpacity_a;
    QL*          file_name_ql;
    QL*          fps_ql;
    QL*          id_ql;
    QL*          duration_ql;
    QVector<double> default_x;

    QProcess* python_runner = nullptr;
    QElapsedTimer* py_timer = nullptr;
    QS expected_file_name;

    // funcs :
    QF* make_selected_box( void );
    QScrollArea* make_scroll_qw( void );
    QHL* make_entry_hl( const ManiaEntryPtr& );


private slots:
    void update_selection( CQS& );
    void run_python( void );

    void handle_error( void );
    void handle_finished( qint32, QProcess::ExitStatus );


signals:
    void py_start( void );
    void add_log( const quint16&, CQS& );
    void mania_change( const bool&, CQS& );
};

#endif // SELECTBOX_H
