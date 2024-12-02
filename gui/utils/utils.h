/* utils.h */
#ifndef UTILS_H
#define UTILS_H
#include "shorthand.hpp"
#include <QFontDatabase>

class Utils {
public:
    explicit Utils( void );
    [[nodiscard]] QFont get_font( const quint8& );
    [[nodiscard]] QDS*  make_dropshadow( const QColor&, QObject*, const quint8& = 10 );
    [[nodiscard]] QF*   make_vline( const QSize&, const QColor&, QObject* );
    [[nodiscard]] QPA*  make_animation(
        const quint32&,
        const QVariant&,
        const QVariant&,
        const QEasingCurve&,
        const QByteArray&,
        QObject*,
        QObject*
    );

private:
    QS font_family = "Arial";
};

#endif // UTILS_H :: - -                                                               - -
