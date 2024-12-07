#include "processhandler.h"
#include <QDir>


//
ProcessHandler::ProcessHandler( QO* p ) : QO{ p }
{
    top_id = 1;
    ManiaEntry default_entry = {
        {},
        "No files",
        "0 fps",
        "0000",
        "0:00"
    };
    for ( quint8 i = 0; i < 30; ++i ) { default_entry.kps_graph << 10; }

    ManiaEntryPtr default_ptr = std::make_shared<ManiaEntry>( default_entry );
    name_to_ptr["NULL"]       = default_ptr;
    mania_library            << default_ptr;
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
            if ( e->file_name == "No files" ) { continue; }
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
            "0000",
            "0:00"
        };
        ManiaEntryPtr mania_ptr = std::make_shared<ManiaEntry>( new_entry );
        name_to_ptr[file_name]  = mania_ptr;
        new_mania_library      << mania_ptr;
    }
    mania_library = new_mania_library;

    emit update_ready();
}
