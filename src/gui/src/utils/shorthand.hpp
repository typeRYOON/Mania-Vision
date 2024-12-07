#ifndef SHORTHAND_H
#define SHORTHAND_H

#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QStackedLayout>
#include <QPushButton>
#include <QObject>
#include <QLabel>

using QO = QObject;
using QW = QWidget;

using CQS = const QString;
using QS  = QString;

using QPB = QPushButton;
using QF  = QFrame;
using QL  = QLabel;

using QSL = QStackedLayout;
using QVL = QVBoxLayout;
using QHL = QHBoxLayout;

using QDS = QGraphicsDropShadowEffect;
using QOE = QGraphicsOpacityEffect;
using QPA = QPropertyAnimation;

//
typedef struct ManiaEntry {
    QVector<double> kps_graph;
    QS file_name;
    QS fps;
    QS id;
    QS duration;

    //
    friend QDataStream& operator<<( QDataStream& out, const ManiaEntry& e ) {
        out << e.kps_graph << e.file_name << e.fps << e.id << e.duration;
        return out;
    }
    //
    friend QDataStream& operator>>( QDataStream& in, ManiaEntry& e ) {
        in  >> e.kps_graph >> e.file_name >> e.fps >> e.id >> e.duration;
        return in;
    }
} ManiaEntry;
using ManiaEntryPtr = std::shared_ptr<ManiaEntry>;

#endif // SHORTHAND_H 
