set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32" CACHE STRING "c flags")

# https://cmake.org/cmake/help/v3.12/command/find_package.html
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB32_PATHS TRUE)
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS FALSE)

# https://github.com/i-rinat/apulse/issues/39
# https://stackoverflow.com/questions/44487053/set-pkg-config-path-in-cmake
set(ENV{PKG_CONFIG_PATH} "/usr/lib32/pkgconfig:/usr/lib/i386-linux-gnu/pkgconfig/")
