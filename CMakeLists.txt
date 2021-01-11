## This CMake file is pretty thrown together and may not work well.
##    Only *used* for Linux and Emscripten builds; other platforms are
##    in here only to verify the build process, but no guarantee
##    they actually work.

cmake_minimum_required (VERSION 3.1)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

project (DunGen)

set(CMAKE_C_STANDARD 11)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -arch arm64 -arch x86_64")
endif()

add_library(lua
    lib/lua-5.3.5+system_patch/src/lapi.c
    lib/lua-5.3.5+system_patch/src/lauxlib.c
    lib/lua-5.3.5+system_patch/src/lbaselib.c
    lib/lua-5.3.5+system_patch/src/lbitlib.c
    lib/lua-5.3.5+system_patch/src/lcode.c
    lib/lua-5.3.5+system_patch/src/lcorolib.c
    lib/lua-5.3.5+system_patch/src/lctype.c
    lib/lua-5.3.5+system_patch/src/ldblib.c
    lib/lua-5.3.5+system_patch/src/ldebug.c
    lib/lua-5.3.5+system_patch/src/ldo.c
    lib/lua-5.3.5+system_patch/src/ldump.c
    lib/lua-5.3.5+system_patch/src/lfunc.c
    lib/lua-5.3.5+system_patch/src/lgc.c
    lib/lua-5.3.5+system_patch/src/linit.c
    lib/lua-5.3.5+system_patch/src/liolib.c
    lib/lua-5.3.5+system_patch/src/llex.c
    lib/lua-5.3.5+system_patch/src/lmathlib.c
    lib/lua-5.3.5+system_patch/src/lmem.c
    lib/lua-5.3.5+system_patch/src/loadlib.c
    lib/lua-5.3.5+system_patch/src/lobject.c
    lib/lua-5.3.5+system_patch/src/lopcodes.c
    lib/lua-5.3.5+system_patch/src/loslib.c
    lib/lua-5.3.5+system_patch/src/lparser.c
    lib/lua-5.3.5+system_patch/src/lstate.c
    lib/lua-5.3.5+system_patch/src/lstring.c
    lib/lua-5.3.5+system_patch/src/lstrlib.c
    lib/lua-5.3.5+system_patch/src/ltable.c
    lib/lua-5.3.5+system_patch/src/ltablib.c
    lib/lua-5.3.5+system_patch/src/ltm.c
    lib/lua-5.3.5+system_patch/src/luac.c
    lib/lua-5.3.5+system_patch/src/lundump.c
    lib/lua-5.3.5+system_patch/src/lutf8lib.c
    lib/lua-5.3.5+system_patch/src/lvm.c
    lib/lua-5.3.5+system_patch/src/lzio.c

    lib/luafilesystem-1.7.0-2/src/lfs.c
)
include_directories(lib/lua-5.3.5+system_patch/src)
include_directories(lib/luafilesystem-1.7.0-2/src)

add_library(sds lib/sds-0bb446e/sds.c)
include_directories(lib/sds-0bb446e)

add_library(sqlite lib/sqlite-amalgamation-3290000/sqlite3.c)
include_directories(lib/sqlite-amalgamation-3290000)

add_library(pqueue lib/libpqueue-f319ea0/src/pqueue.c)
include_directories(lib/libpqueue-f319ea0/src)

add_library(pcg lib/pcg-c-basic-0.9/pcg_basic.c)
include_directories(lib/pcg-c-basic-0.9)

include_directories(lib/sokol-5894182d)
include_directories(lib/single-file)

include_directories(src)
add_executable (DunGen MACOSX_BUNDLE
    src/main.c
    src/dungen.c
    src/constraints/pathfind.c
    src/hlvm/hlvm.c
    src/infrastructure/attributes.c
    src/infrastructure/game.c
    src/infrastructure/images.c
    src/infrastructure/outline.c
    src/infrastructure/rendering.c
    src/infrastructure/sokol_impl.c
    src/infrastructure/text.c
    src/infrastructure/util.c
    src/infrastructure/world.c
    src/scripting/scripting.c
    src/scripting/wrapping.c
    src/ui/banner.c
    src/ui/choice.c
    src/ui/tile_choice.c
)
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_source_files_properties(
        src/infrastructure/sokol_impl.c
        PROPERTIES
            COMPILE_FLAGS "-x objective-c"
    )
endif()

target_link_libraries(DunGen
    lua
    sqlite
    sds
    pqueue
    pcg
)

if (DEFINED ENV{EMSCRIPTEN})
    target_sources(DunGen PUBLIC src/platform/wasm.c)
    set_target_properties(DunGen
        PROPERTIES
            SUFFIX ".html"
            LINK_FLAGS "-s WASM=1 -s --preload-file ../Resources -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s ASSERTIONS=0 -s ALLOW_MEMORY_GROWTH=1 -s MALLOC=emmalloc --closure=1"
    )

    target_link_options(DunGen
        PRIVATE --shell-file ../platform/WebAssembly/DunGen.html.template
    )

else()
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        target_sources(DunGen PUBLIC src/platform/macOS.c)
        set(ResourceDir "/../Resources")
        set_target_properties(DunGen
            PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/macOS/Info.plist
        )

        target_link_libraries(DunGen
            "-framework Cocoa"
            "-framework QuartzCore"
            "-framework Metal"
            "-framework MetalKit"
            "-framework OpenGL"
        )

    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_sources(DunGen PUBLIC src/platform/linux.c)
        set(ResourceDir "/Resources")

        find_package(OpenGL REQUIRED)
        find_package(X11 REQUIRED)
        target_link_libraries(DunGen
            OpenGL::GL
            X11
            Xi
            Xcursor
            m
            dl
            pthread
        )
    endif()
endif()


find_program(SWIG_EXE "swig")
if(SWIG_EXE)
    add_custom_command(TARGET DunGen PRE_BUILD
                       COMMAND ${SWIG_EXE} -lua -Werror -o ${CMAKE_SOURCE_DIR}/src/scripting/wrapping.c ${CMAKE_SOURCE_DIR}/src/scripting/interfaces/dungen.i
                      )
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_custom_command(TARGET DunGen POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:DunGen>/${ResourceDir}
                  )
endif()

