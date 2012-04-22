AR = ar
RANLIB = ranlib
TARGET = verge3
LD=g++

OBJS = $(addprefix ../verge/Source/, \
	a_common.o a_vfile.o a_codec.o a_config.o a_handle.o a_image.o  \
	g_script.o g_startup.o g_chr.o g_font.o g_controls.o g_engine.o g_editcode.o g_sound.o g_entity.o g_sprites.o g_map.o g_vsp.o garlick.o lua_main.o \
	vc_builtins.o  vc_library.o lua_vector  algebra3 \
	vid_manager.o vid_sysfont.o vid_ddblit.o vid_timeless.o vid_fbfx.o \
	vid_macbase.o mac_joystick.o mac_network.o mac_keyboard.o mac_mouse.o mac_system.o mac_timer.o mac_movie.o )

all: verge3

verge3: $(OBJS)
	g++ $(OBJS) -o verge3  $(LDFLAGS)


clean:
	rm -f verge3 $(OBJS)

INCDIR =

# IMPORTANT: sdl-config referenced below probably belongs to SDL-1.2
# You may need to replace sdl-config with the full path to sdl-config of SDL-1.3
# in your system.
#SDL_CFLAGS = $(shell sdl-config --cflags)
#SDL_LDFLAGS = $(shell sdl-config --libs)
SDL_CFLAGS = $(shell /home/salviati/root/bin/sdl-config --cflags)
SDL_LDFLAGS = $(shell /home/salviati/root/bin/sdl-config --libs)
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-2.0)
GTK_LDFLAGS = $(shell pkg-config --libs gtk+-2.0)
CFLAGS = -c $(SDL_CFLAGS) $(GTK_CFLAGS) -O2 -Wall -fpermissive -ggdb -D__LINUX__
LDFLAGS = -lm -lc -lpng -lcorona -lz -llua -laudiere  $(SDL_LDFLAGS) $(GTK_LDFLAGS)
CXXFLAGS = $(CFLAGS)
