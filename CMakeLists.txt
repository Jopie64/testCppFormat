cmake_minimum_required (VERSION 2.8)
project(TestCppFormat)

SET (CMAKE_MFC_FLAG 2)

if (CMAKE_COMPILER_IS_GNUCXX )
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(FMT_TEST off)
set(FMT_INSTALL off)
set(FMT_DOC off)
add_subdirectory(../cppformat cppformat)

ADD_DEFINITIONS(-D_AFXDLL -D_CRT_SECURE_NO_WARNINGS)

set(SOURCES_CPP main.cpp)
set(SOURCES_H)

source_group("include" FILES ${SOURCES_H})
source_group("src" FILES ${SOURCES_CPP})

add_executable(TestCppFormat
	${SOURCES_H}
	${SOURCES_CPP}
	)

target_link_libraries(TestCppFormat cppformat)