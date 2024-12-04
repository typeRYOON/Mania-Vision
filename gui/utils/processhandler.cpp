#include "processhandler.h"
#include <QDir>


//
ProcessHandler::ProcessHandler( QO* p ) : QO{ p }
{
    top_id = 0;
}

//
void ProcessHandler::get_file_list( void )
{
    //
    QVector<ManiaEntryPtr> new_mania_library;
    foreach ( const auto& e, mania_library )
    {
        // File was removed so remove entry associated :
        if ( !QFile::exists( QS( "mania-files/input-videos/%1" ).arg( e->file_name ) ) ) {
            name_to_ptr.remove( e->file_name );
            continue;
        }
        new_mania_library << e;
    }

    //
    QDir dir( "mania-files/input-videos" );
    dir.setNameFilters( { "*.mp4", "*.mov" } );
    foreach ( CQS& file_name, dir.entryList() )
    {
        if ( name_to_ptr.contains( file_name ) ) { continue; }
        ManiaEntry new_entry = {
            {},
            file_name,
            "0 fps",
            "0000"
        };
        ManiaEntryPtr mania_ptr = std::make_shared<ManiaEntry>( new_entry );
        name_to_ptr[file_name]  = mania_ptr;
        new_mania_library      << mania_ptr;
    }
    mania_library = new_mania_library;

    emit update_ready();
}
