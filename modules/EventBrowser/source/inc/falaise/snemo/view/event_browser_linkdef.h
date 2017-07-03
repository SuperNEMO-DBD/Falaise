#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;           // added for namespace
#pragma link C++ nestedtypedef;         // added for namespace
#pragma link C++ namespace snemo::visualization::view;

#pragma link C++ class snemo::visualization::view::event_browser+;
#pragma link C++ class snemo::visualization::view::display_2d+;
#pragma link C++ class snemo::visualization::view::display_3d+;
#pragma link C++ class snemo::visualization::view::display_options+;
#pragma link C++ class snemo::visualization::view::browser_tracks+;
#pragma link C++ class snemo::visualization::view::event_selection+;
#pragma link C++ class snemo::visualization::view::progress_bar+;
#pragma link C++ class snemo::visualization::view::status_bar+;

#endif
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
