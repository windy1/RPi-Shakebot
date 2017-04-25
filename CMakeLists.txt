cmake_minimum_required(VERSION 3.6)
project(shakespeare)

set(CMAKE_CXX_STANDARD 14)
if(APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-framework CoreServices -framework AudioUnit -framework AudioToolbox -framework Carbon")
endif()

set(SOURCE_FILES
        src/main.cpp
        src/app.h
        src/graphics/RenderShakebot.cpp src/graphics/RenderShakebot.h
        src/Shakebot.cpp src/Shakebot.h
        src/tests.cpp src/tests.h
        src/audio/speech.cpp src/audio/speech.h
        src/audio/record.cpp src/audio/record.h
        src/audio/flac.cpp src/audio/flac.h
        src/audio/base64.cpp src/audio/base64.h
        src/audio/speech_api.cpp src/audio/speech_api.h
        src/audio/audio.h src/graphics/graphics.cpp src/graphics/graphics.h src/cmd/Command.cpp src/cmd/Command.h src/cmd/GreetingCommand.cpp src/cmd/GreetingCommand.h src/cmd/wiki_api.cpp src/cmd/wiki_api.h src/RestClient.cpp src/RestClient.h src/audio/AudioClient.cpp src/audio/AudioClient.h src/audio/audio.cpp)
add_executable(shakespeare ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR}  ${GLUT_INCLUDE_DIRS})
find_package(SFML REQUIRED system window graphics audio)
include_directories(${SFML_INCLUDE_DIR})

include_directories(/usr/local/opt/speech_tools/include)

if(NOT APPLE)
    target_link_libraries(shakespeare rt m jack pthread asound estools eststring estbase)
endif()

target_link_libraries(shakespeare
        ${SFML_LIBRARIES}
        ${OPENGL_LIBRARIES}
        ncurses
        portaudio
        FLAC++
        FLAC
        curl
        Festival
        /usr/local/opt/speech_tools/lib/libestools.a
        /usr/local/opt/speech_tools/lib/libeststring.a
        /usr/local/opt/speech_tools/lib/libestbase.a)
