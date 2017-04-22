cmake_minimum_required(VERSION 3.6)
project(shakespeare)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-stdlib=libc++")
set(CMAKE_C_FLAGS "-stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "-framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Carbon")

set(SOURCE_FILES
        src/main.cpp
        src/graphics/Graphics.cpp src/graphics/Graphics.h
        src/app.h
        src/graphics/RenderShakebot.cpp src/graphics/RenderShakebot.h
        src/Shakebot.cpp src/Shakebot.h
        src/tests.cpp src/tests.h
        src/audio/speech.cpp src/audio/speech.h
        src/audio/record.cpp src/audio/record.h
        src/audio/flac.cpp src/audio/flac.h
        src/audio/base64.cpp src/audio/base64.h
        src/audio/speech_api.cpp src/audio/speech_api.h
        src/audio/audio.h)
add_executable(shakespeare ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR}  ${GLUT_INCLUDE_DIRS})

find_package(SFML REQUIRED system window graphics audio)
include_directories(${SFML_INCLUDE_DIR})

if(APPLE)
    include_directories(/usr/local/opt/speech_tools/include /usr/local/opt/festival/src/include/festival.h)
    link_directories(/usr/local/opt/speech_tools/lib /usr/local/opt/festival/src/lib)
endif()

target_link_libraries(shakespeare
        ${SFML_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ncurses
        portaudio
        FLAC++
        FLAC
        curl
        /usr/local/opt/festival/src/lib/libFestival.a
        /usr/local/opt/speech_tools/lib/libestools.a
        /usr/local/opt/speech_tools/lib/libeststring.a
        /usr/local/opt/speech_tools/lib/libestbase.a)
