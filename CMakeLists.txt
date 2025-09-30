cmake_minimum_required(VERSION 3.16)
project(tancadataacquisition VERSION 1.0 LANGUAGES CXX)

# activate C++17 - has to be same as root
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: no GNU extensions

# important for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# automatic MOC, UIC, RCC (for Qt)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# add executables
add_executable(tancadataacquisition
    src/main.cpp
    include/Arduino.h
    src/Arduino.cpp
    src/ConfigHandler.cpp
    src/DataCollector.cpp
    src/DigitizerWrapper.cpp
    include/ErrorHandler.h
    src/ErrorHandler.cpp
    src/RootTreeWriter.cpp
    src/TimeTagHandler.cpp
    include/Window.h
    src/Window.cpp
    include/SettingsWidget.h
    src/SettingsWidget.cpp
    resources.qrc
)

# includes
target_include_directories(tancadataacquisition
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


# ============================================================
# CAEN
# ============================================================

# find CAEN Digitizer libary
find_library(CAEN_DIGITIZER_LIB
    NAMES CAENDigitizer
    PATHS /usr/lib /usr/local/lib
)

find_path(CAEN_DIGITIZER_INCLUDE
    NAMES CAENDigitizer.h
    PATHS /usr/include /usr/local/include
)

if(NOT CAEN_DIGITIZER_LIB OR NOT CAEN_DIGITIZER_INCLUDE)
    message(FATAL_ERROR "CAEN Digitizer library not found!")
endif()

target_include_directories(tancadataacquisition PRIVATE ${CAEN_DIGITIZER_INCLUDE})
target_link_libraries(tancadataacquisition PRIVATE ${CAEN_DIGITIZER_LIB})


# ============================================================
# ROOT
# ============================================================
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree)
target_link_libraries(tancadataacquisition
  PRIVATE
    ROOT::Core
    ROOT::RIO
    ROOT::Hist
    ROOT::Tree
)


# ============================================================
# Qt6
# ============================================================
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets SerialPort DBus)
target_link_libraries(tancadataacquisition
  PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
    Qt6::DBus
)



# ============================================================
# nlohmann_json (FetchContent)
# ============================================================
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)
target_link_libraries(tancadataacquisition PRIVATE nlohmann_json::nlohmann_json)

# --- Optionales Debugging ---
# set(CMAKE_VERBOSE_MAKEFILE ON)
# message(STATUS "ROOT version: ${ROOT_VERSION}")
# message(STATUS "Qt6Core version: ${Qt6Core_VERSION}")
