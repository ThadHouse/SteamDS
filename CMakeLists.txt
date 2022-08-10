cmake_minimum_required(VERSION 3.13)

set(THREADS_PREFER_PTHREAD_FLAG ON)

project(SteamDS)

find_package(OpenAL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

set(FILES
    src/main.cpp
    src/gameenginesdl.cpp
    src/glew.c
)

add_executable(SteamDS ${FILES})

target_include_directories(SteamDS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/steamapi)
target_include_directories(SteamDS PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(SteamDS ${CMAKE_CURRENT_SOURCE_DIR}/steamapi/libsteam_api.so)

target_compile_definitions(SteamDS PRIVATE POSIX SDL)

target_link_libraries(SteamDS Threads::Threads ${OPENAL_LIBRARY} ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} SDL2_ttf)
