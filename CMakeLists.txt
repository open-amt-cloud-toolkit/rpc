cmake_minimum_required (VERSION 3.1)

project (rpc VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 11)

# RPC version info
configure_file(version.h.in
               version.h)
include_directories(${PROJECT_BINARY_DIR})

# TODO: figure out how to read the LMS version from repo like the main lms CMakeLists.txt
set (LMS_VERSION_STRING 1932.0.0.0)

# Compiler settings [Obtained from CmakeLists.txt for lms]
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
  #add_definitions (/D UNICODE /D _UNICODE)
  add_definitions (/D UNICODE /D _UNICODE  /D_NO_ASYNCRTIMP /D_ASYNCRT_EXPORT /D_NO_PPLXIMP /DWIN32 /DMBCS /D_USRDLL /DCPPREST_EXCLUDE_COMPRESSION /D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_compile_options ($<$<CONFIG:Release>:/O2>)
  add_compile_options (/MT$<$<CONFIG:Debug>:d>)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif (UNIX)

# Add  MicroLMS directly to our build. This adds
# the following targets: MicroLMS
add_subdirectory(MicroLMS)
#add_dependencies(rpc MicroLMS)

if (UNIX)

# Find threads [unix it pthreads]
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Find Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system REQUIRED)

# Find OpenSSL
find_package(OpenSSL)

# Find ZLIB
find_package(ZLIB)

# Download and build CppRestSDK, If GIT_TAG is changed then need to delete cpprestsdk-prefix because UPDATE_COMMAND is set to ""
include(ExternalProject)
ExternalProject_Add(cpprestsdk
  GIT_REPOSITORY    https://github.com/Microsoft/cpprestsdk.git 
  GIT_TAG           v2.10.16
  CMAKE_ARGS        -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=0 -DBUILD_SAMPLES=OFF -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=<SOURCE_DIR>/../../install
  TEST_COMMAND      ""
  UPDATE_COMMAND    ""
)
ExternalProject_Get_Property(cpprestsdk SOURCE_DIR)
set(CPPRESTSDK_LIBARIES ${SOURCE_DIR}/../../install/lib/)
set(CPPRESTSDK_INCLUDE_DIR ${SOURCE_DIR}/../../install/include/)

add_library(cpprest INTERFACE)
target_link_libraries(cpprest INTERFACE ${CPPRESTSDK_LIBARIES}/libcpprest.a  OpenSSL::SSL OpenSSL::Crypto ${Boost_LIBRARIES} Threads::Threads ZLIB::ZLIB)
target_include_directories(cpprest INTERFACE ${CPPRESTSDK_INCLUDE_DIR})

else (UNIX)

# CppRestSDK
find_package(cpprestsdk CONFIG REQUIRED)

endif (UNIX)

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
  lms.h
  lms.cpp
  main.cpp
)

target_include_directories(rpc PUBLIC
  "MicroLMS/heci"
)

if (UNIX)

add_dependencies(rpc cpprestsdk)

target_link_libraries (rpc PRIVATE 
  MicroLMS
  cpprest
)

else (UNIX)

add_dependencies(rpc MicroLMS )

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
