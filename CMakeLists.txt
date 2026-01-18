cmake_minimum_required(VERSION 3.14)

project(easyqt LANGUAGES CXX)
set(version 0.0.1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(easyqt SHARED
    include/easyqt/communication/commands/commandheader.h
    include/easyqt/communication/commands/command.h src/communication/commands/command.cpp
    include/easyqt/communication/core/abstractcommanddatastreamer.h src/communication/core/abstractcommanddatastreamer.cpp
    include/easyqt/communication/core/abstractcommandsqueue.h src/communication/core/abstractcommandsqueue.cpp
    include/easyqt/communication/core/abstractcommunicationinterface.h src/communication/core/abstractcommunicationinterface.cpp
    include/easyqt/communication/core/abstractdevicecommandsqueue.h src/communication/core/abstractdevicecommandsqueue.cpp
    include/easyqt/communication/core/abstractsimulatedcommandsqueue.h src/communication/core/abstractsimulatedcommandsqueue.cpp
    include/easyqt/communication/core/commanddatatype.h
    include/easyqt/communication/dataparseresult.h

    include/easyqt/bitfield.h src/bitfield.cpp
    include/easyqt/datastorage.h src/datastorage.cpp
    include/easyqt/debug.h src/debug.cpp
    include/easyqt/easyqt_global.h
    include/easyqt/enum.h
    include/easyqt/file.h src/file.cpp
    include/easyqt/json.h src/json.cpp
    include/easyqt/logger.h src/logger.cpp
    include/easyqt/logoutput.h
    include/easyqt/logpart.h
    include/easyqt/metatyperegisterer.h
    include/easyqt/parser.h src/parser.cpp
    include/easyqt/preferences.h src/preferences.cpp
    include/easyqt/qobject_helper.h
    include/easyqt/resourcetype.h
    include/easyqt/singleton.h
    include/easyqt/slotreceiver.h src/slotreceiver.cpp
    include/easyqt/writefilemode.h
    include/easyqt/lazypointer.h
)

target_link_libraries(easyqt PRIVATE Qt${QT_VERSION_MAJOR}::Core)

set_property(TARGET easyqt PROPERTY VERSION ${version})
set_property(TARGET easyqt PROPERTY SOVERSION 0)
set_property(TARGET easyqt PROPERTY INTERFACE_easyqt_MAJOR_VERSION 0)
set_property(TARGET easyqt APPEND PROPERTY COMPATIBLE_INTERFACE_STRING easyqt_MAJOR_VERSION)

target_compile_definitions(easyqt PRIVATE EASYQT_LIBRARY)

target_include_directories(easyqt PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include/easyqt"
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

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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
