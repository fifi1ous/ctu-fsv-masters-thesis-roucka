# --- CMakeLists.txt ---
# compile Control_GP_Odevzdani on MSVC2022 with Qt6.9.3 on Windows
# - Qt6 is required
# - Python is required
# - pybind11 is required
# ------------------------------------------------------------
cmake_minimum_required(VERSION 3.16)
project(Control_GP_Odevzdani LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- find Qt components ----
find_package(Qt6 REQUIRED COMPONENTS Widgets Pdf PdfWidgets)

# ---- Get Pybind11 and Python ----
set(PYBIND11_FINDPYTHON ON)
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# ---- Get pybind11 ----
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# ---- Create the executable ----
add_executable(Control_GP_Odevzdani WIN32

    # =========================
    # App (entry point)
    # =========================
    src/app/main.cpp


    # =========================
    # UI (windows, widgets, views)
    # =========================
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui

    src/ui/pdfviewerwindow.cpp
    src/ui/pdfviewerwindow.h
    src/ui/pdfviewerwindow.ui

    src/ui/displaygp.cpp
    src/ui/displaygp.h
    src/ui/displaygp.ui

    src/ui/displayzpmz.cpp
    src/ui/displayzpmz.h
    src/ui/displayzpmz.ui


    # =========================
    # Structures (data models)
    # =========================
    src/structures/filebase.cpp
    src/structures/filebase.h

    src/structures/filenonpdf.cpp
    src/structures/filenonpdf.h

    src/structures/fileother.cpp
    src/structures/fileother.h

    src/structures/filepdf.cpp
    src/structures/filepdf.h

    src/structures/geometricplan.cpp
    src/structures/geometricplan.h

    src/structures/structures.h

    src/structures/maps.h


    # =========================
    # Algorithms (processing logic)
    # =========================
    src/algorithms/controlgeometricplan.cpp
    src/algorithms/controlgeometricplan.h

    src/algorithms/bboxoverlay.cpp
    src/algorithms/bboxoverlay.h

    src/algorithms/pythonmanager.h
    src/algorithms/pythonmanager.cpp

    src/algorithms/addpath.h

    src/algorithms/controlzpmz.h
    src/algorithms/controlzpmz.cpp

    src/algorithms/controlzadost.h
    src/algorithms/controlzadost.cpp

    src/algorithms/deleteworkfiles.h
    src/algorithms/deleteworkfiles.cpp

    src/algorithms/displayresults.h
    src/algorithms/displayresults.cpp

    # =========================
    # Icons
    # =========================
    icons.qrc
)

# --- Add definitions ----
add_definitions(-DQT_NO_KEYWORDS)

# --- Link directories ----
target_link_directories(Control_GP_Odevzdani PRIVATE "${Python_LIBRARY_DIRS}")

# --- Link libraries ----
target_link_libraries(Control_GP_Odevzdani PRIVATE
    Qt6::Widgets
    pybind11::embed
    Qt6::Pdf
    Qt6::PdfWidgets
    ${Python_LIBRARIES}
)

# --- Include directories ----
target_include_directories(Control_GP_Odevzdani PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/algorithms
    ${CMAKE_CURRENT_SOURCE_DIR}/src/structures
)

# --- Set source directories ----
set(WORKSPACE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/workspace")
set(MODELS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/models")
set(SCRIPTS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripts")

# --- Get Python DLLs ----
file(GLOB PYTHON_DLLS "${Python_RUNTIME_LIBRARY_DIRS}/python3*.dll")

# --- If Windows ----
if(WIN32)
    # --- Get Qt6::qmake IMPORTED_LOCATION ----
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    # --- Find windeployqt ----
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    # --- If windeployqt is found ----
    if(WINDEPLOYQT_EXECUTABLE)
        # Get the Python DLL directory
        get_filename_component(Python_DLL_DIR "${Python_RUNTIME_LIBRARY_DIRS}" ABSOLUTE)

        # --- Add custom command to deploy Qt and sync Python + scripts + models (incremental) ----
        add_custom_command(TARGET Control_GP_Odevzdani POST_BUILD
          COMMAND "${WINDEPLOYQT_EXECUTABLE}" --verbose 0 --no-compiler-runtime "$<TARGET_FILE:Control_GP_Odevzdani>"
          COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        ${PYTHON_DLLS}
                        "$<TARGET_FILE_DIR:Control_GP_Odevzdani>/"
          # --- Incremental sync: only copies changed files via robocopy /MIR ----
          # --- Sync Python + scripts + models (incremental) ----
          COMMAND ${CMAKE_COMMAND} -DSRC="${CMAKE_CURRENT_SOURCE_DIR}/python" -DDST="$<TARGET_FILE_DIR:Control_GP_Odevzdani>/python" -P "${CMAKE_CURRENT_SOURCE_DIR}/sync_dir.cmake"
          COMMAND ${CMAKE_COMMAND} -DSRC="${CMAKE_CURRENT_SOURCE_DIR}/scripts" -DDST="$<TARGET_FILE_DIR:Control_GP_Odevzdani>/scripts" -P "${CMAKE_CURRENT_SOURCE_DIR}/sync_dir.cmake"
          COMMAND ${CMAKE_COMMAND} -DSRC="${CMAKE_CURRENT_SOURCE_DIR}/models" -DDST="$<TARGET_FILE_DIR:Control_GP_Odevzdani>/models" -P "${CMAKE_CURRENT_SOURCE_DIR}/sync_dir.cmake"
          COMMAND ${CMAKE_COMMAND} -DSRC="${CMAKE_CURRENT_SOURCE_DIR}/workspace" -DDST="$<TARGET_FILE_DIR:Control_GP_Odevzdani>/workspace" -P "${CMAKE_CURRENT_SOURCE_DIR}/sync_dir.cmake"
          COMMENT "Deploying Qt and syncing Python + scripts + models (incremental)..." 
        )
    endif()
endif() # end of if(WIN32)
