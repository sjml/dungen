project('DunGen', 'c',
    license: 'MIT',
    default_options: ['c_std=c11', 'warning_level=0']
)

iphone_check = '''#include <TargetConditionals.h>
#if !TARGET_OS_IPHONE
#error not iphone
#endif'''

cc = meson.get_compiler('c')
host_machine_system = host_machine.system()
if host_machine_system == 'darwin'
    if cc.compiles(iphone_check, name: 'iOS Target Check')
        host_machine_system = 'ios'
    else
        host_machine_system = 'macos'
    endif
endif


if host_machine_system == 'macos' and not get_option('debug')
    add_global_arguments(['-arch', 'arm64', '-arch', 'x86_64'], language: 'c')
    add_global_link_arguments(['-arch', 'arm64', '-arch', 'x86_64'], language: 'c')
endif

if get_option('debug')
    add_global_arguments(['-D_DEBUG', '-DDEBUG'], language: 'c')
endif

# By default, don't show any warnings; we'll enable them for our client code.
#   (If there are warnings in library code, that's not something we're gonna fix.)
lib_args = []
exe_args = []
if build_machine.system() == 'windows'
    lib_args += '/W0' # no warnings

    exe_args += [
        '/W4', '/WX', # all warnings, and treat as errors
        '/external:anglebrackets', '/external:W0', # don't warn for any files imported with angle brackets (external)
    ]
else
    lib_args += '-w' # no warnings

    exe_args += [
        '-Wall', '-Wextra', '-Wpedantic', '-Werror', # all warnings, and treat as errors
        '-Wno-missing-braces', # turn off missing braces warning; missing initializers catches real problems
    ]
endif


if build_machine.system() == 'windows'
    exe_args += ['-D_CRT_SECURE_NO_WARNINGS']
elif build_machine.system() == 'linux'
    exe_args += ['-D_GNU_SOURCE']
    lib_args += ['-DLUA_USE_LINUX']
endif



dungen_inc_dirs = []

lua_srcs = [
    'lib/lua-5.3.5+system_patch/src/lapi.c',
    'lib/lua-5.3.5+system_patch/src/lauxlib.c',
    'lib/lua-5.3.5+system_patch/src/lbaselib.c',
    'lib/lua-5.3.5+system_patch/src/lbitlib.c',
    'lib/lua-5.3.5+system_patch/src/lcode.c',
    'lib/lua-5.3.5+system_patch/src/lcorolib.c',
    'lib/lua-5.3.5+system_patch/src/lctype.c',
    'lib/lua-5.3.5+system_patch/src/ldblib.c',
    'lib/lua-5.3.5+system_patch/src/ldebug.c',
    'lib/lua-5.3.5+system_patch/src/ldo.c',
    'lib/lua-5.3.5+system_patch/src/ldump.c',
    'lib/lua-5.3.5+system_patch/src/lfunc.c',
    'lib/lua-5.3.5+system_patch/src/lgc.c',
    'lib/lua-5.3.5+system_patch/src/linit.c',
    'lib/lua-5.3.5+system_patch/src/liolib.c',
    'lib/lua-5.3.5+system_patch/src/llex.c',
    'lib/lua-5.3.5+system_patch/src/lmathlib.c',
    'lib/lua-5.3.5+system_patch/src/lmem.c',
    'lib/lua-5.3.5+system_patch/src/loadlib.c',
    'lib/lua-5.3.5+system_patch/src/lobject.c',
    'lib/lua-5.3.5+system_patch/src/lopcodes.c',
    'lib/lua-5.3.5+system_patch/src/loslib.c',
    'lib/lua-5.3.5+system_patch/src/lparser.c',
    'lib/lua-5.3.5+system_patch/src/lstate.c',
    'lib/lua-5.3.5+system_patch/src/lstring.c',
    'lib/lua-5.3.5+system_patch/src/lstrlib.c',
    'lib/lua-5.3.5+system_patch/src/ltable.c',
    'lib/lua-5.3.5+system_patch/src/ltablib.c',
    'lib/lua-5.3.5+system_patch/src/ltm.c',
    'lib/lua-5.3.5+system_patch/src/lundump.c',
    'lib/lua-5.3.5+system_patch/src/lutf8lib.c',
    'lib/lua-5.3.5+system_patch/src/lvm.c',
    'lib/lua-5.3.5+system_patch/src/lzio.c',

    'lib/luafilesystem-1.7.0-2/src/lfs.c'
]
lua_inc_dirs = [
    'lib/lua-5.3.5+system_patch/src',
    'lib/luafilesystem-1.7.0-2/src'
]
dungen_inc_dirs += lua_inc_dirs
lua = static_library('lua',
    sources: lua_srcs,
    include_directories: lua_inc_dirs,
    override_options: ['c_std=none'], # I don't like this, but Ubuntu doesn't like compiling Lua with C11?
    c_args: lib_args
)

sds_inc_dirs = ['lib/sds-0bb446e']
dungen_inc_dirs += sds_inc_dirs
sds = static_library('sds',
    sources: 'lib/sds-0bb446e/sds.c',
    include_directories: sds_inc_dirs,
    c_args: lib_args
)

sqlite_inc_dirs = ['lib/sqlite-amalgamation-3290000']
dungen_inc_dirs += sqlite_inc_dirs
sqlite = static_library('sqlite',
    sources: 'lib/sqlite-amalgamation-3290000/sqlite3.c',
    include_directories: sqlite_inc_dirs,
    c_args: lib_args
)

pqueue_inc_dirs = ['lib/libpqueue-f319ea0/src']
dungen_inc_dirs += pqueue_inc_dirs
pqueue = static_library('pqueue',
    sources: 'lib/libpqueue-f319ea0/src/pqueue.c',
    include_directories: pqueue_inc_dirs,
    c_args: lib_args
)

pcg_inc_dirs = ['lib/pcg-c-basic-0.9']
dungen_inc_dirs += pcg_inc_dirs
pcg = static_library('pcg',
    sources: 'lib/pcg-c-basic-0.9/pcg_basic.c',
    include_directories: pcg_inc_dirs,
    c_args: lib_args
)

dungen_srcs = [
    'src/main.c',
    'src/dungen.c',
    'src/constraints/pathfind.c',
    'src/hlvm/hlvm.c',
    'src/infrastructure/attributes.c',
    'src/infrastructure/game.c',
    'src/infrastructure/images.c',
    'src/infrastructure/outline.c',
    'src/infrastructure/rendering.c',
    'src/infrastructure/text.c',
    'src/infrastructure/util.c',
    'src/infrastructure/world.c',
    'src/scripting/scripting.c',
    'src/scripting/wrapping.c',
    'src/ui/banner.c',
    'src/ui/choice.c',
    'src/ui/tile_choice.c'
]

if host_machine_system == 'macos'
    dungen_srcs += 'src/platform/macOS.c'
elif host_machine_system == 'ios'
    dungen_srcs += 'src/platform/iOS.c'
elif host_machine_system == 'linux'
    dungen_srcs += 'src/platform/linux.c'
elif host_machine_system == 'windows'
    dungen_srcs += 'src/platform/windows.c'
    dungen_inc_dirs += ['platform/windows/GL_headers']
elif host_machine_system == 'emscripten'
    dungen_srcs += 'src/platform/wasm.c'
endif


sokol_inc_dirs = ['src', 'lib/sokol-3825314']
dungen_inc_dirs += sokol_inc_dirs
dungen_inc_dirs += [
    'lib/single-file'
]


dungen_libs = [lua, sqlite, sds, pqueue, pcg]
if host_machine_system == 'macos' or host_machine_system == 'ios'
    sokol = static_library('sokol',
        'src/infrastructure/sokol_impl.c',
        include_directories: dungen_inc_dirs,
        c_args: ['-x', 'objective-c']
    )
    dungen_libs += sokol
elif host_machine_system == 'windows'
    dungen_srcs += [
        'src/infrastructure/sokol_impl.c',
        'platform/windows/GL_headers/glad.c'
    ]
elif host_machine_system == 'linux'
    dungen_srcs += [
        'src/infrastructure/sokol_impl.c'
    ]
elif host_machine_system == 'emscripten'
    dungen_srcs += [
        'src/infrastructure/sokol_impl.c'
    ]
endif


dungen_deps = []
if host_machine_system == 'macos'
    dungen_deps = dependency(
        'appleframeworks',
        modules: [
            'cocoa',
            'quartzcore',
            'metal',
            'metalkit',
            'opengl'
        ]
    )
elif host_machine_system == 'ios'
    dungen_deps = dependency(
        'appleframeworks',
        modules: [
            'foundation',
            'uikit',
            'metal',
            'metalkit',
            'opengles',
            'glkit'
        ]
    )
elif host_machine_system == 'windows'
    dungen_deps = dependency('gl')
    dungen_srcs += import('windows').compile_resources('platform/windows/DunGen/DunGen.rc')
elif host_machine_system == 'linux'
    dungen_deps = [
        dependency('threads'),
        dependency('gl'),
        cc.find_library('m', required : false),
        cc.find_library('dl'),
        cc.find_library('X11'),
        cc.find_library('Xi'),
        cc.find_library('Xcursor')
    ]
elif host_machine_system == 'emscripten'

endif

executable('DunGen',
    sources: dungen_srcs,
    include_directories: include_directories(dungen_inc_dirs, is_system: true),
    link_with: dungen_libs,
    dependencies: dungen_deps,
    install: true,
    c_args: exe_args
)

if host_machine_system == 'macos'
    install_data('platform/macOS/Info.plist', install_dir: 'Contents')
    install_subdir('Resources', install_dir: 'Contents')
    meson.add_install_script('buildscripts/compileIcons.sh', ['mac'])
elif host_machine_system == 'ios'
    install_data('platform/iOS/Info.plist', install_dir: './')
    install_subdir('Resources', install_dir: 'DunGenResources', strip_directory: true)
    meson.add_install_script('buildscripts/compileIcons.sh', ['ios'])
elif host_machine_system == 'windows' or host_machine_system == 'linux'
    install_subdir('Resources', install_dir: '')
elif host_machine_system == 'emscripten'
    # this if statement is so the configuration can run before the files
    #   exist; we'll do a reconfigure before install
    if import('fs').exists(meson.current_build_dir() + '/DunGen.data')
        install_data(meson.current_build_dir() + '/DunGen.data', install_dir: '.')
        install_data(meson.current_build_dir() + '/DunGen.html', install_dir: '.')
    endif
endif
