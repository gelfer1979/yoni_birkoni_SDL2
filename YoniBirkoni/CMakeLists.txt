cmake_minimum_required(VERSION 3.16)
project(YonyGame LANGUAGES CXX)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/YONI/sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/YONI
)

# Staging directory for assets
set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging")
file(MAKE_DIRECTORY "${STAGING_DIR}")

# Copy assets at configuration time
file(GLOB GAME_ASSETS 
    "${CMAKE_CURRENT_SOURCE_DIR}/YONI/*.gra"
    "${CMAKE_CURRENT_SOURCE_DIR}/YONI/*.san"
    "${CMAKE_CURRENT_SOURCE_DIR}/YONI/*.mp3"
    "${CMAKE_CURRENT_SOURCE_DIR}/YONI/*.dat"
)

foreach(ASSET_FILE ${GAME_ASSETS})
    get_filename_component(ASSET_NAME "${ASSET_FILE}" NAME)
    configure_file("${ASSET_FILE}" "${STAGING_DIR}/${ASSET_NAME}" COPYONLY)
endforeach()

# Copy level directories (1 to 19) at configuration time
file(GLOB LEVEL_DIRS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/YONI" "${CMAKE_CURRENT_SOURCE_DIR}/YONI/[0-9]*")
foreach(LEVEL_DIR ${LEVEL_DIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/YONI/${LEVEL_DIR}")
        file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/YONI/${LEVEL_DIR}" DESTINATION "${STAGING_DIR}")
    endif()
endforeach()

# Target source files for SDL2 port
set(GAME_SOURCES
    YONI/YonyDlg.cpp
    YONI/enemys.cpp
    YONI/sdl/compat.h
    YONI/sdl/GRAPH.H
    YONI/sdl/GRAPH.CPP
    YONI/sdl/INPUT.H
    YONI/sdl/INPUT.CPP
    YONI/sdl/sound.h
    YONI/sdl/sound.cpp
    YONI/sdl/dshow.h
    YONI/sdl/dshow.cpp
    YONI/sdl/yony.cpp
)

# Detect and configure platform targets
if(EMSCRIPTEN)
    # Emscripten target (WebAssembly)
    add_executable(yony ${GAME_SOURCES})
    
    # Enable SDL2, SDL2_mixer, SDL2_ttf and preload assets
    target_compile_options(yony PRIVATE 
        -sUSE_SDL=2 
        -sUSE_SDL_MIXER=2 
        "-sSDL2_MIXER_FORMATS=mp3"
        -sUSE_MPG123=1
        -sUSE_SDL_TTF=2
        -sASYNCIFY
    )
    
    target_link_options(yony PRIVATE 
        -sUSE_SDL=2 
        -sUSE_SDL_MIXER=2 
        "-sSDL2_MIXER_FORMATS=mp3"
        -sUSE_MPG123=1
        -sUSE_SDL_TTF=2
        --preload-file ${STAGING_DIR}@/
        -sALLOW_MEMORY_GROWTH=1
        -sASYNCIFY
        --shell-file ${CMAKE_CURRENT_SOURCE_DIR}/YONI/shell.html
    )
    
    # Output file extension
    set_target_properties(yony PROPERTIES SUFFIX ".html")
    
    # Automatically copy yony.html to index.html for itch.io and local server
    add_custom_command(TARGET yony POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE_DIR:yony>/yony.html"
        "$<TARGET_FILE_DIR:yony>/index.html"
        COMMENT "Copying yony.html to index.html for Web/itch.io entry point"
    )

elseif(ANDROID)
    message(STATUS "Configuring Android build from source...")
    
    # Disable target installation to prevent redundant file copying
    set(SDL2_DISABLE_INSTALL ON CACHE BOOL "Disable SDL2 install" FORCE)
    
    # SDL2_mixer settings
    set(SDL2_MIXER_DISABLE_INSTALL ON CACHE BOOL "Disable Mixer install" FORCE)
    set(SDL2MIXER_WAVE ON CACHE BOOL "Support WAV" FORCE)
    set(SDL2MIXER_MP3 ON CACHE BOOL "Support MP3" FORCE)
    set(SDL2MIXER_MP3_MINIMP3 ON CACHE BOOL "Support loading MP3 music via minimp3" FORCE)
    set(SDL2MIXER_MP3_MPG123 OFF CACHE BOOL "Support loading MP3 music via MPG123" FORCE)
    set(SDL2MIXER_MOD OFF CACHE BOOL "Disable MOD" FORCE)
    set(SDL2MIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
    set(SDL2MIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
    set(SDL2MIXER_OPUS OFF CACHE BOOL "Disable Opus" FORCE)
    set(SDL2MIXER_VORBIS "OFF" CACHE STRING "Disable Vorbis" FORCE)
    set(SDL2MIXER_GME OFF CACHE BOOL "Disable GME" FORCE)
    set(SDL2MIXER_WAVPACK OFF CACHE BOOL "Disable Wavpack" FORCE)
    
    # SDL2_ttf settings
    set(SDL2_TTF_DISABLE_INSTALL ON CACHE BOOL "Disable TTF install" FORCE)
    set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "Use HarfBuzz" FORCE)
    
    # Add library sources via add_subdirectory
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2 ${CMAKE_BINARY_DIR}/SDL2)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_mixer ${CMAKE_BINARY_DIR}/SDL2_mixer)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_ttf ${CMAKE_BINARY_DIR}/SDL2_ttf)
    
    # Android target (Shared library)
    add_library(yony SHARED ${GAME_SOURCES})
    
    # Link built libraries and NDK log
    find_library(ANDROID_LOG_LIB log)
    target_link_libraries(yony PRIVATE
        ${ANDROID_LOG_LIB}
        SDL2
        SDL2_mixer
        SDL2_ttf
    )
    
    # On Android, include source paths for headers
    target_include_directories(yony PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_mixer
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_ttf
    )

else()
    # PC Desktop Targets (Windows MinGW, Linux, macOS)
    add_executable(yony ${GAME_SOURCES})
    
    # Try finding packages on the system
    find_package(SDL2 QUIET)
    find_package(SDL2_mixer QUIET)
    find_package(SDL2_ttf QUIET)
    
    if(SDL2_FOUND AND SDL2_MIXER_FOUND AND SDL2_TTF_FOUND)
        message(STATUS "Using system SDL2 libraries")
        target_link_libraries(yony PRIVATE
            SDL2::SDL2
            SDL2_mixer::SDL2_mixer
            SDL2_ttf::SDL2_ttf
        )
    else()
        message(STATUS "System SDL2 libraries not found; building from external/src...")
        
        # Disable target installation to prevent redundant file copying
        set(SDL2_DISABLE_INSTALL ON CACHE BOOL "Disable SDL2 install" FORCE)
        
        # SDL2_mixer settings
        set(SDL2_MIXER_DISABLE_INSTALL ON CACHE BOOL "Disable Mixer install" FORCE)
        set(SDL2MIXER_WAVE ON CACHE BOOL "Support WAV" FORCE)
        set(SDL2MIXER_MP3 ON CACHE BOOL "Support MP3" FORCE)
        set(SDL2MIXER_MP3_MINIMP3 ON CACHE BOOL "Support loading MP3 music via minimp3" FORCE)
        set(SDL2MIXER_MP3_MPG123 OFF CACHE BOOL "Support loading MP3 music via MPG123" FORCE)
        set(SDL2MIXER_MOD OFF CACHE BOOL "Disable MOD" FORCE)
        set(SDL2MIXER_MIDI OFF CACHE BOOL "Disable MIDI" FORCE)
        set(SDL2MIXER_FLAC OFF CACHE BOOL "Disable FLAC" FORCE)
        set(SDL2MIXER_OPUS OFF CACHE BOOL "Disable Opus" FORCE)
        set(SDL2MIXER_VORBIS "OFF" CACHE STRING "Disable Vorbis" FORCE)
        set(SDL2MIXER_GME OFF CACHE BOOL "Disable GME" FORCE)
        set(SDL2MIXER_WAVPACK OFF CACHE BOOL "Disable Wavpack" FORCE)
        
        # SDL2_ttf settings
        set(SDL2_TTF_DISABLE_INSTALL ON CACHE BOOL "Disable TTF install" FORCE)
        set(SDL2TTF_HARFBUZZ OFF CACHE BOOL "Use HarfBuzz" FORCE)
        set(SDL2TTF_VENDORED ON CACHE BOOL "Use vendored third-party libraries" FORCE)
        
        # Add library sources via add_subdirectory
        if(NOT TARGET SDL2)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2 ${CMAKE_BINARY_DIR}/SDL2)
        endif()
        if(NOT TARGET SDL2_mixer)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_mixer ${CMAKE_BINARY_DIR}/SDL2_mixer)
        endif()
        if(NOT TARGET SDL2_ttf)
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_ttf ${CMAKE_BINARY_DIR}/SDL2_ttf)
        endif()
        
        target_link_libraries(yony PRIVATE
            SDL2
            SDL2_mixer
            SDL2_ttf
        )
        
        # Include source paths for headers as fallback
        target_include_directories(yony PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2/include
            ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_mixer
            ${CMAKE_CURRENT_SOURCE_DIR}/../external/src/SDL2_ttf
        )
    endif()

    
    # On desktop, copy staged assets to target bin directory at build time
    add_custom_command(TARGET yony POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${STAGING_DIR}"
        "$<TARGET_FILE_DIR:yony>"
        COMMENT "Copying game assets to execution directory"
    )
endif()

# Build optimizations
if(NOT MSVC)
    target_compile_options(yony PRIVATE -Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(yony PRIVATE -O0 -g)
    else()
        target_compile_options(yony PRIVATE -O2)
    endif()
endif()
