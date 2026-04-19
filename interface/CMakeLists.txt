# Создаем статическую библиотеку для интерфейса
add_library(interface STATIC
    mainwindow.cpp
    mainwindow.hpp
    consolewidget.cpp
    consolewidget.hpp
    syntax_highlighter.cpp
    syntax_highlighter.hpp
    theme/theme.cpp
    theme/theme.hpp
    variable_panel.hpp
    variable_panel.cpp 
)

target_link_libraries(interface
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE
        core
)

target_include_directories(interface
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}
)