### Basic cmake setup ###

cmake_minimum_required(VERSION 3.2.1)

project(Silent_Pass)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-sign")

### Git Submodule management ###
find_package(Git REQUIRED)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if(GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
				WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/src/utilities/Silent_Log/log.c")
	message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

### Includes ### 

include_directories(includes includes/linux includes/utilities includes/win32 includes/lib includes/lib/argtable3 includes/lib/iniparser includes/lib/libyuarel includes/lib/cJSON includes/lib/sqlite3 includes/lib/nss includes/lib/nspr src/utilities/Silent_Log)

### Sources ###

set(SOURCES src/chrome.c src/firefox.c src/filezilla.c src/main.c src/git.c)

set(UTILITIES_SOURCES src/utilities/functions.c src/utilities/json.c src/utilities/base64.c src/utilities/xml.c src/utilities/Silent_Log/log.c)
set(SOURCES ${SOURCES} ${UTILITIES_SOURCES})

if(WIN32)
	set(SOURCES ${SOURCES} src/win32/chrome.c src/win32/explorer.c src/win32/firefox.c src/win32/specific.c src/win32/filezilla.c src/win32/git.c)
endif(WIN32)

if(UNIX)
	set(SOURCES ${SOURCES} src/linux/chrome.c src/linux/firefox.c src/linux/specific.c src/linux/filezilla.c src/linux/git.c)
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

set(YUAREL_SOURCES "lib/src/yuarel.c")
add_library(yuarel STATIC ${YUAREL_SOURCES})
#install(TARGETS yuarel DESTINATION staticlibs)

target_link_libraries(Silent_Pass cjson sqlite3 argtable3 yuarel)


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
	set(LIBZ_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libz.a)

	target_link_libraries(Silent_Pass ${LIBXML2_SOURCES})
	target_link_libraries(Silent_Pass ${LIBZ_SOURCES})
	target_link_libraries(Silent_Pass ${OPENSSL_SOURCES} crypt32 ws2_32 gdi32 m pthread)
endif(WIN32)
