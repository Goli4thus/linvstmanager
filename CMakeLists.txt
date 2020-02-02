cmake_minimum_required(VERSION 3.0.0)
# Not sure on what to define regarding minimm version. Have tested 3.16.3) myself.

set(TARGET_NAME linvstmanager)

project(${TARGET_NAME})

# Project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Debug message enabling
# Hint: Run cmake with "cmake -DENABLE_DEBUG_MSG=1 .." to enable the following:
if(ENABLE_DEBUG_MSG)
    set(DEBUG_MSG_ENABLED "1")
else()
    set(DEBUG_MSG_ENABLED "0")
endif()

# Unset this variable, so that in case the user doesn't specify it,
# we default back to "debug msg not enabled".
unset(ENABLE_DEBUG_MSG CACHE)

# Set installation path
set(INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH "")


set(CMAKE_BUILD_TYPE Release)

find_package(Qt5Widgets REQUIRED)

# Generate config header
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/src/config.h
)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}/src
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set compiler flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    #set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -D__WIDL_objidl_generated_name_0000000C=")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
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
# Enable AUTORCC for Qt resources processing
set(CMAKE_AUTORCC ON)

set(SOURCES
    src/confighandler.cpp
    src/customprogressdialog.cpp
    src/customsortfilterproxymodel.cpp
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
    src/runguard.cpp
    src/scanhandler.cpp
    src/scanresult.cpp
    src/verticalline.cpp
    src/vstbucket.cpp
)

set(RESOURCES
    src/resources/resources.qrc
)

set(LIBRARIES
	Qt5::Widgets
)

#qt5_add_resources(RCC_SOURCES ${RESOURCES})



# Set target
#add_executable(${TARGET_NAME} ${SOURCES} ${RCC_SOURCES})
add_executable(${TARGET_NAME} ${SOURCES} ${RESOURCES})

# Use the Widgets module from Qt 5
target_link_libraries(${TARGET_NAME} ${LIBRARIES})

# Generate desktop file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop
)


install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
install(FILES src/resources/${TARGET_NAME}.png DESTINATION share/icons/hicolor/48x48/apps)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.desktop
        DESTINATION share/applications)
        


################################################################
###### Triggering make based build of VstDllCheck utility ######
################################################################
add_custom_target(TargetVstDllCheck ALL make
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/VstDllCheck
    COMMENT "Building VstDllCheck utility..."
)

