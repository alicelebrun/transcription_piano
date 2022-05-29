project(transcription_piano)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_C_FLAGS "-Wall -Werror -g3")

add_executable(transcription_piano transcription_piano.c son.c note.c graphique.c interface.c transcription.c)

install(TARGETS transcription_piano DESTINATION .)
INSTALL(FILES transcription.cfg DESTINATION .)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} PkgConfig::FFTW)
target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image PkgConfig::FFTW m)
add_subdirectory(Sons)
add_subdirectory(Images)
