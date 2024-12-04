#ifndef PROCESSHANDLER_H
#define PROCESSHANDLER_H

#include "shorthand.hpp"
#include <QObject>

class ProcessHandler : public QO {
    Q_OBJECT

public:
    explicit ProcessHandler( QO* = nullptr );
    QHash<QString, ManiaEntryPtr> name_to_ptr;
    QVector<ManiaEntryPtr>        mania_library;

public slots:
    void get_file_list( void );

private:
    quint16 top_id;

signals:
    void update_ready( void );
};

#endif // PROCESSHANDLER_H
