#include "utils.h"

//
Utils::Utils( void )
{
    const qint32 font_ID = QFontDatabase::addApplicationFont(
        "res/Hiragino Maru Gothic ProN W4.otf"
    );
    if ( font_ID == -1 ) { return; }
    font_family = QFontDatabase::applicationFontFamilies( font_ID ).at(0);
}

//
QFont Utils::get_font( const quint8& point_size ) {
    return QFont( font_family, point_size, QFont::Bold );
}

// Drop shadow maker :: - -                                                            - -
QDS* Utils::make_dropshadow( const QColor& color, QObject* p, const quint8& r )
{
    QDS* ret = new QDS( p );
    ret->setBlurRadius( r );
    ret->setOffset( 0, 0 );
    ret->setColor( color );
    return ret;
}

//
QF* Utils::makeVLine( const QSize& size, const QColor& color, QObject* p )
{
    QF* ret = new QF();
    ret->setFixedSize( size );
    ret->setStyleSheet( "background-color: white; border: 0px;" );
    ret->setGraphicsEffect(
        make_dropshadow( QColor( color ), p )
        );
    return ret;
}

//
QPA* Utils::make_animation(
    const quint32     & duration,
    const QVariant    & start,
    const QVariant    & end,
    const QEasingCurve& curve,
    const QByteArray  & property,
    QObject*            target,
    QObject*            parent )
{
    QPA* ret = new QPA( target, property, parent );
    ret->setDuration( duration );
    ret->setStartValue( start );
    ret->setEndValue( end );
    ret->setEasingCurve( curve );
    return ret;
}
// End. :: - -                                                                         - -
