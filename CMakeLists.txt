cmake_minimum_required(VERSION 3.2.1)

project(Silent_Pass)

include_directories(includes)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



### Sources ###

set(SOURCES src/chrome.c src/firefox.c src/main.c src/utilities/functions.c src/utilities/json.c)

if(WIN32)
	set(SOURCES ${SOURCES} src/win32/chrome.c src/win32/explorer.c src/win32/firefox.c src/win32/specific.c)
endif(WIN32)

if(UNIX)
	set(SOURCES ${SOURCES} src/linux/chrome.c src/linux/firefox.c src/linux/specific.c)
endif(UNIX)

add_executable(Silent_Pass ${SOURCES})



### Static libs ###

set(ARGTABLE_SOURCES "lib/src/argtable3.c")
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})
install(TARGETS argtable3 DESTINATION staticlibs)

set(CJSON_SOURCES "lib/src/cJSON.c")
add_library(cjson STATIC ${CJSON_SOURCES})
install(TARGETS cjson DESTINATION staticlibs)

set(SQLITE3_SOURCES "lib/src/sqlite3.c")
add_library(sqlite3 STATIC ${SQLITE3_SOURCES})
install(TARGETS sqlite3 DESTINATION staticlibs)

target_link_libraries(Silent_Pass cjson sqlite3 argtable3)


### LIBS ###

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(NSS REQUIRED) 
include_directories(Silent_Pass ${NSS_INCLUDE_DIRS})
message(STATUS "${NSS_INCLUDE_DIRS}")

### Linux libs ###
if(UNIX)
	set(INIPARSER_SOURCES "lib/src/iniparser.c" "lib/src/dictionary.c")
	add_library(iniparser STATIC ${INIPARSER_SOURCES})
	install(TARGETS iniparser DESTINATION staticlibs)

	find_package(OpenSSL REQUIRED)
	find_package(Libsecret REQUIRED)

	#find_library(NSS_LIB NAMES nss nss3)
	#find_library(SECRET_LIB NAMES secret secret-1)

	include_directories(${OPENSSL_INCLUDE_DIRS} ${LIBSECRET_INCLUDE_DIRS})
	target_link_libraries(Silent_Pass ${OPENSSL_LIBRARIES} ${NSS_LIBRARIES} ${LIBSECRET_LIBRARIES} iniparser dl m pthread)
endif(UNIX)

### Windows libs ###
if(WIN32)
	set(OPENSSL_SOURCES "lib/win32/libcrypto_s.a")
	target_link_libraries(Silent_Pass ${CMAKE_SOURCE_DIR}/${OPENSSL_SOURCES} crypt32 ws2_32 gdi32 m pthread)
endif(WIN32)
