cmake_minimum_required(VERSION 3.16)

project(wordle_game VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WORDLE_ENABLE_OPENMP "Enable OpenMP parallel support" ON)
option(WORDLE_ENABLE_MPI "Enable MPI parallel support" OFF)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appwordle_game
    main.cpp
    wordlegame.cpp
    wordlegame.h
    parallelengine.cpp
    parallelengine.h
)

qt_add_qml_module(appwordle_game
    URI wordle_game
    QML_FILES
        Main.qml
        qml/LetterTile.qml
        qml/GameBoard.qml
        qml/GameKeyboard.qml
        qml/KeyButton.qml
        qml/GameResultDialog.qml
        qml/ThemeIcon.qml
        qml/ModeSelector.qml
)

qt_add_resources(appwordle_game "game_resources"
    PREFIX "/"
    FILES
        resources/words.txt
)

set_target_properties(appwordle_game PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appwordle_game
    PRIVATE Qt6::Quick
)

if(WORDLE_ENABLE_OPENMP)
    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(appwordle_game PRIVATE OpenMP::OpenMP_CXX)
    endif()
endif()

if(WORDLE_ENABLE_MPI)
    find_package(MPI REQUIRED)
    target_compile_definitions(appwordle_game PRIVATE WORDLE_USE_MPI)
    target_link_libraries(appwordle_game PRIVATE MPI::MPI_CXX)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(appwordle_game PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

install(TARGETS appwordle_game
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
