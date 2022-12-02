import os
import os.path
import sys

# Find emcc + others
path = os.environ['PATH'].split(os.path.pathsep)

def locate_command(path, command):
    for p in path:
        p2 = os.path.join(p, command)
        if os.path.exists(p2):
            return p2

    print('Could not find ' + command +'.  Check your PATH?')
    sys.exit(1)

emcc = locate_command(path, 'emcc')
emar = locate_command(path, 'emar')
emranlib = locate_command(path, 'emranlib')

def EmscriptenEnvironment():
    if sys.platform in ('windows', 'win32'):
        env_dict = {
            'path': os.environ['PATH']
        }

        for key in ['HOME', 'USERPROFILE', 'EM_CONFIG']:
            value = os.environ.get(key)
            if value is not None:
                env_dict[key] = value

        env = Environment(ENV=env_dict, TOOLS=['mingw'])

        # ar on Windows omits ranlib, we need to put it back.
        env['ARCOM'] = '$AR $ARFLAGS $TARGET $SOURCES\n$RANLIB $RANLIBFLAGS $TARGET'
    else:
        env = Environment()

    env['AR'] = emar
    env['RANLIB'] = emranlib
    env['CC'] = emcc
    env['CXX'] = emcc

    emscriptenOpts = [
        '-s', 'ASYNCIFY',
        '-s', 'ASYNCIFY_IMPORTS=["fetchSync","downloadAll","wasm_nextFrame","emscripten_sleep"]',
        '-s', 'FETCH=1',
        '-s', 'FORCE_FILESYSTEM=1',
        '-s', 'ALLOW_MEMORY_GROWTH=1',
        '-s', 'USE_SDL=1',
        '-s', 'LLD_REPORT_UNDEFINED'
    ]

    cflags = ['-fcolor-diagnostics'] #'-fno-rtti', '-fno-exceptions']

    asmjs = ARGUMENTS.get('asmjs', 0)
    debug = int(ARGUMENTS.get('debug', 0))
    asan = ARGUMENTS.get('asan', 0)
    ubsan = ARGUMENTS.get('ubsan', 0)
    cyberdwarf = ARGUMENTS.get('cyberdwarf', 0)

    debug_flags = {
        'debug_functions',
        'debug_locals',
        'debug_input',
        'debug_vc',
        'profile_vc'
    }
    
    for flag in debug_flags:
        if ARGUMENTS.get(flag, 0) > 0:
            env.Append(CPPDEFINES=[
                flag.upper()
            ])

    if debug:
        if not asan:
            emscriptenOpts += [
                '-s', 'SAFE_HEAP=1',
            ]

        emscriptenOpts += [
            '-s', 'ASYNCIFY_STACK_SIZE=327680',
            '-s', 'ASSERTIONS=1',
            '-s', 'STACK_OVERFLOW_CHECK=1',
            '-s', 'DEMANGLE_SUPPORT=1',
        ]

        cflags.append('-g')

        env.Append(LINKFLAGS=[
            '-g4',
            '--source-map-base', 'http://localhost:8000/',
        ])

    else:
        emscriptenOpts += [
            '-s', 'ASYNCIFY_STACK_SIZE=32768',
        ]
        cflags.append('-O3')
        # cflags.append('-flto')
        # env.Append(LINKFLAGS=['-flto'])

    if cyberdwarf:
        env.Append(LINKFLAGS=[
            '-s', 'CYBERDWARF=1'
        ])

    if asmjs:
        env.Append(LINKFLAGS=[
            '-s', 'WASM=0',
        ])

    if asan:
        cflags.append('-fsanitize=address')
        env.Append(LINKFLAGS=['-fsanitize=address'])
    if ubsan:
        cflags.append('-fsanitize=undefined')
        env.Append(LINKFLAGS=['-fsanitize=undefined'])

    cflags.extend([
        '-MMD',
        '-Wno-parentheses',
        '-Wno-long-long',
        '-Wno-dangling-else',
        '-Wno-writable-strings',
    ])

    cflags.extend(emscriptenOpts)

    env.Append(CFLAGS=cflags)
    env.Append(CXXFLAGS=cflags)

    env.Append(LINKFLAGS=[
        '-lidbfs.js',
    ] + emscriptenOpts)

    return env

verge_dir = 'verge/Source'
verge_sources = [verge_dir + '/' + s for s in [
    'algebra3.cpp',
    'a_common.cpp',
    'a_dict.cpp',
    'a_vfile.cpp',
    'a_codec.cpp',
    'a_config.cpp',
    'a_handle.cpp',
    'a_image.cpp',
    'a_string.cpp',
    'g_script.cpp',
    'g_startup.cpp',
    'g_chr.cpp',
    'g_font.cpp',
    'g_controls.cpp',
    'g_engine.cpp',
    'g_editcode.cpp',
    'g_sound.cpp',
    'snd_fmod.cpp',
    'g_entity.cpp',
    'g_sprites.cpp',
    'g_map.cpp',
    'g_vsp.cpp',
    'garlick.cpp',
    'lua_main.cpp',
    'lua_vector.cpp',
    'vc_builtins.cpp',
    'vc_core.cpp',
    'vc_compiler.cpp',
    'vc_library.cpp',
    'vc_debug.cpp',
    'vid_manager.cpp',
    'vid_sysfont.cpp',
    'vid_ddblit.cpp',
    'vid_timeless.cpp',
    'vid_fbfx.cpp',
    'vid_macbase.cpp',
    'mac_joystick.cpp',
    'mac_network.cpp',
    'mac_keyboard.cpp',
    'mac_mouse.cpp',
    'mac_system.cpp',
    'mac_timer.cpp',
    'mac_movie.cpp',
    'xerxes.cpp',
]]

# TODO: make this optional and have separate Lua and non-Lua builds? (for VC games that don't need it)
lua_dir = 'lua/lua-5.1.2'
lua_sources = [lua_dir + '/' + s for s in [
    'lapi.c',
    'lauxlib.c',
    'lbaselib.c',
    'lcode.c',
    'ldblib.c',
    'ldebug.c',
    'ldo.c',
    'ldump.c',
    'lfunc.c',
    'lgc.c',
    'linit.c',
    'liolib.c',
    'llex.c',
    'lmathlib.c',
    'lmem.c',
    'loadlib.c',
    'lobject.c',
    'lopcodes.c',
    'loslib.c',
    'lparser.c',
    'lstate.c',
    'lstring.c',
    'lstrlib.c',
    'ltable.c',
    'ltablib.c',
    'ltm.c',
    'lundump.c',
    'lvm.c',
    'lzio.c',
    'print.c',
]]

# TODO: could this be replaced with SDL_Image?
corona_dir = 'corona'
corona_sources = [corona_dir + '/' + s for s in [
    'Corona.cpp',
    'Convert.cpp',
    'DefaultFileSystem.cpp',
    'MemoryFile.cpp',
    'OpenBMP.cpp',
    'OpenGIF.cpp',
    'OpenPCX.cpp',
    'OpenPNG.cpp',
    'OpenJPEG.cpp',
    'OpenTGA.cpp',
    'SavePNG.cpp',
    'SaveTGA.cpp',
]]

# TODO: -s USE_ZLIB=1
zlib_dir = 'zlib'
zlib_sources = [zlib_dir + '/' + s for s in [
    'adler32.c',
    'compress.c',
    'crc32.c',
    'gzio.c',
    'uncompr.c',
    'deflate.c',
    'trees.c',
    'zutil.c',
    'inflate.c',
    'infblock.c',
    'inftrees.c',
    'infcodes.c',
    'infutil.c',
    'inffast.c',
]]

# TODO: -s USE_GIFLIB=1
libungif_dir = corona_dir + '/libungif-4.1.0'
libungif_sources = [libungif_dir + '/' +  s for s in [
    'dgif_lib.c',
    'gif_err.c',
    'gifalloc.c',
]]

# TODO: -s USE_LIBPNG=1
libpng_dir = corona_dir + '/libpng-1.2.1'
libpng_sources = [libpng_dir + '/' + s for s in [
    'png.c',
    'pngerror.c',
    'pnggccrd.c',
    'pngget.c',
    'pngmem.c',
    'pngpread.c',
    'pngread.c',
    'pngrio.c',
    'pngrtran.c',
    'pngrutil.c',
    'pngset.c',
    'pngtest.c',
    'pngtrans.c',
    'pngvcrd.c',
    'pngwio.c',
    'pngwrite.c',
    'pngwtran.c',
    'pngwutil.c',
]]

# TODO: -s USE_LIBJPEG=1
libjpeg_dir = corona_dir + '/jpeg-6b'
libjpeg_sources = [libjpeg_dir + '/' + s for s in [
    'jcapimin.c',
    'jcapistd.c',
    'jccoefct.c',
    'jccolor.c',
    'jcdctmgr.c',
    'jchuff.c',
    'jcinit.c',
    'jcmainct.c',
    'jcmarker.c',
    'jcmaster.c',
    'jcomapi.c',
    'jcparam.c',
    'jcphuff.c',
    'jcprepct.c',
    'jcsample.c',
    'jctrans.c',
    'jdapimin.c',
    'jdapistd.c',
    'jdatadst.c',
    'jdatasrc.c',
    'jdcoefct.c',
    'jdcolor.c',
    'jddctmgr.c',
    'jdhuff.c',
    'jdinput.c',
    'jdmainct.c',
    'jdmarker.c',
    'jdmaster.c',
    'jdmerge.c',
    'jdphuff.c',
    'jdpostct.c',
    'jdsample.c',
    'jdtrans.c',
    'jerror.c',
    'jfdctflt.c',
    'jfdctfst.c',
    'jfdctint.c',
    'jidctflt.c',
    'jidctfst.c',
    'jidctint.c',
    'jidctred.c',
    'jmemmgr.c',
    'jmemnobs.c',
    'jquant1.c',
    'jquant2.c',
    'jutils.c',
]]

corona_combined_sources = corona_sources + libungif_sources + libpng_sources + libjpeg_sources

boost_dir = 'boost-1.37.0'

verge_cpppath = [
    verge_dir,
    lua_dir,
    corona_dir,
    zlib_dir,
    libungif_dir,
    libpng_dir,
    libjpeg_dir,
    boost_dir
]

# TODO: audio engine. could it use libmodopenmpt + another library for wav/ogg/mp3 instead of Audiere?

verge_libs = [
    'lua',
    'corona',
    'zlib',
]

env = EmscriptenEnvironment()

env.Append(
    CXXFLAGS=[
        '-std=c++17',
        #'-Werror', # Hahaha no way.  This code dates back to like 2001
    ],
    CPPPATH=verge_cpppath
)

lua = env.Library('lua', lua_sources)
corona = env.Library('corona', corona_combined_sources)
zlib = env.Library('zlib', zlib_sources)
verge = env.Program('verge3.out.js', verge_sources, LIBS=verge_libs, LIBPATH='.', PROGSUFFIX='.js')
