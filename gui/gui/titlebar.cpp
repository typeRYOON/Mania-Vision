#include "titlebar.h"

// TitleBar Widget Constructor :: - -                                                  - -
TitleBar::TitleBar( Utils* u, QWidget* p ) : utils( u ), QFrame{ p }
{
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setFixedHeight( 46 );
    setObjectName( "TitleBar" );
    setStyleSheet(
        "#TitleBar { background-color: #181818 }"
    );
}
