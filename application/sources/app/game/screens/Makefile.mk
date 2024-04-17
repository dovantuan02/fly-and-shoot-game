CFLAGS		+= -I./sources/app/game/screens
CPPFLAGS	+= -I./sources/app/game/screens

VPATH += sources/app/game/screens

# CPP source files
SOURCES_CPP += sources/app/game/screens/scr_idle.cpp
SOURCES_CPP += sources/app/game/screens/scr_startup.cpp
SOURCES_CPP += sources/app/game/screens/scr_bitmap.cpp
SOURCES_CPP += sources/app/game/screens/scr_menu.cpp
SOURCES_CPP += sources/app/game/screens/scr_game_on.cpp
SOURCES_CPP += sources/app/game/screens/scr_game_over.cpp
SOURCES_CPP += sources/app/game/screens/scr_tutorial.cpp
SOURCES_CPP += sources/app/game/screens/scr_setting.cpp
SOURCES_CPP += sources/app/game/screens/scr_history.cpp