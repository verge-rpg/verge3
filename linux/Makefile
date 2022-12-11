AR = ar
RANLIB = ranlib
TARGET = verge3
V3SRC = ../verge/Source/
ZLIBSRC = ../zlib/
CORSRC = ../corona/
LUASRC = ../lua/lua-5.1.2/

OBJS = \
	$(V3SRC)a_common.o $(V3SRC)a_dict.o $(V3SRC)a_vfile.o $(V3SRC)a_codec.o $(V3SRC)a_config.o $(V3SRC)a_handle.o $(V3SRC)a_image.o $(V3SRC)a_string.o \
	$(V3SRC)g_script.o $(V3SRC)g_startup.o $(V3SRC)g_chr.o $(V3SRC)g_font.o $(V3SRC)g_controls.o $(V3SRC)g_engine.o $(V3SRC)g_editcode.o $(V3SRC)g_sound.o $(V3SRC)snd_fmod.o $(V3SRC)g_entity.o $(V3SRC)g_sprites.o $(V3SRC)g_map.o $(V3SRC)g_vsp.o $(V3SRC)garlick.o $(V3SRC)lua_main.o $(V3SRC)lua_vector.o $(V3SRC)algebra3.o \
	$(V3SRC)vc_builtins.o $(V3SRC)vc_core.o $(V3SRC)vc_compiler.o $(V3SRC)vc_library.o $(V3SRC)vc_debug.o \
	$(V3SRC)vid_manager.o $(V3SRC)vid_sysfont.o $(V3SRC)vid_ddblit.o $(V3SRC)vid_timeless.o $(V3SRC)vid_fbfx.o \
	$(V3SRC)vid_macbase.o $(V3SRC)mac_joystick.o $(V3SRC)mac_network.o $(V3SRC)mac_keyboard.o $(V3SRC)mac_mouse.o $(V3SRC)mac_system.o $(V3SRC)mac_timer.o $(V3SRC)mac_movie.o \
	$(V3SRC)xerxes.o

LUAOBJS = $(LUASRC)lapi.o \
	$(LUASRC)lauxlib.o \
	$(LUASRC)lbaselib.o \
	$(LUASRC)lcode.o \
	$(LUASRC)ldblib.o \
	$(LUASRC)ldebug.o \
	$(LUASRC)ldo.o \
	$(LUASRC)ldump.o \
	$(LUASRC)lfunc.o \
	$(LUASRC)lgc.o \
	$(LUASRC)linit.o \
	$(LUASRC)liolib.o \
	$(LUASRC)llex.o \
	$(LUASRC)lmathlib.o \
	$(LUASRC)lmem.o \
	$(LUASRC)loadlib.o \
	$(LUASRC)lobject.o \
	$(LUASRC)lopcodes.o \
	$(LUASRC)loslib.o \
	$(LUASRC)lparser.o \
	$(LUASRC)lstate.o \
	$(LUASRC)lstring.o \
	$(LUASRC)lstrlib.o \
	$(LUASRC)ltable.o \
	$(LUASRC)ltablib.o \
	$(LUASRC)ltm.o \
	$(LUASRC)lundump.o \
	$(LUASRC)lvm.o \
	$(LUASRC)lzio.o \
	$(LUASRC)print.o

ZLIBOBJS = \
	$(ZLIBSRC)adler32.o $(ZLIBSRC)compress.o $(ZLIBSRC)crc32.o $(ZLIBSRC)gzio.o $(ZLIBSRC)uncompr.o $(ZLIBSRC)deflate.o $(ZLIBSRC)trees.o \
	$(ZLIBSRC)zutil.o $(ZLIBSRC)inflate.o $(ZLIBSRC)infblock.o $(ZLIBSRC)inftrees.o $(ZLIBSRC)infcodes.o $(ZLIBSRC)infutil.o $(ZLIBSRC)inffast.o
	
COROBJS = \
	$(CORSRC)Corona.o $(CORSRC)Convert.o $(CORSRC)DefaultFileSystem.o $(CORSRC)MemoryFile.o $(CORSRC)OpenBMP.o $(CORSRC)OpenGIF.o $(CORSRC)OpenPCX.o $(CORSRC)OpenPNG.o $(CORSRC)OpenJPEG.o $(CORSRC)OpenTGA.o $(CORSRC)SavePNG.o $(CORSRC)SaveTGA.o
	 
	
LIBUNGIFOBJS= \
	$(CORSRC)libungif-4.1.0/dgif_lib.o \
	$(CORSRC)libungif-4.1.0/gif_err.o \
	$(CORSRC)libungif-4.1.0/gifalloc.o	
	
LIBPNGOBJS=	\
	$(CORSRC)libpng-1.2.1/png.o \
	$(CORSRC)libpng-1.2.1/pngerror.o \
	$(CORSRC)libpng-1.2.1/pnggccrd.o \
	$(CORSRC)libpng-1.2.1/pngget.o \
	$(CORSRC)libpng-1.2.1/pngmem.o \
	$(CORSRC)libpng-1.2.1/pngpread.o \
	$(CORSRC)libpng-1.2.1/pngread.o \
	$(CORSRC)libpng-1.2.1/pngrio.o \
	$(CORSRC)libpng-1.2.1/pngrtran.o \
	$(CORSRC)libpng-1.2.1/pngrutil.o \
	$(CORSRC)libpng-1.2.1/pngset.o \
	$(CORSRC)libpng-1.2.1/pngtest.o \
	$(CORSRC)libpng-1.2.1/pngtrans.o \
	$(CORSRC)libpng-1.2.1/pngvcrd.o \
	$(CORSRC)libpng-1.2.1/pngwio.o \
	$(CORSRC)libpng-1.2.1/pngwrite.o \
	$(CORSRC)libpng-1.2.1/pngwtran.o \
	$(CORSRC)libpng-1.2.1/pngwutil.o
	
LIBJPEGOBJS=	\
	$(CORSRC)jpeg-6b/jcapimin.o \
	$(CORSRC)jpeg-6b/jcapistd.o \
	$(CORSRC)jpeg-6b/jccoefct.o \
	$(CORSRC)jpeg-6b/jccolor.o \
	$(CORSRC)jpeg-6b/jcdctmgr.o \
	$(CORSRC)jpeg-6b/jchuff.o \
	$(CORSRC)jpeg-6b/jcinit.o \
	$(CORSRC)jpeg-6b/jcmainct.o \
	$(CORSRC)jpeg-6b/jcmarker.o \
	$(CORSRC)jpeg-6b/jcmaster.o \
	$(CORSRC)jpeg-6b/jcomapi.o \
	$(CORSRC)jpeg-6b/jcparam.o \
	$(CORSRC)jpeg-6b/jcphuff.o \
	$(CORSRC)jpeg-6b/jcprepct.o \
	$(CORSRC)jpeg-6b/jcsample.o \
	$(CORSRC)jpeg-6b/jctrans.o \
	$(CORSRC)jpeg-6b/jdapimin.o \
	$(CORSRC)jpeg-6b/jdapistd.o \
	$(CORSRC)jpeg-6b/jdatadst.o \
	$(CORSRC)jpeg-6b/jdatasrc.o \
	$(CORSRC)jpeg-6b/jdcoefct.o \
	$(CORSRC)jpeg-6b/jdcolor.o \
	$(CORSRC)jpeg-6b/jddctmgr.o \
	$(CORSRC)jpeg-6b/jdhuff.o \
	$(CORSRC)jpeg-6b/jdinput.o \
	$(CORSRC)jpeg-6b/jdmainct.o \
	$(CORSRC)jpeg-6b/jdmarker.o \
	$(CORSRC)jpeg-6b/jdmaster.o \
	$(CORSRC)jpeg-6b/jdmerge.o \
	$(CORSRC)jpeg-6b/jdphuff.o \
	$(CORSRC)jpeg-6b/jdpostct.o \
	$(CORSRC)jpeg-6b/jdsample.o \
	$(CORSRC)jpeg-6b/jdtrans.o \
	$(CORSRC)jpeg-6b/jerror.o \
	$(CORSRC)jpeg-6b/jfdctflt.o \
	$(CORSRC)jpeg-6b/jfdctfst.o \
	$(CORSRC)jpeg-6b/jfdctint.o \
	$(CORSRC)jpeg-6b/jidctflt.o \
	$(CORSRC)jpeg-6b/jidctfst.o \
	$(CORSRC)jpeg-6b/jidctint.o \
	$(CORSRC)jpeg-6b/jidctred.o \
	$(CORSRC)jpeg-6b/jmemmgr.o \
	$(CORSRC)jpeg-6b/jmemnobs.o \
	$(CORSRC)jpeg-6b/jquant1.o \
	$(CORSRC)jpeg-6b/jquant2.o \
	$(CORSRC)jpeg-6b/jutils.o
	
	

verge3: libzlib.a libcorona.a liblua.a $(OBJS)
	g++ $(CXXFLAGS) $(OBJS) -o verge3 -L$(LIBDIR) $(LDFLAGS)

libzlib.a: $(ZLIBOBJS)
	$(AR) r $@ $(ZLIBOBJS)
	$(RANLIB) $@

libcorona.a: $(COROBJS) $(LIBJPEGOBJS) $(LIBPNGOBJS) $(LIBUNGIFOBJS)
	$(AR) r $@  $(COROBJS) $(LIBJPEGOBJS) $(LIBPNGOBJS) $(LIBUNGIFOBJS)
	$(RANLIB) $@ 
liblua.a: $(LUAOBJS)
	$(AR) r $@  $(LUAOBJS)
	$(RANLIB) $@

clean:
	rm libzlib.a libcorona.a liblua.a verge3 $(OBJS) $(COROBJS) $(LIBJPEGOBJS) $(LIBPNGOBJS) $(LIBUNGIFOBJS)

INCDIR = $(ZLIBSRC) $(V3SRC) $(CORSRC)jpeg-6b $(CORSRC)libungif-4.1.0 $(CORSRC)libpng-1.2.1 $(CORSRC)
SDL_CFLAGS = $(shell sdl-config --cflags)
SDL_LDFLAGS = $(shell sdl-config --libs)
CFLAGS = $(SDL_CFLAGS) -O3 -Wall
LIBDIR = .
LDFLAGS = -lm -lc -lcorona -lzlib -llua -lluabind -lfmod-3.75 $(SDL_LDFLAGS)
CXXFLAGS = $(CFLAGS) -D__LINUX__ -I$(ZLIBSRC) -I$(CORSRC) -I$(CORSRC)jpeg-6b -I$(CORSRC)libungif-4.1.0/ -I$(CORSRC)libpng-1.2.1/ -I../boost-1.37.0/ -I../sdl/include/ -I../fmod/ -I$(LUASRC)
ASFLAGS = $(CFLAGS)

