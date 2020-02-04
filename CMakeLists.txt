### Basic cmake setup ###

cmake_minimum_required(VERSION 3.2.1)

project(Silent_Pass)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


### Includes ### 

include_directories(includes includes/nss includes/nspr)


### Sources ###

set(SOURCES src/chrome.c src/firefox.c src/filezilla.c src/main.c src/utilities/functions.c src/utilities/json.c src/utilities/base64.c src/utilities/xml.c)

if(WIN32)
	set(SOURCES ${SOURCES} src/win32/chrome.c src/win32/explorer.c src/win32/firefox.c src/win32/specific.c src/win32/filezilla.c)
endif(WIN32)

if(UNIX)
	set(SOURCES ${SOURCES} src/linux/chrome.c src/linux/firefox.c src/linux/specific.c src/linux/filezilla.c)
endif(UNIX)

add_executable(Silent_Pass ${SOURCES})
install(TARGETS Silent_Pass DESTINATION bin)


### Libs ###

### Static common libs ###

set(ARGTABLE_SOURCES "lib/src/argtable3.c")
add_library(argtable3 STATIC ${ARGTABLE_SOURCES})
#install(TARGETS argtable3 DESTINATION staticlibs)

set(CJSON_SOURCES "lib/src/cJSON.c")
add_library(cjson STATIC ${CJSON_SOURCES})
#install(TARGETS cjson DESTINATION staticlibs)

set(SQLITE3_SOURCES "lib/src/sqlite3.c")
add_library(sqlite3 STATIC ${SQLITE3_SOURCES})
#install(TARGETS sqlite3 DESTINATION staticlibs)

target_link_libraries(Silent_Pass cjson sqlite3 argtable3)


### Other libs ###

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

### Linux libs ###
if(UNIX)
	set(INIPARSER_SOURCES "lib/src/iniparser.c" "lib/src/dictionary.c")
	add_library(iniparser STATIC ${INIPARSER_SOURCES})
	install(TARGETS iniparser DESTINATION staticlibs)

	find_package(OpenSSL REQUIRED)
	find_package(Libsecret REQUIRED)
	find_package(NSS REQUIRED) 
	find_package(LibXml2 REQUIRED)

	#find_library(NSS_LIB NAMES nss nss3)
	#find_library(SECRET_LIB NAMES secret secret-1)

	include_directories(${OPENSSL_INCLUDE_DIRS} ${LIBSECRET_INCLUDE_DIRS} ${NSS_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})
	target_link_libraries(Silent_Pass ${OPENSSL_LIBRARIES} ${NSS_LIBRARIES} ${LIBSECRET_LIBRARIES} ${LIBXML2_LIBRARIES} iniparser dl m pthread)
endif(UNIX)

### Windows libs ###
if(WIN32)
	set(OPENSSL_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libcrypto_s.a)
	set(LIBXML2_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libxml2.a)
	target_link_libraries(Silent_Pass ${OPENSSL_SOURCES} crypt32 ws2_32 gdi32 m pthread)
	target_link_libraries(Silent_Pass xml2 ${LIBXML2_SOURCES} iconv z)
endif(WIN32)
