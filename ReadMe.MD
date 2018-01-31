# FFParser_2017

FFParser is an application for extracting users' data from theirs Mozilla Firefox profiles.

Main features:
- Support for multiple profiles
- Smooth concurrent loading of multiple records
- Extracting of following data:
  - History
  - Bookmarks
  - Logins and passwords
  - Cache files
- Extracted data search
- Export extracted data in JSON
- Calculate MD5 for exported files

## Application Info 

Application consists of two components:
- FFParser_DLL library (developed with Visual Studio 2013)
- GUI (developed with QtCreator 3.3.1)

Main 3rd-party libraries and components:
- Boost 1.58.0 (property_tree, regex, filesystem, iostreams::filter::gzip)
- sqlite3
- WinCrypt
- QtConcurrent

Used design patterns:
- Factory method
- Singleton
- Facade

## Build steps

- Library:

  1. Add paths to Boost libraries (x86 and x64 correspondingly) to the project
  2. Build from Visual Studio

- GUI:

  1. Specify build paths in FFParser_GUI project (project_dir/obj/FFParser_GUI)
  2. Build from QtCreator

## Screenshots

![Main window](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/Main.png)

![Main menu](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/Main_menu.png)

![Record right click menu](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/RClick_menu.png)

![File right click menu](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/RClick_menu2.png)

![File export](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/FileExport.png)

![Data export](https://github.com/sp0wk/FFParser_2017/raw/master/docs/screenshots/DataExport.png)
