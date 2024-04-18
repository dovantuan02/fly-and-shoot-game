CFLAGS		+= -I./sources/app/game/fs_game
CPPFLAGS	+= -I./sources/app/game/fs_game

VPATH += sources/app/game/fs_game

# CPP source files
SOURCES_CPP += sources/app/game/fs_game/fs_plane.cpp
SOURCES_CPP += sources/app/game/fs_game/fs_missile.cpp
SOURCES_CPP += sources/app/game/fs_game/fs_tunnel_wall.cpp
SOURCES_CPP += sources/app/game/fs_game/fs_explosion.cpp
SOURCES_CPP += sources/app/game/fs_game/fs_mine.cpp
SOURCES_CPP += sources/app/game/fs_game/fs_bom.cpp
