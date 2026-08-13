cmake_minimum_required(VERSION 3.19)
project(mathe-trainer LANGUAGES CXX)

cmake_minimum_required(VERSION 3.19)
project(mathe-trainer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(mathe-trainer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    task_generator.cpp
    task_generator.h
    task.h
    operation.h
    difficulty.h
    operation.cpp
    session_controller.h
    session_controller.cpp
    topic.h
    resources.qrc
    sidebar_menu.h
    sidebar_menu.cpp
    task_view.h
    task_view.cpp
    settings_view.h
    settings_view.cpp
)

target_link_libraries(mathe-trainer
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS mathe-trainer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET mathe-trainer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
