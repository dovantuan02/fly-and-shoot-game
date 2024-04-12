CFLAGS		+= -I./sources/app/game/process
CPPFLAGS	+= -I./sources/app/game/process

VPATH += sources/app/game/process

# CPP source files
SOURCES_CPP += sources/app/game/process/prc_plane.cpp
SOURCES_CPP += sources/app/game/process/prc_missle.cpp
SOURCES_CPP += sources/app/game/process/prc_tunnel_wall.cpp
SOURCES_CPP += sources/app/game/process/prc_explosion.cpp
SOURCES_CPP += sources/app/game/process/prc_mine.cpp
SOURCES_CPP += sources/app/game/process/prc_bom.cpp

