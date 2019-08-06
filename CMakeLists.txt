## This CMake file is a test. It's pretty thrown together and may not work well.
##   In fact, it just exits right now to make sure you don't think it'll do stuff.
message(FATAL_ERROR "This CMake file isn't in use at the moment, but could be a reference. Comment out this line to give it a try.")

cmake_minimum_required (VERSION 3.0)
project (DunGen)

set(CMAKE_C_STANDARD 11)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(lib/glfw-3.3.0)
find_package(OpenGL REQUIRED)

# Nothing against these libraries; we're just not using them
#  in the Xcode build so for the sake of consistency preventing
#  them getting found by CMake if they're installed otherwise.
set(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_BZip2 TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
set(CMAKE_DISABLE_FIND_PACKAGE_PNG TRUE)
add_subdirectory(lib/freetype-2.10.1 freetype)
include_directories(lib/freetype-2.10.1/include)

add_library(lua
    lib/lua-5.3.5/src/lapi.c
    lib/lua-5.3.5/src/lauxlib.c
    lib/lua-5.3.5/src/lbaselib.c
    lib/lua-5.3.5/src/lbitlib.c
    lib/lua-5.3.5/src/lcode.c
    lib/lua-5.3.5/src/lcorolib.c
    lib/lua-5.3.5/src/lctype.c
    lib/lua-5.3.5/src/ldblib.c
    lib/lua-5.3.5/src/ldebug.c
    lib/lua-5.3.5/src/ldo.c
    lib/lua-5.3.5/src/ldump.c
    lib/lua-5.3.5/src/lfunc.c
    lib/lua-5.3.5/src/lgc.c
    lib/lua-5.3.5/src/linit.c
    lib/lua-5.3.5/src/liolib.c
    lib/lua-5.3.5/src/llex.c
    lib/lua-5.3.5/src/lmathlib.c
    lib/lua-5.3.5/src/lmem.c
    lib/lua-5.3.5/src/loadlib.c
    lib/lua-5.3.5/src/lobject.c
    lib/lua-5.3.5/src/lopcodes.c
    lib/lua-5.3.5/src/loslib.c
    lib/lua-5.3.5/src/lparser.c
    lib/lua-5.3.5/src/lstate.c
    lib/lua-5.3.5/src/lstring.c
    lib/lua-5.3.5/src/lstrlib.c
    lib/lua-5.3.5/src/ltable.c
    lib/lua-5.3.5/src/ltablib.c
    lib/lua-5.3.5/src/ltm.c
    lib/lua-5.3.5/src/lua.c
    lib/lua-5.3.5/src/luac.c
    lib/lua-5.3.5/src/lundump.c
    lib/lua-5.3.5/src/lutf8lib.c
    lib/lua-5.3.5/src/lvm.c
    lib/lua-5.3.5/src/lzio.c

    lib/luafilesystem-1.7.0-2/src/lfs.c
)
include_directories(lib/lua-5.3.5/src)
include_directories(lib/luafilesystem-1.7.0-2/src)

add_library(sds lib/sds-0bb446e/sds.c)
include_directories(lib/sds-0bb446e)

add_library(sqlite lib/sqlite-amalgamation-3290000/sqlite3.c)
include_directories(lib/sqlite-amalgamation-3290000)

add_library(pqueue lib/libpqueue-f319ea0/src/pqueue.c)
include_directories(lib/libpqueue-f319ea0/src)

include_directories(lib/single-file)

add_compile_definitions(GL_SILENCE_DEPRECATION)
add_executable (DunGen MACOSX_BUNDLE
    src/main.c
    src/scripting/scripting.c
    src/scripting/wrapping.c
    src/infrastructure/attributes.c
    src/infrastructure/game.c
    src/infrastructure/images.c
    src/infrastructure/outline.c
    src/infrastructure/rendering.c
    src/infrastructure/text.c
    src/infrastructure/util.c
    src/infrastructure/world.c
    src/hlvm/hlvm.c
    src/constraints/pathfind.c
)

set_target_properties(DunGen PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/macOS/Info.plist)

target_link_libraries(DunGen
    glfw OpenGL::GL
    freetype
    lua
    sqlite
    sds
    pqueue
)

find_program(SWIG_EXE "swig")
if(SWIG_EXE)
    add_custom_command(TARGET DunGen PRE_BUILD
                       COMMAND ${SWIG_EXE} -lua -Werror -I${CMAKE_SOURCE_DIR}/lib/swiglib -o ${CMAKE_SOURCE_DIR}/src/scripting/wrapping.c ${CMAKE_SOURCE_DIR}/src/scripting/interfaces/dungen.i
                      )
endif()

add_custom_command(TARGET DunGen POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/images $<TARGET_FILE_DIR:DunGen>/../Resources/images
                  )
add_custom_command(TARGET DunGen POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/scripts $<TARGET_FILE_DIR:DunGen>/../Resources/scripts
                  )
add_custom_command(TARGET DunGen POST_BUILD
                  COMMAND ${CMAKE_COMMAND} -E copy_directory
                  ${CMAKE_SOURCE_DIR}/fonts $<TARGET_FILE_DIR:DunGen>/../Resources/fonts
                  )
