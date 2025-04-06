cmake_minimum_required(VERSION 3.10)
project(SquareConverter LANGUAGES C)

# Список исходных файлов
set(SRC_FILES
    square_converter.c
)

# Создание исполняемого файла
add_executable(square_converter ${SRC_FILES})

# Настройки компиляции
target_compile_options(square_converter PRIVATE -Wall -Wextra)

# Установка (опционально)
install(TARGETS square_converter
        RUNTIME DESTINATION bin
        COMPONENT square_tools)
