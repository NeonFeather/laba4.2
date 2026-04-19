cmake_minimum_required(VERSION 3.16)
project(PascalIDE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Настройки для macOS
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment version")
endif()

# Поиск Qt6 (или Qt5 если Qt6 не найден)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

# Подключаем поддиректории
add_subdirectory(interface)
add_subdirectory(core)

# Создаем исполняемый файл
add_executable(PascalIDE
    main.cpp
)

# Линкуем библиотеки
target_link_libraries(PascalIDE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    interface
    core
)

# Устанавливаем свойства для macOS bundle
if(APPLE)
    set_target_properties(PascalIDE PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
endif()

# Копируем Info.plist если есть
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)
    set_target_properties(PascalIDE PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
endif()