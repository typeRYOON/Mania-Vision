#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../utils/utils.h"

#include <QMainWindow>

//
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* = nullptr );
    ~MainWindow() = default;

private:
    //
    QF*  make_main_widget();
    QPA* window_fade_to(
        const float&,
        const quint16&,
        const QEasingCurve& = QEasingCurve::Linear
    );

    //
    QPoint drag_position;
    bool   is_force_focused = false;

    //
    Utils* utils = nullptr;

protected:
    void mousePressEvent( QMouseEvent* )   override;
    void mouseMoveEvent( QMouseEvent* )    override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void keyPressEvent( QKeyEvent* )       override;
    void changeEvent( QEvent* )            override;

};
#endif // MAINWINDOW_H
