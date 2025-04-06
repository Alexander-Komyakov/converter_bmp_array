cmake_minimum_required(VERSION 3.10)

# Установка версии пакета с возможностью переопределения через аргумент
if(NOT DEFINED PACKAGE_VERSION)
    set(PACKAGE_VERSION "1.0.0" CACHE STRING "Package version")
endif()

project(ImageConverter LANGUAGES C)  # Убрали VERSION из project()

# Включаем подпроекты
add_subdirectory(gif_converter)
add_subdirectory(jpg_converter)
add_subdirectory(square_converter)

# Настройка CPack
set(CPACK_PACKAGE_NAME "image-converter")
set(CPACK_PACKAGE_VERSION "${PACKAGE_VERSION}")  # Используем PACKAGE_VERSION
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Image conversion tools collection")
set(CPACK_PACKAGE_VENDOR "Alexander Komyakov")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Alexander Komyakov <sanya202018@gmail.com>")
set(CPACK_GENERATOR "DEB")

# Разбиваем версию на компоненты для DEB пакета
string(REPLACE "." ";" VERSION_LIST ${PACKAGE_VERSION})
list(GET VERSION_LIST 0 CPACK_DEBIAN_PACKAGE_VERSION_MAJOR)
list(GET VERSION_LIST 1 CPACK_DEBIAN_PACKAGE_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_DEBIAN_PACKAGE_VERSION_PATCH)

include(CPack)
