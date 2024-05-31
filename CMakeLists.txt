cmake_minimum_required(VERSION 3.5)

project(Test1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# add_subdirectory("/Users/albertkison/desktop/cpp_sql" build_cpp_sql)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt6 REQUIRED COMPONENTS Sql)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Test1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        highlighter.h highlighter.cpp
        LineNumberArea.cpp
        codeeditor.h codeeditor.cpp
        fileviewer.h fileviewer.cpp
        diagram.h diagram.cpp
        diagramscene.h diagramscene.cpp
        diagramitem.h
        diagramitem.h diagramitem.cpp
        diagramtextitem.h diagramtextitem.cpp
        arrow.h arrow.cpp
        Resources.qrc
        sqlcodegenerator.h sqlcodegenerator.cpp
        Table.h
        Relationship.h
        ../Desktop/cpp_sql/sql-parser/src/parser/bison_parser.cpp ../Desktop/cpp_sql/sql-parser/src/parser/bison_parser.h ../Desktop/cpp_sql/sql-parser/src/parser/bison_parser.y ../Desktop/cpp_sql/sql-parser/src/parser/flex_lexer.cpp ../Desktop/cpp_sql/sql-parser/src/parser/flex_lexer.h ../Desktop/cpp_sql/sql-parser/src/parser/flex_lexer.l ../Desktop/cpp_sql/sql-parser/src/parser/keywordlist_generator.py ../Desktop/cpp_sql/sql-parser/src/parser/parser_typedef.h ../Desktop/cpp_sql/sql-parser/src/parser/sql_keywords.txt ../Desktop/cpp_sql/sql-parser/src/sql/AlterStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/ColumnType.h ../Desktop/cpp_sql/sql-parser/src/sql/CreateStatement.cpp ../Desktop/cpp_sql/sql-parser/src/sql/CreateStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/DeleteStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/DropStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/ExecuteStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/ExportStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/Expr.cpp ../Desktop/cpp_sql/sql-parser/src/sql/Expr.h ../Desktop/cpp_sql/sql-parser/src/sql/ImportStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/InsertStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/PrepareStatement.cpp ../Desktop/cpp_sql/sql-parser/src/sql/PrepareStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/SelectStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/ShowStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/SQLStatement.cpp ../Desktop/cpp_sql/sql-parser/src/sql/SQLStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/statements.cpp ../Desktop/cpp_sql/sql-parser/src/sql/statements.h ../Desktop/cpp_sql/sql-parser/src/sql/Table.h ../Desktop/cpp_sql/sql-parser/src/sql/TransactionStatement.h ../Desktop/cpp_sql/sql-parser/src/sql/UpdateStatement.h ../Desktop/cpp_sql/sql-parser/src/util/sqlhelper.cpp ../Desktop/cpp_sql/sql-parser/src/util/sqlhelper.h ../Desktop/cpp_sql/sql-parser/src/SQLParser.cpp ../Desktop/cpp_sql/sql-parser/src/SQLParser.h ../Desktop/cpp_sql/sql-parser/src/SQLParserResult.cpp ../Desktop/cpp_sql/sql-parser/src/SQLParserResult.h
        createprojectwindow.h createprojectwindow.cpp createprojectwindow.ui
        projectwindow.h projectwindow.cpp projectwindow.ui
        Project.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Test1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(Test1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Test1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Test1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Sql Qt${QT_VERSION_MAJOR}::PrintSupport)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Test1)
endif()
set_target_properties(Test1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Test1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Test1)
endif()
