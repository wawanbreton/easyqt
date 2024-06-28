cmake_minimum_required(VERSION 3.14)

project(easyqt LANGUAGES CXX)
set(version 0.0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

set(HEADER_FILES
    include/easyqt/easyqt_global.h
    include/easyqt/datastorage.h
    include/easyqt/enum.h
    include/easyqt/file.h
    include/easyqt/logger.h
    include/easyqt/logoutput.h
    include/easyqt/logpart.h
    include/easyqt/metatyperegisterer.h
    include/easyqt/preferences.h
    include/easyqt/resourcetype.h
    include/easyqt/singleton.h
    include/easyqt/writefilemode.h
)

set(CPP_FILES
    src/datastorage.cpp
    src/file.cpp
    src/logger.cpp
    src/preferences.cpp
)

add_library(easyqt SHARED
  ${HEADER_FILES}
  ${CPP_FILES}
)

target_link_libraries(easyqt PRIVATE Qt${QT_VERSION_MAJOR}::Core)

set_property(TARGET easyqt PROPERTY VERSION ${version})
set_property(TARGET easyqt PROPERTY SOVERSION 0)
set_property(TARGET easyqt PROPERTY INTERFACE_easyqt_MAJOR_VERSION 0)
set_property(TARGET easyqt APPEND PROPERTY COMPATIBLE_INTERFACE_STRING easyqt_MAJOR_VERSION)

target_compile_definitions(easyqt PRIVATE EASYQT_LIBRARY)

target_include_directories(easyqt PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
)

target_include_directories(easyqt PUBLIC
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS easyqt
        EXPORT easyqt
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${HEADER_FILES} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/easyqt)

install(EXPORT easyqt
        FILE easyqt.cmake
        NAMESPACE easyqt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easyqt
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/easyqtConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easyqt
)

write_basic_package_version_file(easyqtConfigVersion.cmake
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/easyqtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/easyqtConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easyqt
)
