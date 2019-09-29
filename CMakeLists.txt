## This CMake file is a test. It's pretty thrown together and may not work well.
##   In fact, it just exits right now to make sure you don't think it'll do stuff.

cmake_minimum_required (VERSION 3.1)
if(POLICY CMP0076)
    cmake_policy(SET CMP0076 NEW)
endif()

project (DunGen)

set(CMAKE_C_STANDARD 11)

if (NOT DEFINED ENV{EMSCRIPTEN})
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(OpenGL_GL_PREFERENCE GLVND)
    add_subdirectory(lib/glfw-3.3.0)
    find_package(OpenGL REQUIRED)
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

include_directories(lib/single-file)

include_directories(src)
add_compile_definitions(GL_SILENCE_DEPRECATION)
add_executable (DunGen MACOSX_BUNDLE
    src/dungen.c
    src/constraints/pathfind.c
    src/hlvm/hlvm.c
    src/infrastructure/attributes.c
    src/infrastructure/game.c
    src/infrastructure/images.c
    src/infrastructure/outline.c
    src/infrastructure/rendering.c
    src/infrastructure/text.c
    src/infrastructure/util.c
    src/infrastructure/world.c
    src/scripting/scripting.c
    src/scripting/wrapping.c
    src/ui/banner.c
    src/ui/choice.c
    src/ui/tile_choice.c
)

if (DEFINED ENV{EMSCRIPTEN})
    target_sources(DunGen PUBLIC src/platform/wasm.c)
    target_sources(DunGen PUBLIC platform/WebAssembly/main.c)
    set_target_properties(DunGen
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS "--emrun -s ALLOW_MEMORY_GROWTH=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s WASM=1 -s --preload-file ../Resources"
    )

    target_link_libraries(DunGen
        # glfw OpenGL::GL
        lua
        sqlite
        sds
        pqueue
        pcg
    )

else()
    target_sources(DunGen PUBLIC src/main.c)

    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        target_sources(DunGen PUBLIC src/platform/macOS.c)
        set(ResourceDir "/../Resources")
        set_target_properties(DunGen PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/macOS/Info.plist)

    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_sources(DunGen PUBLIC src/platform/windows.c)
        # ???

    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_sources(DunGen PUBLIC src/platform/linux.c)
        set(ResourceDir "/Resources")
    endif()

    target_link_libraries(DunGen
        glfw OpenGL::GL
        lua
        sqlite
        sds
        pqueue
        pcg
    )
endif()


find_program(SWIG_EXE "swig")
if(SWIG_EXE)
    add_custom_command(TARGET DunGen PRE_BUILD
                       COMMAND ${SWIG_EXE} -lua -Werror -o ${CMAKE_SOURCE_DIR}/src/scripting/wrapping.c ${CMAKE_SOURCE_DIR}/src/scripting/interfaces/dungen.i
                      )
endif()

add_custom_command(TARGET DunGen POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/Resources $<TARGET_FILE_DIR:DunGen>/${ResourceDir}
                  )

