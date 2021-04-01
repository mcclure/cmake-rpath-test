# This project is a minimal chunk of lovr.org to demonstrate an issue with CMake. The lovr license applies
cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0063 NEW)
project(lovr)

if (NOT APPLE)
  message(FATAL_ERROR "This CMakeLists contains Mac build instructions only.")
endif()

# Options
option(LOVR_USE_STEAM_AUDIO "Enable the Steam Audio spatializer (be sure to also set LOVR_STEAM_AUDIO_PATH)" OFF)

option(LOVR_BUILD_EXE "Build an executable (or an apk on Android)" ON)
option(LOVR_BUILD_SHARED "Build a shared library (takes precedence over LOVR_BUILD_EXE)" OFF)
option(LOVR_BUILD_BUNDLE "On macOS, build a .app bundle instead of a raw program" OFF)

# Setup

# Steam Audio (aka Phonon)
if(LOVR_USE_STEAM_AUDIO)
  if(NOT LOVR_STEAM_AUDIO_PATH)
    message(FATAL_ERROR "LOVR_USE_STEAM_AUDIO requires the LOVR_STEAM_AUDIO_PATH to be set to the location of the Steam Audio folder")
  endif()
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "LOVR_USE_STEAM_AUDIO is not currently available on 32 bit builds")
  endif()
  include_directories("${LOVR_STEAM_AUDIO_PATH}/include")
  add_library(Phonon SHARED IMPORTED)

  # Apple case
  set_target_properties(Phonon PROPERTIES IMPORTED_LOCATION "${LOVR_STEAM_AUDIO_PATH}/lib/OSX/libphonon.dylib")

  set(LOVR_PHONON Phonon)
endif()

# LÖVR

set(LOVR_SRC
  main.c
)

if(LOVR_BUILD_SHARED)
  add_library(lovr SHARED ${LOVR_SRC})
elseif(LOVR_BUILD_EXE)
  add_executable(lovr ${LOVR_SRC})
else()
  return()
endif()

set_target_properties(lovr PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(lovr PROPERTIES C_STANDARD 99)
target_link_libraries(lovr ${LOVR_PHONON})

# Apple platform
if(LOVR_BUILD_BUNDLE)
  function(move_lib)
    if(TARGET ${ARGV0})
      add_custom_command(TARGET lovr POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_SONAME_FILE:${ARGV0}>
        ${CMAKE_CURRENT_BINARY_DIR}/lovr.app/Contents/MacOS/$<TARGET_SONAME_FILE_NAME:${ARGV0}> # Bad
      )
    endif()
  endfunction()

  move_lib(${LOVR_OCULUS_AUDIO})
  move_lib(${LOVR_PHONON})

  target_sources(lovr PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/lovr.icns")
  set_target_properties(lovr PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_RPATH TRUE
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH "@executable_path"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/Info.plist"
    RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/lovr.icns"
  )
endif()

