### Basic cmake setup ###

cmake_minimum_required(VERSION 3.11)

project(Silent_Pass)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/staticlibs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-sign")

# To avoid libxml xmlFree error
# https://stackoverflow.com/questions/4984853/mingw-libxml2-issue
# https://github.com/msys2/MSYS2-packages/issues/171
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DIN_LIBXML")

# For valgrind line numbers
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

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

### Includes ### 

include_directories(includes includes/linux includes/utilities includes/win32)

### Submodules ###
include_directories(src/utilities/Silent_Log)
include_directories(src/utilities/Silent_OpenSSL/includes)

set(SUBMODULES_SOURCES src/utilities/Silent_Log/log.c)
set(SUBMODULES_SOURCES ${SUBMODULES_SOURCES} src/utilities/Silent_OpenSSL/src/s_aead.c src/utilities/Silent_OpenSSL/src/s_aes.c src/utilities/Silent_OpenSSL/src/s_base64.c src/utilities/Silent_OpenSSL/src/s_pbkdf.c src/utilities/Silent_OpenSSL/src/s_digest.c src/utilities/Silent_OpenSSL/src/s_curve.c)

set(SOURCES ${SOURCES} ${SUBMODULES_SOURCES})
message(${SOURCES})

### Sources ###

set(SOURCES ${SOURCES} src/chrome.c src/firefox.c src/filezilla.c src/main.c src/git.c)

set(UTILITIES_SOURCES src/utilities/functions.c src/utilities/json.c src/utilities/xml.c src/utilities/Silent_Log/log.c)
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

### External dependencies ###

option(ARGTABLE3_ENABLE_TESTS OFF)
option(ARGTABLE3_ENABLE_EXAMPLES OFF)
add_subdirectory("extern/argtable3")

option(ENABLE_CJSON_TEST OFF)
add_subdirectory("extern/cJSON")
include_directories(cjson PUBLIC  "extern/cJSON")

add_library(libyuarel "extern/libyuarel/yuarel.c")
target_include_directories(libyuarel PUBLIC  "extern/libyuarel")

add_library(iniparser "extern/iniparser/src/dictionary.c" "extern/iniparser/src/iniparser.c")
target_include_directories(iniparser PUBLIC "extern/iniparser/src")

target_link_libraries(Silent_Pass
    argtable3
    cjson
    libyuarel
    iniparser)

### Other libs ###

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

### Linux libs ###
if(UNIX)
	find_package(OpenSSL REQUIRED)
	find_package(Libsecret REQUIRED)
	find_package(NSS REQUIRED) 
	find_package(LibXml2 REQUIRED)
    find_package(SQLite3 REQUIRED)

	include_directories(
        ${OPENSSL_INCLUDE_DIRS}
        ${LIBSECRET_INCLUDE_DIRS}
        ${NSS_INCLUDE_DIRS}
        ${LIBXML2_INCLUDE_DIRS}
        ${SQLite3_INCLUDE_DIRS})
	target_link_libraries(Silent_Pass 
        ${OPENSSL_LIBRARIES} 
        ${NSS_LIBRARIES}
        ${LIBSECRET_LIBRARIES}
        ${LIBXML2_LIBRARIES}
        ${SQLite3_LIBRARIES}
        dl
        m
        pthread)
endif(UNIX)

### Windows libs ###
#if(WIN32)
#	set(OPENSSL_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libcrypto_s.a)
#	set(LIBXML2_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libxml2.a)
#	set(LIBZ_SOURCES ${CMAKE_SOURCE_DIR}/lib/win32/libz.a)
#
#	target_link_libraries(Silent_Pass ${LIBXML2_SOURCES})
#	target_link_libraries(Silent_Pass ${LIBZ_SOURCES})
#	target_link_libraries(Silent_Pass ${OPENSSL_SOURCES} crypt32 ws2_32 gdi32 m pthread)
#endif(WIN32)
