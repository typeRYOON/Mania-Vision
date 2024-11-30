#include "consolebox.h"

//
ConsoleBox::ConsoleBox( Utils* u, QW* p ) : utils( u ), QF{ p }
{
    setFixedSize( 1000, 232 );
    setObjectName( "ConsoleBox" );
    setStyleSheet(
        "#ConsoleBox { background-color: #000000; border-radius: 16px;"
        "border: 2px solid #0b0b0b }"
    );
}
