#include "selectbox.h"

//
SelectBox::SelectBox( Utils* u, QW* p ) : utils( u ), QF{ p }
{
    setFixedSize( 288, 806 );
    setObjectName( "SelectBox" );
    setStyleSheet(
        "#SelectBox { background-color: #0b0b0b; border-radius: 16px;"
        "border: 2px solid #0d0d0d }"
    );
}
