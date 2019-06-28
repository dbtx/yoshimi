#
#   CMakeLists.txt
#
#   Copyright 2009-2011, Alan Calvert
#   Copyright 2014-2019, Will Godfrey & others
#
#   This file is part of yoshimi, which is free software: you can
#   redistribute it and/or modify it under the terms of the GNU General
#   Public License as published by the Free Software Foundation, either
#   version 2 of the License, or (at your option) any later version.
#
#   yoshimi is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with yoshimi.  If not, see <http://www.gnu.org/licenses/>.

project (Yoshimi)
cmake_minimum_required (VERSION 3.0)
cmake_policy (VERSION 3.0)
cmake_policy (SET CMP0004 NEW)
if (POLICY CMP0046)
    cmake_policy (SET CMP0046 NEW)
endif (POLICY CMP0046) # opengl version
if (POLICY CMP0072)
    cmake_policy (SET CMP0072 OLD)
endif (POLICY CMP0072)
set (CMAKE_CXX_STANDARD 11) # we seem to need both for
add_definitions (-std=gnu++11) # various versions of cmake

add_definitions (-Wall)

# vvv
#add_definitions (-Wpedantic) # full ISO spec
#add_definitions (-Werror) # warnings as errors
add_definitions (-Wfatal-errors) # stop on first
add_definitions (-Werror=format-security)
add_definitions (-Wextra)
add_definitions (-Wno-vla) # Nothing wrong with variable length arrays
# ^^^ comment these out on release pushes

set (YOSHIMI_VERSION "1.5.11 M")

add_definitions (-DMIN_CONFIG_MAJOR=1)
add_definitions (-DMIN_CONFIG_MINOR=5)

file (WRITE version.txt "${YOSHIMI_VERSION}")

set (CMAKE_INCLUDE_CURRENT_DIR ON)
set (CMAKE_USE_RELATIVE_PATHS OFF)
    # relative paths break some scripts(?)
set (CMAKE_SKIP_RULE_DEPENDENCY OFF)
    # Rebuild objects if rules have changed, even if source hasn't.

message (STATUS "Building Yoshimi version ${YOSHIMI_VERSION} for ${CMAKE_SYSTEM_NAME}")

include (CheckCSourceCompiles)
include (GNUInstallDirs)

mark_as_advanced (EXECUTABLE_OUTPUT_PATH)
mark_as_advanced (LIBRARY_OUTPUT_PATH)
mark_as_advanced (CMAKE_BUILD_TYPE)
mark_as_advanced (CMAKE_INSTALL_PREFIX)

# Check for jack session availability
check_c_source_compiles (
    "#include <jack/session.h>
    int main(int argc, char **argv)
    {
        jack_session_event_type_t x = JackSessionSave;
        return 0;
    }" HasJackSession
)

if (HasJackSession)
    option (JackSessionSupport "Include Jack Session Support" ON)
else (HasJackSession)
    set (JackSessionSupport OFF)
endif (HasJackSession)

option (BuildForOld_X86 "Build for legacy x86 system" OFF)

option (BuildForAMD_X86_64 "Build for AMD x86_64 system" OFF)
option (BuildForCore2_X86_64 "Build for Intel Core2 x86_64 system" OFF)
option (BuildForNEON "Build for ARM Cortex A9 system" OFF)
option (BuildForThisCPU "Optimize for presently running system (usually not portable!)" OFF)
option (BuildForDebug "Debug build without optimisation. Enable gdb debugging support" OFF)

option (BuildWithFLTK "Build with FLTK GUI" ON)

# option to build lv2 plugin
option (LV2Plugin "Build yoshimi lv2 plugin interface" ON)

# developer info
option (ShowNoteOnTimes "Report Note-On Times" OFF)
option (ReportNotes "Report Notes On/Off" OFF)

set (BuildOptions_Common
    "-ffast-math -fno-finite-math-only -fomit-frame-pointer -Wall -pipe"
    CACHE STRING "generic compilation options *always added*"
)

set (BuildOptions_X86
    "-msse -msse2 -mfpmath=sse"
    CACHE STRING "basic X86 compiler options (minimum Pentium 4 by default)"
)

set (BuildOptions_X86_64AMD
    "-march=athlon64 -m64"
  CACHE STRING "x86_64 (AMD Athlon64) compiler options"
)

set (BuildOptions_X86_64Core2
    "-march=core2 -m64"
  CACHE STRING "x86_64 (Intel Core2) compiler options"
)

set (BuildOptions_NEON
    "-march=armv7-a -mfloat-abi=hard -mfpu=neon -mcpu=cortex-a9 -mtune=cortex-a9 -mvectorize-with-neon-quad -funsafe-loop-optimizations"
  CACHE STRING "Cortex A9 compiler options"
)

set (BuildOptions_ThisCPU
    "-march=native"
  CACHE STRING "Current CPU compiler options"
)

set (BuildOptionsDebug
    "-O0 -g3 -ggdb -Wpointer-arith -Wpedantic -Wextra"
    CACHE STRING "Compile without optimisation and with debug information"
)

set (BuildOptionsRelease
    "-O3"
    CACHE STRING "Compile with optimisation for Release builds"
)

if (BuildWithFLTK)
    add_definitions (-DGUI_FLTK)
endif (BuildWithFLTK)

find_package (PkgConfig REQUIRED)
if (PKG_CONFIG_FOUND)
    message (STATUS "Found pkg-config ${PKG_CONFIG_EXECUTABLE}")
else (PKG_CONFIG_FOUND)
    message (FATAL_ERROR "pkg-config required but not found")
endif (PKG_CONFIG_FOUND)

include (CheckFunctionExists)
check_function_exists (exp10f EXP10F)

if (EXP10F)
    add_definitions (-DHAVE_EXP10F)
endif (EXP10F)

check_c_source_compiles (
    "#include <argp.h>
    #include <stdlib.h>
     int main () {
         int argc=1;
         char *argv[]={\"test\"};
         argp_parse(0,argc,argv,0,0,0);
         return 0;
     }"
     LIBC_HAS_ARGP
)

if (NOT LIBC_HAS_ARGP)
    message (STATUS "libc does not have argp")
    find_library  (ARGP_LIB argp REQUIRED)
    if (ARGP_LIB)
        message (STATUS "Found libargp")
    else (ARGP_LIB)
        message (FATAL_ERROR "libargp required but not found")
    endif (ARGP_LIB)
endif (NOT LIBC_HAS_ARGP)

# libz
find_package (ZLIB REQUIRED)
if (ZLIB_FOUND)
#    message(STATUS "Found Zlib" ${ZLIB_VERSION_STRING})
else (ZLIB_FOUND)
    message (FATAL_ERROR "libz required but not found: ${ZLIB_FOUND}")
endif (ZLIB_FOUND)

# fftw3f
pkg_check_modules (FFTW3F REQUIRED fftw3f>=0.22)
if (FFTW3F_FOUND)
    set (FFTW3F_LIBRARIES "${FFTW3F_LIBRARIES}")
    message (STATUS "Found fftw3f ${FFTW3F_VERSION}")
else (FFTW3F_FOUND)
    message (FATAL_ERROR "fftw3f >=0.22 required but not found")
endif (FFTW3F_FOUND)

# mxml
pkg_check_modules (MXML REQUIRED mxml>=2.5)
if (MXML_FOUND)
    message (STATUS "Found mxml ${MXML_VERSION}")
else (MXML_FOUND)
    message (FATAL_ERROR "mxml >=2.5 required but not found")
endif (MXML_FOUND)

# Alsa
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    pkg_check_modules (ALSA REQUIRED alsa>=1.0.17)
    if (ALSA_FOUND)
        message (STATUS "Found Alsa ${ALSA_VERSION}")
    else (ALSA_FOUND)
        message (FATAL_ERROR "Alsa >=1.0.17 required but not found")
    endif (ALSA_FOUND)
    add_definitions (-DHAVE_ALSA)
    FIND_LIBRARY (LIBDL_LINUX NAMES dl)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# Jack
pkg_check_modules (JACK REQUIRED jack>=0.115.6)
if (JACK_FOUND)
    message (STATUS "Found jack ${JACK_VERSION}")
else (JACK_FOUND)
    message (FATAL_ERROR "Jack >=0.115.6 required but not found")
endif (JACK_FOUND)

# fontconfig
pkg_check_modules (FONTCONFIG REQUIRED fontconfig>=0.22)
mark_as_advanced (FONTCONFIG_LIBRARIES)
if (FONTCONFIG_FOUND)
    message (STATUS "Found fontconfig ${FONTCONFIG_VERSION}")
else (FONTCONFIG_FOUND)
    message (FATAL_ERROR "fontconfig>=0.22 required but not found")
endif (FONTCONFIG_FOUND)


if (BuildWithFLTK)
# libcairo
    pkg_check_modules (LIBCAIRO REQUIRED cairo)
    if (LIBCAIRO_FOUND)
        message (STATUS "Found libcairo ${LIBCAIRO_VERSION}")
    else (LIBCAIRO_FOUND)
        message (FATAL_ERROR "libcairo required but not found")
    endif (LIBCAIRO_FOUND)

# fltk
    find_package (FLTK REQUIRED)
    if (FLTK_FOUND)
        message (STATUS "Found FLTK")
    else (FLTK_FOUND)
        message (FATAL_ERROR "FLTK required but not found")
    endif (FLTK_FOUND)
    mark_as_advanced (FLTK_DIR)
    mark_as_advanced (FLTK_FLUID_EXECUTABLE)
    mark_as_advanced (FLTK_MATH_LIBRARY)
endif (BuildWithFLTK)


# libncurses / libcursesw
set (CURSES_NEED_NCURSES TRUE)
find_package (Curses REQUIRED)
if (CURSES_FOUND)
    message (STATUS "Found libncurses or libncursesw")
else (CURSES_FOUND)
    message (FATAL_ERROR "libncurses or libncursesw required but not found")
endif (CURSES_FOUND)

find_path (Readline_ROOT_DIR
    NAMES include/readline/readline.h
)

find_path (Readline_INCLUDE_DIR
    NAMES readline/readline.h
    HINTS ${Readline_ROOT_DIR}/include
)

find_library(Readline_LIBRARY
    NAMES readline
    HINTS ${Readline_ROOT_DIR}/lib
)

if (Readline_INCLUDE_DIR AND Readline_LIBRARY AND CURSES_LIBRARY)
    set (READLINE_FOUND TRUE)
else (Readline_INCLUDE_DIR AND Readline_LIBRARY AND CURSES_LIBRARY)
    FIND_LIBRARY (Readline_LIBRARY NAMES readline)
    include (FindPackageHandleStandardArgs)
    FIND_PACKAGE_HANDLE_STANDARD_ARGS (Readline DEFAULT_MSG Readline_INCLUDE_DIR Readline_LIBRARY )
    MARK_AS_ADVANCED (Readline_INCLUDE_DIR Readline_LIBRARY)
endif (Readline_INCLUDE_DIR AND Readline_LIBRARY AND CURSES_LIBRARY)

mark_as_advanced (
    Readline_ROOT_DIR
    Readline_INCLUDE_DIR
    Readline_LIBRARY
)

if (READLINE_FOUND)
    message (STATUS "Readline library found" )
else (READLINE_FOUND)
    message (FATAL_ERROR "Readline library not found! Please install development components (libreadline-dev)" )
endif (READLINE_FOUND)

# set platform specific compiler flags
if (BuildForOld_X86)
    set (OurCxxFlags ${BuildOptions_X86})
else (BuildForOld_X86)
    if (BuildForAMD_X86_64)
        set (OurCxxFlags "${BuildOptions_X86_64AMD} ${BuildOptions_X86}")
    else (BuildForAMD_X86_64)
        if (BuildForCore2_X86_64)
            set (OurCxxFlags "${BuildOptions_X86_64Core2} ${BuildOptions_X86}")
        else (BuildForCore2_X86_64)
            if (BuildForNEON)
                set (OurCxxFlags ${BuildOptions_NEON})
            else (BuildForNEON)
                if (BuildForThisCPU)
                    set (OurCxxFlags ${BuildOptions_ThisCPU})
                else (BuildForThisCPU)
                    set (OurCxxFlags "")
                endif (BuildForThisCPU)
            endif (BuildForNEON)
        endif (BuildForCore2_X86_64)
    endif (BuildForAMD_X86_64)
endif (BuildForOld_X86)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OurCxxFlags} ${BuildOptions_Common}")

if (BuildForDebug)
    set (CMAKE_BUILD_TYPE "Debug")
    set (CMAKE_CXX_FLAGS_DEBUG "${BuildOptionsDebug} ${CMAKE_CXX_FLAGS}")
    message (STATUS "DEBUG Build, flags: ${CMAKE_CXX_FLAGS_DEBUG}")
else (BuildForDebug)
    set (CMAKE_BUILD_TYPE "Release")
    set (CMAKE_CXX_FLAGS_RELEASE "${BuildOptionsRelease} ${CMAKE_CXX_FLAGS}")
    message (STATUS "RELEASE Build, flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif (BuildForDebug)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/../desktop/yoshimi.desktop.in"
    "${CMAKE_CURRENT_BINARY_DIR}/yoshimi.desktop"
    IMMEDIATE @ONLY)

# LV2 doesn't use them
set (CmdInterface_sources
    Interface/CmdInterface.cpp
    Interface/CmdInterface.h
)

set (DSP_sources
    DSP/AnalogFilter.cpp
    DSP/AnalogFilter.h
    DSP/FFTwrapper.cpp
    DSP/FFTwrapper.h
    DSP/Filter.cpp
    DSP/Filter.h
    DSP/Filter_.h
    DSP/FormantFilter.cpp
    DSP/FormantFilter.h
    DSP/SVFilter.cpp
    DSP/SVFilter.h
    DSP/Unison.cpp
    DSP/Unison.h
)

set (Effects_sources
    Effects/Alienwah.cpp
    Effects/Alienwah.h
    Effects/Chorus.cpp
    Effects/Chorus.h
    Effects/Distorsion.cpp
    Effects/Distorsion.h
    Effects/DynamicFilter.cpp
    Effects/DynamicFilter.h
    Effects/Echo.cpp
    Effects/Echo.h
    Effects/Effect.cpp
    Effects/Effect.h
    Effects/EffectLFO.cpp
    Effects/EffectLFO.h
    Effects/EffectMgr.cpp
    Effects/EffectMgr.h
    Effects/EQ.cpp
    Effects/EQ.h
    Effects/Phaser.cpp
    Effects/Phaser.h
    Effects/Reverb.cpp
    Effects/Reverb.h
)

set (Interface_sources
    Interface/Data2Text.cpp
    Interface/Data2Text.h
    Interface/FileMgr.cpp
    Interface/FileMgr.h
    Interface/InterChange.cpp
    Interface/InterChange.h
    Interface/MidiLearn.cpp
    Interface/MidiLearn.h
    Interface/MidiDecode.cpp
    Interface/MidiDecode.h
    Interface/RingBuffer.cpp
    Interface/RingBuffer.h
    UI/MiscGui.cpp
    UI/MiscGui.h
)

set (Misc_sources
    Misc/Bank.cpp
    Misc/Bank.h
    Misc/ConfBuild.h
    Misc/Config.cpp
    Misc/Config.h
    Misc/Float2Int.h
    Misc/Microtonal.cpp
    Misc/Microtonal.h
    Misc/MiscFuncs.cpp
    Misc/MiscFuncs.h
    Misc/Part.cpp
    Misc/Part.h
    Misc/RandomGen.h
    Misc/Splash.cpp
    Misc/Splash.h
    Misc/SynthEngine.cpp
    Misc/SynthEngine.h
    Misc/SynthHelper.h
    Misc/Tank.cpp
    Misc/Tank.h
    Misc/WaveShapeSamples.h
    Misc/WavFile.cpp
    Misc/WavFile.h
    Misc/XMLwrapper.cpp
    Misc/XMLwrapper.h
)

set (MusicIO_sources
    MusicIO/AlsaEngine.cpp
    MusicIO/AlsaEngine.h
    MusicIO/JackEngine.cpp
    MusicIO/JackEngine.h
    MusicIO/MusicClient.cpp
    MusicIO/MusicClient.h
    MusicIO/MusicIO.cpp
    MusicIO/MusicIO.h
)

set (Params_sources
    Params/ADnoteParameters.cpp
    Params/ADnoteParameters.h
    Params/Controller.cpp
    Params/Controller.h
    Params/EnvelopeParams.cpp
    Params/EnvelopeParams.h
    Params/FilterParams.cpp
    Params/FilterParams.h
    Params/LFOParams.cpp
    Params/LFOParams.h
    Params/PADnoteParameters.cpp
    Params/PADnoteParameters.h
    Params/Presets.cpp
    Params/Presets.h
    Params/PresetsStore.cpp
    Params/PresetsStore.h
    Params/SUBnoteParameters.cpp
    Params/SUBnoteParameters.h
    Params/UnifiedPresets.cpp
    Params/UnifiedPresets.h
)

set (Synth_sources
    Synth/ADnote.cpp
    Synth/ADnote.h
    Synth/Envelope.cpp
    Synth/Envelope.h
    Synth/LegatoTypes.h
    Synth/LFO.cpp
    Synth/LFO.h
    Synth/OscilGen.cpp
    Synth/OscilGen.h
    Synth/PADnote.cpp
    Synth/PADnote.h
    Synth/Resonance.cpp
    Synth/Resonance.h
    Synth/SUBnote.cpp
    Synth/SUBnote.h
    Synth/SynthTypes.h
)


if (BuildWithFLTK)
    set (FltkUI_names
        "ADnoteUI"
        "BankUI"
        "ConfigUI"
        "ConsoleUI"
        "EffUI"
        "EnvelopeUI"
        "FilterUI"
        "LFOUI"
        "MasterMiscUI"
        "MasterUI"
        "MicrotonalUI"
        "MidiLearnUI"
        "OscilGenUI"
        "PADnoteUI"
        "ParametersUI"
        "PartUI"
        "PresetsUI"
        "ResonanceUI"
        "SUBnoteUI"
        "VectorUI"
        "VirKeyboardUI"
    )

# workaround fltk_wrap_ui breakage
    set (FltkUI_sources)
    set (FltkUI_headers)
    foreach (basename ${FltkUI_names})
        set (fluidfile "${CMAKE_CURRENT_SOURCE_DIR}/UI/${basename}.fl")
        set (sourcefile "${basename}.cpp")
        set (headerfile "${basename}.h")
        add_custom_command (
            OUTPUT ${sourcefile}
            COMMAND ${FLTK_FLUID_EXECUTABLE} ARGS -c -o .cpp ${fluidfile}
            DEPENDS ${fluidfile}
        )
        set (FltkUI_sources ${FltkUI_sources} "${sourcefile}")
        set (FltkUI_headers ${FltkUI_headers} "${headerfile}")
    endforeach (basename ${FltkUI_names})

    set (FltkUI_sources
        ${FltkUI_sources}
        UI/DynamicTooltip.cpp
        UI/DynamicTooltip.h
        UI/WidgetPDial.cpp
        UI/WidgetPDial.h
        UI/WidgetMWSlider.cpp
        UI/WidgetMWSlider.h
        UI/MiscGui.cpp
        UI/MiscGui.h
    )
    set (YOSHI_INCLUDES ${FLTK_INCLUDE_DIR})
endif (BuildWithFLTK)

add_definitions (
    -D'YOSHIMI_VERSION="${YOSHIMI_VERSION}"'
    -D'BASE_INSTALL_DIR="${CMAKE_INSTALL_PREFIX}"'
    ${ALSA_LDFLAGS}
    ${JACK_LDFLAGS}
    -DYOSHI_FIFO_DIR="${FifoDirectory}"
)

add_definitions (-DYOSHIMI="yoshimi")

if (ShowNoteOnTimes)
    add_definitions (-DREPORT_NOTEON)
endif (ShowNoteOnTimes)

if (ReportNotes)
    add_definitions (-DREPORT_NOTES_ON_OFF)
endif (ReportNotes)

add_definitions (-DDEFAULT_AUDIO=jack_audio)

add_definitions (-DDEFAULT_MIDI=jack_midi)

if (JackSessionSupport)
    message (STATUS "With jack session support")
    add_definitions (-DJACK_SESSION)
else (JackSessionSupport)
    message (STATUS "Without jack session support")
endif (JackSessionSupport)

# Check for jack latency API >= 0.120.1
check_c_source_compiles (
    "#include <jack/jack.h>
    int main(int argc, char **argv)
    {
        if (jack_set_latency_callback)
            return 0;
        else
            return 1;
    }"
    HasJackLatency
)

if (HasJackLatency)
    add_definitions (-DJACK_LATENCY)
endif (HasJackLatency)

set (ProgSources
    ${CmdInterface_sources}
    ${DSP_sources}
    ${Effects_sources}
    ${FltkUI_sources}
    ${Interface_sources}
    ${Misc_sources}
    ${MusicIO_sources}
    ${Params_sources}
    ${Synth_sources}
)

include_directories (AFTER
    ${ALSA_INCLUDE_DIRS}
    ${FFTW3F_INC_DIR}
    ${FLTK_INCLUDE_DIR}
    ${FONTCONFIG_INCLUDE_DIRS}
    ${JACK_INCLUDE_DIRS}
    ${LIBCAIRO_INCLUDE_DIRS}
    ${MXML_INCLUDE_DIRS}
    ${Readline_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
)

set (ExternLibraries
    ${ALSA_LIBRARIES}
    ${CURSES_LIBRARIES}
    ${FFTW3F_LIBRARIES}
    ${FLTK_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${JACK_LIBRARIES}
    ${LIBCAIRO_LIBRARIES}
    ${LIBDL_LINUX}
    ${LIBSNDFILE_LIBRARIES}
    ${MXML_LIBRARIES}
    ${Readline_LIBRARY}
    ${ZLIB_LIBRARIES}
)

if (ARGP_LIB)
    set (ExternLibraries
        ${ExternLibraries}
        ${ARGP_LIB}
    )
endif (ARGP_LIB)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    link_directories (/usr/local/lib)
endif ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")

add_executable (yoshimi
    ${ProgSources}
    globals.h
    main.cpp
)

target_link_libraries (yoshimi
    ${ExternLibraries}
)

install (TARGETS yoshimi RUNTIME DESTINATION bin)

install (DIRECTORY ../banks DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/yoshimi
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE
        GROUP_READ GROUP_WRITE
        WORLD_READ
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)

install (DIRECTORY ../examples DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/yoshimi
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE
        GROUP_READ GROUP_WRITE
        WORLD_READ
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)

install (DIRECTORY ../presets DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/yoshimi
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE
        GROUP_READ GROUP_WRITE
        WORLD_READ
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)

install (DIRECTORY ../doc/ DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/yoshimi
    FILE_PERMISSIONS
        OWNER_READ OWNER_WRITE
        GROUP_READ GROUP_WRITE
        WORLD_READ
    DIRECTORY_PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/yoshimi.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../desktop/yoshimi.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../desktop/yoshimi.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../desktop/yoshimi_alt.svg
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../desktop/metainfo/yoshimi.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/../desktop/yoshimi.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

set_directory_properties (PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${FltkUI_headers}"
)

add_custom_target (showversion
    COMMAND echo -n "Version: "
    COMMAND cat version.txt
    COMMAND echo
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# clean sweep
add_custom_target (distclean
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/*.log
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/Makefile
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/install_manifest.txt
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/cmake_install.cmake
    COMMAND find ${CMAKE_SOURCE_DIR} -type f -name CMakeCache.txt | xargs -r rm -f
    COMMAND find ${CMAKE_SOURCE_DIR} -type d -name CMakeFiles | xargs -r rm -rf
    COMMAND find ${CMAKE_SOURCE_DIR} -type f -name "*.marks" | xargs -r rm -f
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/LV2_Plugin/Makefile
    COMMAND rm -f ${CMAKE_SOURCE_DIR}/LV2_Plugin/cmake_install.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)


if (LV2Plugin)
    if (NOT BuildWithFLTK)
        message (FATAL_ERROR "Can't build LV2 headless")
    endif (NOT BuildWithFLTK)
    if (EXISTS $ENV{LV2_PATH})
        set (LV2_INSTALL_DIR $ENV{LV2_PATH} CACHE PATH
        "Specifies where the LV2 libraries should be installed")
    else (EXISTS $ENV{LV2_PATH})
        set (LV2_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/lv2 CACHE PATH
        "Specifies where the LV2 libraries should be installed")
    endif (EXISTS $ENV{LV2_PATH})

    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        message (STATUS "Found lv2 package ${LV2_VERSION}")
    else (LV2_FOUND)
        message (FATAL_ERROR "lv2 package required but not found (version 1.0.0 needed)")
    endif (LV2_FOUND)

    set (LV2_plugin_sources
        LV2_Plugin/YoshimiLV2Plugin.cpp
        LV2_Plugin/YoshimiLV2Plugin.h
        LV2_Plugin/lv2extui.h
        LV2_Plugin/lv2extprg.h
    )

    set (yoshimi_manifest_ttl
        LV2_Plugin/manifest.ttl
    )

    set (yoshimi_plugin_ttl
        LV2_Plugin/yoshimi.ttl
    )

    add_library (yoshimi_lv2 MODULE
        ${DSP_sources}
        ${Effects_sources}
        ${FltkUI_sources}
        ${Interface_sources}
        ${LV2_plugin_sources}
        ${Misc_sources}
        ${MusicIO_sources}
        ${Params_sources}
        ${Synth_sources}
        ${yoshimi_manifest_ttl}
        ${yoshimi_plugin_ttl}
        globals.h
    )

    add_custom_target (yoshimi_lv2_ttl
        COMMAND test -f ${CMAKE_CURRENT_SOURCE_DIR}/${yoshimi_manifest_ttl}
        COMMAND test -f ${CMAKE_CURRENT_SOURCE_DIR}/${yoshimi_plugin_ttl}
    )

    include_directories (AFTER
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
        ${FFTW3F_INC_DIR}
        ${FLTK_INCLUDE_DIR}
        ${FONTCONFIG_INCLUDE_DIRS}
        ${LV2_INCLUDE_DIRS}
        ${MXML_INCLUDE_DIRS}
        ${ZLIB_INCLUDE_DIRS}
    )

    target_link_libraries (yoshimi_lv2
        ${ALSA_LIBRARIES}
        ${FFTW3F_LIBRARIES}
        ${FLTK_LIBRARIES}
        ${JACK_LIBRARIES}
        ${LIBCAIRO_LIBRARIES}
        ${LIBDL_LINUX_LIBS}
        ${LIBSNDFILE_LIBRARIES}
        ${LV2_LIBRARIES}
        ${MXML_LIBRARIES}
        ${ZLIB_LIBRARIES}
    )

    add_dependencies (yoshimi_lv2 yoshimi_lv2_ttl)

    install (TARGETS yoshimi_lv2 DESTINATION ${LV2_INSTALL_DIR}/yoshimi.lv2)
    install (FILES ${yoshimi_manifest_ttl} ${yoshimi_plugin_ttl}
      DESTINATION ${LV2_INSTALL_DIR}/yoshimi.lv2
)

endif (LV2Plugin)
