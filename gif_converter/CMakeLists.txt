cmake_minimum_required(VERSION 3.10)
project(GifConverter VERSION 1.0.0 LANGUAGES C)

# Проверка наличия компилятора
if(NOT CMAKE_C_COMPILER)
    message(FATAL_ERROR "C compiler not found!")
endif()

# Поиск библиотеки GIF
find_package(GIF REQUIRED)
if(NOT GIF_FOUND)
    message(WARNING "libgif not found - trying to install")
    find_program(APTGET apt-get)
    if(APTGET)
        execute_process(COMMAND sudo apt-get install -y libgif-dev
                       RESULT_VARIABLE INSTALL_RESULT)
        if(NOT INSTALL_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to install libgif-dev")
        endif()
        # Повторная проверка после установки
        find_package(GIF REQUIRED)
    else()
        message(FATAL_ERROR "apt-get not found - please install libgif-dev manually")
    endif()
endif()

# Настройки компилятора
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

# Сборка исполняемого файла
add_executable(gif_converter 
    gif_converter.c 
    image.c)

target_link_libraries(gif_converter PRIVATE ${GIF_LIBRARIES})
target_include_directories(gif_converter PRIVATE ${GIF_INCLUDE_DIRS})

# Установка
install(TARGETS gif_converter
        RUNTIME DESTINATION bin
        COMPONENT gif_tools)

# Тесты (если нужно)
enable_testing()
add_test(NAME test_gif_converter
         COMMAND gif_converter --test)
