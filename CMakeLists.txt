cmake_minimum_required(VERSION 3.10)

# Установка версии проекта с возможностью переопределения через аргумент
if(NOT DEFINED PROJECT_VERSION)
    set(PROJECT_VERSION "1.0.0" CACHE STRING "Project version")
endif()

project(ImageConverter VERSION ${PROJECT_VERSION} LANGUAGES C)

# Включаем подпроекты
add_subdirectory(gif_converter)
add_subdirectory(jpg_converter)
add_subdirectory(square_converter)

# Настройка CPack (общая для всех подпроектов)
set(CPACK_PACKAGE_NAME "image-converter")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Image conversion tools collection")
set(CPACK_PACKAGE_VENDOR "Alexander Komyakov")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexander Komyakov <sanya202018@gmail.com>")
set(CPACK_GENERATOR "DEB")

# Настройка номеров версий для DEB пакета
string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 CPACK_DEBIAN_PACKAGE_VERSION_MAJOR)
list(GET VERSION_LIST 1 CPACK_DEBIAN_PACKAGE_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_DEBIAN_PACKAGE_VERSION_PATCH)

include(CPack)
