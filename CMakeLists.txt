CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(circos)
#dependency

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
endif(MSVC)


FIND_PACKAGE(PNG 1.2.9 REQUIRED)
INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIRS})
# external library: zlib (mandatory)

FIND_PACKAGE(ZLIB REQUIRED)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})

FIND_PACKAGE(Freetype REQUIRED)
IF(FREETYPE_FOUND)
	INCLUDE_DIRECTORIES(${FREETYPE_INCLUDE_DIRS})
	SET(FREETYPE_FLAGS "-DUSE_FREETYPE")
ELSE(FREETYPE_FOUND)
	SET(FREETYPE_FLAGS "-DNO_FREETYPE")
	SET(FREETYPE_LIBRARIES "")
ENDIF(FREETYPE_FOUND)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FREETYPE_FLAGS}")

FIND_PACKAGE(xlsx_reader CONFIG REQUIRED)
FIND_PACKAGE(typed_string CONFIG REQUIRED)
FIND_PACKAGE(miniz CONFIG REQUIRED)
FIND_PACKAGE(tinyxml2 CONFIG REQUIRED)
FIND_PACKAGE(nlohmann_json CONFIG REQUIRED)


SET(CIRCOS_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
SET(CIRCOS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
FILE(GLOB_RECURSE  CIRCOS_SRC_LISTS ${CIRCOS_SRC_DIR}/*.cpp)

SET(CIRCOS_TEST_DIR ${CMAKE_SOURCE_DIR}/test)

ADD_LIBRARY(circos ${CIRCOS_SRC_LISTS})

target_link_libraries(circos ${ZLIB_LIBRARIES} ${PNG_LIBRARIES} ${FREETYPE_LIBRARIES} xlsx_reader)

INCLUDE_DIRECTORIES(${CIRCOS_INCLUDE_DIR})

ADD_EXECUTABLE(shape_test  ${CIRCOS_TEST_DIR}/shape_test.cpp )
ADD_EXECUTABLE(pi_shape_test  ${CIRCOS_TEST_DIR}/pi_shape_test.cpp )
ADD_EXECUTABLE(pi_excel_test ${CIRCOS_TEST_DIR}/pi_excel_test.cpp )
ADD_EXECUTABLE(tick_test ${CIRCOS_TEST_DIR}/tick_test.cpp )
ADD_EXECUTABLE(xlsx_circos ${CIRCOS_TEST_DIR}/xlsx_circos.cpp )
ADD_EXECUTABLE(flood_test ${CIRCOS_TEST_DIR}/flood_test.cpp)
ADD_EXECUTABLE(track_test ${CIRCOS_TEST_DIR}/track_test.cpp)
ADD_EXECUTABLE(text_test ${CIRCOS_TEST_DIR}/text_test.cpp)
ADD_EXECUTABLE(freetype_test ${CIRCOS_TEST_DIR}/freetype_test.cpp)

target_link_libraries(shape_test circos)
target_link_libraries(pi_shape_test circos)

target_link_libraries(flood_test circos)
target_link_libraries(track_test circos)
target_link_libraries(xlsx_circos circos)
target_link_libraries(pi_excel_test circos)
target_link_libraries(tick_test circos)
target_link_libraries(text_test circos)
target_link_libraries(freetype_test circos)


ADD_EXECUTABLE("pi_csv" ${CIRCOS_TEST_DIR}/pi_csv.cpp)
