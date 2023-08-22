cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (ccppknife
	VERSION 0.1
	DESCRIPTION "A C++ library with some applications")

set(CMAKE_CXX_STANDARD 17)
set(LIB_VERSION 0.6.4)
set(LIB_NAME cppknife-${LIB_VERSION})
set(GEOLIB_NAME cppknifegeo-${LIB_VERSION})

if(NOT DEFINED mode)
    set(mode debug)
endif()
  

set(CMAKE_BUILD_TYPE Debug)

include(FindPkgConfig)
include(CTest) 
enable_testing()
find_package(GTest REQUIRED)

set(BASIC_SOURCES basic/BaseRandom.cpp basic/CharRandom.cpp basic/Logger.cpp basic/PortableRandom.cpp
	basic/StringTool.cpp basic/TimeTool.cpp basic/InternalError.cpp)

set(BASIC_UNITTEST_SOURCES unittest/BaseRandom_test.cpp unittest/CharRandom_test.cpp
	 unittest/Logger_test.cpp unittest/PortableRandom_test.cpp unittest/StringTool_test.cpp 
	 unittest/TimeTool_test.cpp)

set(CORE_SOURCES core/ByteStorage.cpp core/KissRandom.cpp core/Storage.cpp)

set(CORE_UNITTEST_SOURCES unittest/ByteStorage_test.cpp unittest/KissRandom_test.cpp 
	unittest/Storage_test.cpp)

set(DB_SOURCES db/MySql.cpp db/Postgres.cpp db/SqlDriver.cpp)

set(DB_UNITTEST_SOURCES unittest/MySql_test.cpp unittest/Postgres_test.cpp unittest/SqlDriver_test.cpp)

set(GEO_SOURCES geo/GeoException.cpp geo/GeoHelper.cpp geo/GpsTranslator.cpp 
	geo/Point2D.cpp geo/Rectangle.cpp geo/Shape.cpp)

set(GEO_UNITTEST_SOURCES unittest/GeoException_test.cpp unittest/GeoHelper_test.cpp 
	unittest/GpsTranslator_test.cpp unittest/Point2D_test.cpp unittest/Rectangle_test.cpp unittest/Shape_test.cpp)

set(NET_SOURCES  net/Agents.cpp net/SocketServer.cpp net/SocketClient.cpp )

SET(NET_UNITTEST_SOURCES unittest/Agents_test.cpp unittest/SocketServer_test.cpp)

set(OS_SOURCES os/File.cpp os/FileTool.cpp os/LineAgent.cpp os/OsException.cpp os/Path.cpp os/Process.cpp os/Traverser.cpp)

set(OS_UNITTEST_SOURCES unittest/File_test.cpp unittest/FileDb_test.cpp unittest/FileTool_test.cpp
	unittest/LineAgent_test.cpp unittest/OsException_test.cpp unittest/Path_test.cpp unittest/Process_test.cpp
	unittest/Traverser_test.cpp)

set(TEXT_SOURCES text/NodeJson.cpp text/Configuration.cpp text/CsvFile.cpp text/FunctionEngine.cpp text/LineList.cpp
	text/LineReader.cpp text/LinesStream.cpp text/Matcher.cpp text/Parser.cpp text/ParserError.cpp text/Script.cpp
	text/SearchEngine.cpp text/StringList.cpp text/Base64.cpp)

set(TEXT_UNITTEST_SOURCES unittest/Configuration_test.cpp unittest/CsvFile_test.cpp unittest/FunctionEngine_test.cpp
	unittest/NodeJson_test.cpp)
set(Reserve1 unittest/LineList_test.cpp unittest/LineReader_test.cpp 
	unittest/LinesStream_test.cpp unittest/Matcher_test.cpp unittest/Parser_test.cpp 
	unittest/ParserError_test.cpp unittest/Script_test.cpp unittest/SearchEngine_test.cpp 
	unittest/StringList_test.cpp unittest/Base64_test.cpp)

set(TOOLS_SOURCES tools/ArgumentParser.cpp tools/ToolsCommons.cpp tools/VeilEngine.cpp tools/SecretConfiguration.cpp)

set (DB_LIBS -lmysqlcppconn	-lpqxx -lpq)
set (GEO_LIBS gdal)
set (UNITTEST_LIB cppknifeunittest)
set (GOOGLE_TEST_LIB GTest::gtest GTest::gtest_main)
set (CPPKNIFE_LIBS ${GOOGLE_TEST_LIB} ${LIB_NAME} ${UNITTEST_LIB}  ${GOOGLE_TEST_LIB})

add_library(${GEOLIB_NAME} SHARED ${GEO_SOURCES} ${GEO_UNITTEST_SOURCES})
target_compile_options(${GEOLIB_NAME} PRIVATE -Wall)
target_include_directories(${GEOLIB_NAME} PRIVATE /usr/include/gdal gdal)

add_library(${LIB_NAME} SHARED  ${TEXT_SOURCES} ${BASIC_SOURCES} ${CORE_SOURCES} ${NET_SOURCES} ${OS_SOURCES} 
	${TOOLS_SOURCES} unittest/google_test.cpp)
target_compile_options(${LIB_NAME} PRIVATE -Wall)

add_library(cppknifeunittest SHARED ${TEXT_UNITTEST_SOURCES})
#	${BASIC_UNITTEST_SOURCES} ${CORE_UNITTEST_SOURCES} ${NET_UNITTEST_SOURCES}
#	${OS_UNITTEST_SOURCES} ${GEO_UNITTEST_SOURCES}
#	${TOOLS_UNITTEST_SOURCES})
target_include_directories(cppknifeunittest PRIVATE /usr/include/gdal gdal)

find_package(GTest REQUIRED)

#add_executable(dbknife tools/dbknife_main.cpp tools/dbknife.cpp 
#	unittest/google_test.cpp)
#target_include_directories(dbknife 
#	PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS}
#	${PostgreSQL_INCLUDE_DIRS})
#target_link_libraries(dbknife
#	-lmysqlcppconn
#	)

add_executable(fileknife tools/fileknife_main.cpp tools/fileknife.cpp 
	unittest/fileknife_test.cpp
	${OS_UNITTEST_SOURCES} ${BASIC_UNITTEST_SOURCES})
target_link_libraries(fileknife ${CPPKNIFE_LIBS})

#add_executable(geoknife tools/geoknife_main.cpp tools/geoknife.cpp 
#	unittest/GpsTranslator_test.cpp
#	${GEO_UNITTEST_SOURCES})
#target_include_directories(geoknife PRIVATE /usr/include/gdal)
#target_link_libraries(geoknife ${CPPKNIFE_LIBS}	gdal)

add_executable(sesknife tools/sesknife_main.cpp tools/sesknife.cpp 
	unittest/sesknife_test.cpp
	${TEXT_UNITTEST_SOURCES}
	)
target_compile_options(sesknife PRIVATE -Wall)
target_link_libraries(sesknife ${CPPKNIFE_LIBS})

add_executable(netknife tools/netknife_main.cpp tools/netknife.cpp 
	unittest/Agents_test.cpp
	${TEXT_UNITTEST_SOURCES}
	)
target_compile_options(netknife PRIVATE -Wall)
target_link_libraries(netknife ${CPPKNIFE_LIBS})

add_executable(textknife tools/textknife_main.cpp tools/textknife.cpp 
	unittest/textknife_test.cpp unittest/NodeJson_test.cpp
	${TEXT_UNITTEST_SOURCES})
target_link_libraries(textknife ${CPPKNIFE_LIBS})

# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
#target_include_directories(textknife PUBLIC "${PROJECT_BINARY_DIR}")
