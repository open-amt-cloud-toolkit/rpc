cmake_minimum_required (VERSION 3.1)

project (rpc VERSION 1.2.0)

set (CMAKE_CXX_STANDARD 11)

# RPC version info
configure_file(version.h.in version.h)
include_directories(${PROJECT_BINARY_DIR})

# Common compiler settings
string(APPEND CMAKE_CXX_FLAGS_DEBUG " -DDEBUG -D_DEBUG")
string(APPEND CMAKE_C_FLAGS_DEBUG " -DDEBUG -D_DEBUG")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

if (UNIX)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack -z relro -z now")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -z noexecstack -z relro -z now")

  #CMake issue #14983
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

  #Secure library usage and secure compile flags
  add_definitions (-fstack-protector-strong -D_FORTIFY_SOURCE=2 -O2 -Wformat -Wformat-security)
  add_definitions (-fno-strict-overflow -fno-delete-null-pointer-checks -fwrapv -fpermissive)
else (UNIX)
  add_definitions (/GS /sdl)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NXCompat /DynamicBase")
  add_definitions (/D UNICODE /D _UNICODE  /D_NO_ASYNCRTIMP /D_ASYNCRT_EXPORT /D_NO_PPLXIMP /DWIN32 /DMBCS /D_USRDLL /DCPPREST_EXCLUDE_COMPRESSION /D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_compile_options ($<$<CONFIG:Release>:/O2>)
  add_compile_options (/MT$<$<CONFIG:Debug>:d>)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif (UNIX)

# Add  MicroLMS directly to our build. This adds
# the following targets: MicroLMS
add_subdirectory(MicroLMS)

# CppRestSDK
find_package(cpprestsdk CONFIG REQUIRED)

# GoogleTest
# Download and unpack googletest at configure time
configure_file(googletest.cmake.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# add the test executable
add_executable(rpctest
  test.cpp
  utils.cpp
)

target_link_libraries(rpctest
  gtest_main
  cpprestsdk::cpprest
  cpprestsdk::cpprestsdk_zlib_internal
  cpprestsdk::cpprestsdk_boost_internal
  cpprestsdk::cpprestsdk_openssl_internal
)

# ccu-poc
add_executable (rpc
  info.h
  info.cpp
  args.h
  args.cpp
  usage.h
  usage.cpp
  port.h
  utils.h
  utils.cpp
  network.h
  network.cpp
  commands.h
  commands.cpp
  activation.h
  activation.cpp
  heartbeat.h
  heartbeat.cpp
  lms.h
  lms.cpp
  main.cpp
)

target_include_directories(rpc PUBLIC
  "MicroLMS/heci"
)

add_dependencies(rpc MicroLMS)

if (UNIX)

target_link_libraries (rpc PRIVATE 
  MicroLMS
  cpprestsdk::cpprest 
  cpprestsdk::cpprestsdk_zlib_internal 
  cpprestsdk::cpprestsdk_boost_internal 
  cpprestsdk::cpprestsdk_openssl_internal
)

else (UNIX)

target_link_libraries (rpc PRIVATE 
  MicroLMS
  cpprestsdk::cpprest
  cpprestsdk::cpprestsdk_zlib_internal
  cpprestsdk::cpprestsdk_boost_internal
  cpprestsdk::cpprestsdk_brotli_internal
  DbgHelp.lib
  Iphlpapi.lib
  Setupapi.lib
  ws2_32.lib
  Psapi.lib
  Crypt32.lib
  Wintrust.lib
  Version.lib
  Wtsapi32.lib
  Gdiplus.lib
  Userenv.lib
)

endif (UNIX)
