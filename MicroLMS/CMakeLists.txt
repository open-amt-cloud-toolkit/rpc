cmake_minimum_required (VERSION 3.4)

###########################################################
# Set project options
#

# set the project name
project(MicroLMS)

set (CMAKE_CXX_STANDARD 11)

set (CMAKE_POSITION_INDEPENDENT_CODE ON)

# set to ON to build library, OFF to build executable
set (BUILD_LIBRARY ON)

include_directories(
    ${PROJECT_BINARY_DIR} 
    MicroLMS 
    core
    heci)

###########################################################
# Compiler / linker options
#

if (UNIX)
#
# Linux Compiler and Linker Options
    
# Find threads [unix it pthreads]
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

if (${BUILD_LIBRARY})
add_definitions(-D BUILD_LIBRARY)
endif (${BUILD_LIBRARY})

add_definitions( -D_POSIX )

else (UNIX)
# Windows Compiler and Linker Options
if (${BUILD_LIBRARY})
add_definitions(/D BUILD_LIBRARY)
endif (${BUILD_LIBRARY})

add_definitions (/GS /GL /W3 /Gy /Zc:wchar_t /Zi  /Zc:inline /fp:precise /WX- /Zc:forScope /Gd /Oi /FC /EHsc /nologo )
add_definitions (/D WIN32 /D WIN64 /D NDEBUG /D _CONSOLE /D MICROSTACK_NO_STDAFX /D WINSOCK2 /D MICROSTACK_NOTLS /D _UNICODE /D UNICODE)
add_definitions (/errorReport:prompt /diagnostics:column)

add_compile_options ($<$<CONFIG:Release>:/O1>)
add_compile_options (/MT$<$<CONFIG:Debug>:d>)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Gm-")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd /Gm /Od /D _DEBUG /D MEMORY_CHECK /D __STDC__  /D _CRT_SECURE_NO_WARNINGS /D ILibWebServer_SESSION_TRACKING /Gd /MDd")

set (CMAKE_EXE_LINKER_FLAGS 
    "${CMAKE_EXE_LINKER_FLAGS} 
    /MANIFEST 
    /LTCG:incremental 
    /NXCOMPAT
    /DynamicBase
    /MACHINE:X64 
    /OPT:REF 
    /INCREMENTAL:NO
    /SUBSYSTEM:CONSOLE 
    /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" 
    /OPT:ICF 
    /ERRORREPORT:PROMPT 
    /NOLOGO 
    /TLBID:1"
)
endif (UNIX)

###########################################################
# Create the binary
#

if (UNIX)
#
# Linux Library and Executable Build
#

if (BUILD_LIBRARY)
# Create a library
add_library (
    MicroLMS
    STATIC
    MicroLMS/main.c 
    core/utils.c
    heci/HECILinux.c
    heci/LMEConnection.c
    heci/PTHICommand.c
    microstack/ILibAsyncServerSocket.c 
    microstack/ILibAsyncSocket.c 
    microstack/ILibLMS.c 
    microstack/ILibParsers.c 
    )

target_link_libraries (
    MicroLMS )

else (BUILD_LIBRARY)
# Create an executable
add_executable (
    MicroLMS
    MicroLMS/main.c 
    core/utils.c
    heci/HECILinux.c
    heci/LMEConnection.c
    heci/PTHICommand.c
    microstack/ILibAsyncServerSocket.c 
    microstack/ILibAsyncSocket.c 
    microstack/ILibLMS.c 
    microstack/ILibParsers.c 
    )

target_link_libraries (
    MicroLMS 
    PRIVATE
    pthread
    )

endif (BUILD_LIBRARY)


else (UNIX)
#
# Windows Library and Executable Build
#

if (BUILD_LIBRARY)
# Create a library
add_library (
    MicroLMS
    STATIC
    MicroLMS/main.c 
    MicroLMS/stdafx.cpp 
    core/utils.c
    heci/HECIWin.c
    heci/LMEConnection.c
    heci/PTHICommand.c
    microstack/ILibAsyncServerSocket.c 
    microstack/ILibAsyncSocket.c 
    microstack/ILibLMS.c 
    microstack/ILibParsers.c 
    )

target_link_libraries (
    MicroLMS )

else (BUILD_LIBRARY)
# Create an executable
add_executable (
    MicroLMS
    MicroLMS/main.c 
    MicroLMS/stdafx.cpp 
    core/utils.c
    heci/HECIWin.c
    heci/LMEConnection.c
    heci/PTHICommand.c
    microstack/ILibAsyncServerSocket.c 
    microstack/ILibAsyncSocket.c 
    microstack/ILibLMS.c 
    microstack/ILibParsers.c 
    )

target_link_libraries (
    MicroLMS 
    PRIVATE 
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

endif (BUILD_LIBRARY)












endif (UNIX)

