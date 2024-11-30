#include "gui/mainwindow.h"
#include <QApplication>

// Program Entrypoint :: - -                                                           - -
qint32 main( qint32 argc, char* argv[] )
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::Unset
    );

    QApplication a( argc, argv );
    MainWindow w;
    w.setWindowOpacity( 0 );
    return a.exec();
}
//  End. :: - -                                                                        - -
