cmake_minimum_required(VERSION 3.10)

# Set some basic project attributes
project (ccppknife
	VERSION 0.1
	DESCRIPTION "A C++ library with some applications")

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

include(FindPkgConfig)

set(BASIC_SOURCES basic/BaseRandom.cpp basic/CharRandom.cpp basic/Logger.cpp basic/PortableRandom.cpp
	basic/StringTool.cpp basic/TimeTool.cpp basic/InternalError)

set(BASIC_UNITTEST_SOURCES unittest/BaseRandom_test.cpp unittest/CharRandom_test.cpp unittest/Logger_test.cpp
	unittest/PortableRandom_test.cpp unittest/StringTool_test.cpp unittest/TimeTool_test.cpp)

set(CORE_SOURCES core/ByteStorage.cpp core/KissRandom.cpp core/Storage.cpp)

set(CORE_UNITTEST_SOURCES unittest/ByteStorage_test.cpp unittest/KissRandom_test.cpp unittest/Storage_test.cpp)

set(DB_SOURCES db/MySql.cpp db/Postgres.cpp db/SqlDriver.cpp)

set(DB_UNITTEST_SOURCES unittest/MySql_test.cpp unittest/Postgres_test.cpp unittest/SqlDriver_test.cpp)

set(GEO_SOURCES geo/GeoException.cpp geo/GeoHelper.cpp geo/GpsTranslator.cpp geo/Point2D.cpp geo/Rectangle.cpp geo/Shape.cpp)

set(GEO_UNITTEST_SOURCES unittest/GeoException_test.cpp unittest/GeoHelper_test.cpp unittest/GpsTranslator_test.cpp unittest/Point2D_test.cpp
	unittest/Rectangle_test.cpp unittest/Shape_test.cpp)

set(OS_SOURCES os/File.cpp os/FileTool.cpp os/LineAgent.cpp os/OsException.cpp os/Path.cpp os/Process.cpp
	os/Traverser.cpp)

set(OS_UNITTEST_SOURCES unittest/File_test.cpp unittest/FileDb_test.cpp unittest/FileTool_test.cpp unittest/LineAgent_test.cpp unittest/OsException_test.cpp unittest/Path_test.cpp unittest/Process_test.cpp
	unittest/Traverser_test.cpp)

set(TEXT_SOURCES text/Configuration.cpp text/CsvFile.cpp text/FunctionEngine.cpp text/JsonReader.cpp text/LineList.cpp
	text/LineReader.cpp text/LinesStream.cpp text/Matcher.cpp text/Parser.cpp text/ParserError.cpp text/Script.cpp
	text/SearchEngine.cpp text/StringList.cpp)

set(TEXT_UNITTEST_SOURCES unittest/Configuration_test.cpp unittest/CsvFile_test.cpp unittest/FunctionEngine_test.cpp unittest/JsonReader_test.cpp unittest/LineList_test.cpp
	unittest/LineReader_test.cpp unittest/LinesStream_test.cpp unittest/Matcher_test.cpp unittest/Parser_test.cpp unittest/ParserError_test.cpp unittest/Script_test.cpp
	unittest/SearchEngine_test.cpp unittest/StringList_test.cpp)

set(TOOLS_SOURCES tools/ArgumentParser.cpp tools/ToolsCommons.cpp tools/VeilEngine.cpp tools/SecretConfiguration.cpp)

add_library(cppknifebasic SHARED ${BASIC_SOURCES})

add_library(cppknifecore SHARED ${CORE_SOURCES})

add_library(cppknifedb SHARED ${DB_SOURCES})

add_library(cppknifetext SHARED ${TEXT_SOURCES})

add_library(cppknifeos SHARED ${OS_SOURCES})

add_library(cppknifegeo SHARED ${GEO_SOURCES})
target_include_directories(cppknifegeo PRIVATE /usr/include/gdal gdal)

add_library(cppknife SHARED ${BASIC_SOURCES} ${CORE_SOURCES} ${OS_SOURCES} ${GEO_SOURCES} ${TEXT_SOURCES} ${TOOLS_SOURCES})
target_include_directories(cppknife PRIVATE /usr/include/gdal gdal)

add_library(cppknifetools SHARED ${TOOLS_SOURCES})

add_library(unittestbasic OBJECT ${BASIC_UNITTEST_SOURCES})

add_library(unittestcore OBJECT ${CORE_UNITTEST_SOURCES})

add_library(unittesttext OBJECT ${TEXT_UNITTEST_SOURCES})

add_library(unittestos OBJECT ${OS_UNITTEST_SOURCES})

add_library(unittestdb OBJECT ${DB_UNITTEST_SOURCES})

add_library(unittestgeo OBJECT ${GEO_UNITTEST_SOURCES})
target_include_directories(unittestgeo PRIVATE /usr/include/gdal gdal)

set_target_properties(cppknifebasic cppknifecore cppknifedb cppknifetext cppknifeos cppknifetools
	cppknifedb cppknifegeo
	unittestbasic unittestcore unittesttext unittestos unittestdb
	PROPERTIES POSITION_INDEPENDENT_CODE True)

find_package(GTest REQUIRED)

add_executable(fileknife tools/fileknife_main.cpp tools/fileknife.cpp 
	unittest/google_test.cpp unittest/fileknife_test.cpp)
target_link_libraries(fileknife
	cppknifebasic cppknifecore cppknifetext cppknifeos cppknifetools
	unittestbasic unittestcore unittesttext unittestos
	GTest::gtest GTest::gtest_main)

add_executable(geoknife tools/geoknife_main.cpp tools/geoknife.cpp 
	unittest/google_test.cpp unittest/GpsTranslator_test)
target_include_directories(geoknife PRIVATE /usr/include/gdal)
target_link_libraries(geoknife
	cppknifebasic cppknifecore cppknifetext cppknifeos cppknifetools cppknifegeo
	unittestbasic unittestcore unittesttext unittestos
	GTest::gtest GTest::gtest_main
	gdal)

add_executable(textknife tools/textknife_main.cpp tools/textknife.cpp 
	unittest/google_test.cpp unittest/textknife_test.cpp)
target_link_libraries(textknife
	cppknifebasic cppknifecore cppknifetext cppknifeos cppknifetools
	unittestbasic unittestcore unittesttext unittestos
	GTest::gtest GTest::gtest_main)

add_executable(sesknife tools/sesknife_main.cpp tools/sesknife.cpp 
	unittest/google_test.cpp unittest/sesknife_test.cpp)
target_link_libraries(sesknife
	cppknifebasic cppknifecore cppknifetext cppknifeos cppknifetools
	unittestbasic unittestcore unittesttext unittestos
	GTest::gtest GTest::gtest_main)

add_executable(dbknife tools/dbknife_main.cpp tools/dbknife.cpp 
	unittest/google_test.cpp)
target_include_directories(dbknife 
	PRIVATE ${LIBMYSQLCLIENT_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(dbknife
	cppknifebasic cppknifecore cppknifetext cppknifeos cppknifetools cppknifedb
	unittestbasic unittestcore unittesttext unittestos
	GTest::gtest GTest::gtest_main
	-lmysqlcppconn
	-lpqxx -lpq)


# Create a simple configuration header
configure_file(config.h.in config.h)

# Include the configuration header in the build
#target_include_directories(textknife PUBLIC "${PROJECT_BINARY_DIR}")
