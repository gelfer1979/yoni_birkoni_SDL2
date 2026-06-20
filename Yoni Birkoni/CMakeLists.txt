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
    # Android target (Shared library)
    add_library(yony SHARED ${GAME_SOURCES})
    
    # Link to NDK libraries, SDL2, SDL2_mixer, SDL2_ttf
    find_library(ANDROID_LOG_LIB log)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    
    target_link_libraries(yony PRIVATE
        ${ANDROID_LOG_LIB}
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
    )

else()
    # PC Desktop Targets (Windows MinGW, Linux, macOS)
    add_executable(yony ${GAME_SOURCES})
    
    # Try CONFIG mode first, then MODULE mode
    find_package(SDL2 REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    
    target_link_libraries(yony PRIVATE
        SDL2::SDL2
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
    )
    
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
