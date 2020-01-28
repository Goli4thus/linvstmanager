cmake_minimum_required(VERSION 2.8.11)

set(TARGET_NAME LinVstManager)

project(${TARGET_NAME})

# Project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Set installation path
set(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "")


set(CMAKE_BUILD_TYPE Release)

find_package(Qt5Widgets REQUIRED)

# Generate config header
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/config.h
)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compiler flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -D__WIDL_objidl_generated_name_0000000C=")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()


## Setup path, where CMake would search for additional modules
#set(CMAKE_MODULE_PATH
	#${CMAKE_MODULE_PATH}
	#${CMAKE_CURRENT_SOURCE_DIR}/cmake
#)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

set(SOURCES
    src/confighandler.cpp
    src/customprogressdialog.cpp
    src/dialogpreferences.cpp
    src/dialogscan.cpp
    src/horizontalline.cpp
    src/lineeditbridge.cpp
    src/linkhandler.cpp
    src/logoutput.cpp
    src/main.cpp
    src/mainwindow.cpp
    src/modelscan.cpp
    src/modelvstbuckets.cpp
    src/preferences.cpp
    src/scanhandler.cpp
    src/scanresult.cpp
    src/verticalline.cpp
    src/vstbucket.cpp
)

#set(HEADERS
	#../common/config.h
	#../common/types.h
	#models/generictreemodel.h
	#widgets/generictreeview.h
#)


#set(RESOURCES
	#resources/resources.qrc
#)

set(LIBRARIES
	Qt5::Widgets
)

#qt5_add_resources(RCC_SOURCES ${RESOURCES})

# Set target
#add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS} ${RCC_SOURCES})
add_executable(${TARGET_NAME} ${SOURCES})

# Use the Widgets module from Qt 5
target_link_libraries(${TARGET_NAME} ${LIBRARIES})

## Generate desktop file
#configure_file(
	#${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.desktop.in
	#${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop
#)


#install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
#install(FILES resources/${TARGET_NAME}.png DESTINATION share/icons/hicolor/48x48/apps)

#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop
		#DESTINATION share/applications)
