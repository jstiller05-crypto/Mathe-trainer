cmake_minimum_required(VERSION 3.19)
project(mathe-trainer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

# Aufgaben-Generatoren + die Nabe (arithmetic_unit) - alles, was KEIN Qt-Widget
# braucht. Als eigene Liste, damit ein zweites Ziel (generator-bench, siehe unten)
# dieselben Dateien nutzen kann, ohne dass beim naechsten neuen Generator eine
# Stelle vergessen wird (vorher haette man ihn an ZWEI Stellen eintragen muessen).
set(GENERATOR_CORE_SOURCES
    task.h difficulty.h task_fragment.h
    task_generator.h task_generator.cpp
    fragment_algebra.h fragment_algebra.cpp
    number_format.h number_format.cpp
    random_utils.h random_utils.cpp
    arithmetic_unit.h arithmetic_unit.cpp
    addition_subtraction_generator.h addition_subtraction_generator.cpp
    percent_mult_div_generator.h percent_mult_div_generator.cpp
    root_power_log_generator.h root_power_log_generator.cpp
    units_generator.h units_generator.cpp
    term_generator.h term_generator.cpp
    negative_number_generator.h negative_number_generator.cpp
    divisibility_generator.h divisibility_generator.cpp
    decimal_generator.h decimal_generator.cpp
    geometry_model.h
    geometry_unit.h geometry_unit.cpp
    rectangle_generator.h rectangle_generator.cpp
)

qt_add_executable(mathe-trainer
    WIN32 MACOSX_BUNDLE
    ${GENERATOR_CORE_SOURCES}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    feedback_colors.h
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

# --- Generator-Pruefstand: GUI-loses Konsolen-Werkzeug fuer Entwickler, ruft die
# Generatoren viele Male auf und wertet ihre Ausgabe statistisch aus. Kein Teil des
# Release-Pakets, deshalb kein install()/deploy_script dafuer.
qt_add_executable(generator-bench
    benchmark_main.cpp
    ${GENERATOR_CORE_SOURCES}
)

target_link_libraries(generator-bench PRIVATE Qt::Core)
