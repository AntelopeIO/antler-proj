# common.cmake
#
# This file sets the following values:
#
#
#   CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE}
#   CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE}
#   CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE}
#
#   CMAKE_DEBUG_POSTFIX
#
#   CMAKE_C_FLAGS
#   CMAKE_CXX_FLAGS
#

# Basic sanity check:
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Avoid building inside the source tree. Create a `build` directory?")
endif()

set(CMAKE_DEBUG_POSTFIX d)  # appends d to libraries.

if(MSVC)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Bin)

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin)  # for .dll files
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Bin)

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Bin)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Bin)

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Bin)  # for .so files
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Bin)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Bin)

  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Lib)  # for .a files
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Lib)
endif()


if( APPLE OR UNIX OR MINGW )

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # go ahead and search for 64 bit libs
    set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS ON)
  endif()

  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")

  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage")
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Colorize output
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always" )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wctor-dtor-privacy -Wnon-virtual-dtor" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wredundant-decls")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wlogical-op")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "8.3.0")
      # do not enable in earlier versions due to gcc defect 83591
      #   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83591
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wduplicated-branches" )
    endif()
  endif()

elseif(MSVC)
  # Increase warning level and disable warnings on an individual basis.
  string(REPLACE "/W3" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /WX" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX" )

  #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4275" ) # example, also provide REASON for disabling.

  # Enable multi processor compilations as default:
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP ")
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP ")

  # Set compatibility to Windows 10.
  add_definitions(-DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00)

  add_definitions(-DCRT_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS) # Prevent various warnings.
  add_definitions(-DNOMINMAX)  # `windows.h` functions interfere with std min & max in `algorithm`.
endif()


SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG> )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>:NDEBUG> )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:RelWithDebInfo>:NDEBUG> )
SET_PROPERTY(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:MinSizeRel>:NDEBUG> )

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-D_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
  add_definitions(-DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
  add_definitions(-DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
  #
else()
  # Seems the build type isn't set. Tell the users. Unless it's visual studio.
  if(NOT MSVC)
    message(FATAL_ERROR "\nSet CMAKE_BUILD_TYPE to one of: Release, Debug, RelWithDebInfo, or MinSizeRel\n  e.g. `cmake .. -DCMAKE_BUILD_TYPE=Debug`\n" )
  endif()
endif()
