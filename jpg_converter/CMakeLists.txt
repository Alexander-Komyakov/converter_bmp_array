cmake_minimum_required(VERSION 3.10)
project(JpgConverter LANGUAGES C)

# Поиск библиотеки JPEG
find_package(JPEG REQUIRED)

if(NOT JPEG_FOUND)
    # Попытка автоматической установки на Ubuntu/Debian
    find_program(APT_GET apt-get)
    if(APT_GET)
        message(STATUS "libjpeg not found - attempting to install via apt")
        execute_process(
            COMMAND sudo apt-get install -y libjpeg-dev
            RESULT_VARIABLE INSTALL_RESULT
        )
        if(NOT INSTALL_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to install libjpeg-dev")
        endif()
        find_package(JPEG REQUIRED)
    else()
        message(FATAL_ERROR "libjpeg-dev required. Please install manually.")
    endif()
endif()

# Список исходных файлов (добавьте свои .c файлы)
set(SRC_FILES
    jpg_converter.c
    # image.c  # если используется общий файл
)

# Создание исполняемого файла
add_executable(jpg_converter ${SRC_FILES})

# Настройки компиляции
target_compile_options(jpg_converter PRIVATE -Wall -Wextra)
target_include_directories(jpg_converter PRIVATE ${JPEG_INCLUDE_DIRS})
target_link_libraries(jpg_converter PRIVATE ${JPEG_LIBRARIES})
target_link_libraries(jpg_converter PRIVATE ${JPEG_LIBRARIES} m)

# Установка
install(TARGETS jpg_converter
        RUNTIME DESTINATION bin
        COMPONENT jpg_tools)
