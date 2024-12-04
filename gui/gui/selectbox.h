#ifndef SELECTBOX_H
#define SELECTBOX_H

#include "../utils/utils.h"
#include "../utils/processhandler.h"

#include <QWidget>
#include <QSoundEffect>
#include <QScrollArea>

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



    // funcs :
    QF* make_selected_box( void );
    QScrollArea* make_scroll_qw( void );
    QHL* make_entry_hl( const ManiaEntryPtr& );

private slots:
    void update_selection( CQS& );
    void tester( void );

signals:
};

#endif // SELECTBOX_H
