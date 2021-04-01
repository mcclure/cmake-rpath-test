# This project is a minimal chunk of lovr.org to demonstrate an issue with CMake. The lovr license applies
cmake_minimum_required(VERSION 3.1.0)
cmake_policy(SET CMP0063 NEW)
project(lovr)

if (NOT APPLE)
  message(FATAL_ERROR "This CMakeLists contains Mac build instructions only.")
endif()

# Options
option(LOVR_USE_STEAM_AUDIO "Enable the Steam Audio spatializer (be sure to also set LOVR_STEAM_AUDIO_PATH)" OFF)
option(LOVR_USE_OCULUS_AUDIO "Enable the Oculus Audio spatializer (be sure to also set LOVR_OCULUS_AUDIO_PATH)" OFF)

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
  # Next line is where it all goes wrong
  set_target_properties(Phonon PROPERTIES IMPORTED_LOCATION "${LOVR_STEAM_AUDIO_PATH}/lib/OSX/libphonon.dylib"
                                          BUILD_RPATH "${LOVR_STEAM_AUDIO_PATH}/lib/OSX")

  set(LOVR_PHONON Phonon)
endif()

# Oculus Audio
if(LOVR_USE_OCULUS_AUDIO)
  if(NOT LOVR_OCULUS_AUDIO_PATH)
    message(FATAL_ERROR "LOVR_USE_OCULUS_AUDIO requires the LOVR_OCULUS_AUDIO_PATH to be set to the location of the Oculus Spatializer Native (AudioSDK) folder")
  endif()

  include_directories("${LOVR_OCULUS_AUDIO_PATH}/Include")

  add_library(OculusAudio SHARED IMPORTED)

  # Apple case
  # Oculus Audio ships with an intel .framework, but (as of 25.0.0) it is broken. We can convert it to a working dylib with install_name_tool in a custom target:
  SET(OCULUS_AUDIO_MAC_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/ovraudio")
  SET(OCULUS_AUDIO_MAC_LIB_FILE "${OCULUS_AUDIO_MAC_LIB_DIR}/ovraudio64.dylib")
  file(MAKE_DIRECTORY "${OCULUS_AUDIO_MAC_LIB_DIR}")
  add_custom_command(OUTPUT "${OCULUS_AUDIO_MAC_LIB_FILE}"
                 COMMAND ${CMAKE_COMMAND} -E copy "${LOVR_OCULUS_AUDIO_PATH}/Lib/mac64/OVRAudio64.framework/Versions/A/ovraudio64" "${OCULUS_AUDIO_MAC_LIB_FILE}"
                 COMMAND install_name_tool -id @rpath/ovraudio64.dylib "${OCULUS_AUDIO_MAC_LIB_FILE}"
                 WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  add_custom_target(OculusAudioFixed DEPENDS "${OCULUS_AUDIO_MAC_LIB_FILE}")
  add_dependencies(OculusAudio OculusAudioFixed)
  set_target_properties(OculusAudio PROPERTIES IMPORTED_LOCATION "${OCULUS_AUDIO_MAC_LIB_FILE}"
                                               BUILD_RPATH "${OCULUS_AUDIO_MAC_LIB_DIR}")

  set(LOVR_OCULUS_AUDIO OculusAudio)
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
target_link_libraries(lovr
  ${LOVR_OCULUS_AUDIO}
)

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

