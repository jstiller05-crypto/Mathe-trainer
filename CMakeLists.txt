cmake_minimum_required(VERSION 3.19)
project(mathe-trainer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

qt_add_executable(mathe-trainer
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    task_generator.cpp
    task_generator.h
    task.h
    difficulty.h
    session_controller.h
    session_controller.cpp
    resources.qrc
    sidebar_menu.h
    sidebar_menu.cpp
    task_view.h
    task_view.cpp
    settings_view.h
    settings_view.cpp
    symbol_menu.h
    symbol_menu.cpp
    task_fragment.h
    addition_subtraction_generator.h
    addition_subtraction_generator.cpp
    root_power_log_generator.h
    root_power_log_generator.cpp
    percent_mult_div_generator.h
    percent_mult_div_generator.cpp
    finance_generator.h
    finance_generator.cpp
    units_generator.h
    units_generator.cpp
    arithmetic_unit.h
    arithmetic_unit.cpp
    written_grid_widget.h
    written_grid_widget.cpp
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
