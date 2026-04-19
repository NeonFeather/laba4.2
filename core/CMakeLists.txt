add_library(core STATIC
    executor.cpp
    executor.hpp
    ast/ast_node.cpp
    ast/ast_node.hpp
    parser/lexer.cpp
    parser/lexer.hpp
    parser/parser.cpp
    parser/parser.hpp
    symbols/symbol_table.cpp
    symbols/symbol_table.hpp
    evaluator/postfix_evaluator.cpp
    evaluator/postfix_evaluator.hpp
    parser/lexer.hpp
)

target_link_libraries(core
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)

target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/ast
        ${CMAKE_CURRENT_SOURCE_DIR}/parser
        ${CMAKE_CURRENT_SOURCE_DIR}/symbols
        ${CMAKE_CURRENT_SOURCE_DIR}/evaluator
)