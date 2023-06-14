CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)

PROJECT( MRPC )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )


LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

FIND_PACKAGE( CLHEP REQUIRED )

FIND_PACKAGE(ROOT 6.08 REQUIRED COMPONENTS XMLIO Minuit MathMore MathCore Minuit2)

FOREACH( pkg ROOT CLHEP)
    IF( ${pkg}_FOUND )
		INCLUDE_DIRECTORIES( SYSTEM ${${pkg}_INCLUDE_DIRS} )
        LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
        ADD_DEFINITIONS ( ${${pkg}_DEFINITIONS} )
    ENDIF()
ENDFOREACH()


### LIBRARY #################################################################

# include directories
include_directories( ${PROJECT_SOURCE_DIR}/include )

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic -std=c++17" )
ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
include_directories( ./include )

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.h ${PROJECT_SOURCE_DIR}/include/*.hpp)

#add library
add_library( MyLib SHARED ${sources} ${headers} )
target_link_libraries( MyLib ${ROOT_COMPONENT_LIBRARIES} )
target_link_libraries( MyLib stdc++fs )

install( TARGETS MyLib LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/lib )

IF(UNIX)
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\$ORIGIN/../lib")
ENDIF()

file( GLOB APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/exec/*.cxx )
foreach( testsourcefile ${APP_SOURCES} )
	get_filename_component(testname ${testsourcefile} NAME_WE)
	add_executable( ${testname} ${testsourcefile} )
	target_link_libraries( ${testname} MyLib)
	install(TARGETS ${testname} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
endforeach( testsourcefile ${APP_SOURCES} )
