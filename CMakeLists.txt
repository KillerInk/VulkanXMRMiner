#
# Cryptonight Vulkan Mining Software
#
cmake_minimum_required(VERSION 3.5.0)

project(VulkanXMRMiner)

# enforce C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)

# change the processor target here if needed
set(ARCH "native")

if(MSVC)
add_definitions(-D__MINGW32__)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
add_definitions(-DMSVC)
endif()

# change gcc for Windows 64 build
if(NOT MSVC)
if(WIN32)
	SET(CMAKE_SYSTEM_NAME Windows)
	SET(CMAKE_C_COMPILER x86_64-w64-mingw32-g++-win32)
	SET(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-win32)
	SET(CMAKE_FIND_ROOT_PATH /usr/bin/)
endif()
endif()

# set AVX for debug,release,unspecified
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS_DEBUG "-g3 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	set(CMAKE_C_FLAGS_DEBUG  "-g3 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
else()
	set(CMAKE_CXX_FLAGS_RELEASE "-O1 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	set(CMAKE_C_FLAGS_RELEASE "-O1 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	# if not build specified
	set(CMAKE_CXX_FLAGS "-O1 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	set(CMAKE_C_FLAGS "-O1 -mavx -march=${ARCH}" CACHE STRING "compile flags" FORCE)
endif()

# Look for vulkan SDK
# For a Windows build on Linux, install the Vulkan SDK with wine, and set VULKAN_SDK to ..../.wine/drive_c/VulkanSDK/1.x.y
# For Linux, install libvulkan-dev
if(MSVC)
find_path(VulkanIncludes Include/vulkan/vulkan.h PATHS $ENV{VK_SDK_PATH} )
else()
find_path(VulkanIncludes vulkan/vulkan.h PATHS ENV VULKAN_SDK ENV /usr )
endif()
if(NOT VulkanIncludes)
  message(FATAL_ERROR "Can't find folder containing vulkan.h")
endif()

if (SOFT_AES)
	message("Build without aesi-ni and without avx")
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_CXX_FLAGS_RELEASE "-g3 -DSOFT_AES -march=${ARCH}" CACHE STRING "compile flags" FORCE)
		set(CMAKE_C_FLAGS_RELEASE "-g3 -DSOFT_AES  -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "-O1 -DSOFT_AES -march=${ARCH}" CACHE STRING "compile flags" FORCE)
		set(CMAKE_C_FLAGS_RELEASE "-O1 -DSOFT_AES  -march=${ARCH}" CACHE STRING "compile flags" FORCE)
		# if not build specified
		set(CMAKE_CXX_FLAGS "-O1 -DSOFT_AES  -march=${ARCH}" CACHE STRING "compile flags" FORCE)
		set(CMAKE_C_FLAGS "-O1 -DSOFT_AES  -march=${ARCH}" CACHE STRING "compile flags" FORCE)
	endif()
endif()
	
# Add Include/Lib (Windows) and include/lib (Linux)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
if(MSVC)
	set(ADDITIONAL_LIBRARIES vulkan-1 ws2_32)
else()
	set(ADDITIONAL_LIBRARIES vulkan-1 pthread ws2_32)
endif()
	include_directories("$ENV{VULKAN_SDK}/Include")
	link_directories("$ENV{VULKAN_SDK}/Lib")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(ADDITIONAL_LIBRARIES vulkan pthread)
	include_directories("$ENV{VULKAN_SDK}/include")
	link_directories("$ENV{VULKAN_SDK}/lib")
endif()
                      
file(GLOB SRCFILES_CPP "src/*.cpp")
set_source_files_properties(${SRCFILES_CPP} PROPERTIES LANGUAGE CXX)

add_executable(miner ${SRCFILES_CPP})
target_link_libraries(miner ${ADDITIONAL_LIBRARIES})

# copy SPIR-V programs, GLSL compiler (cryptonightR), and Windows DLLs
add_custom_command(TARGET miner PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/spirv spirv)
add_custom_command(TARGET miner PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/redist .)
add_custom_command(TARGET miner POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/build/miner.exe ${CMAKE_SOURCE_DIR}/build/miner.exe)

# make the download file for Linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	add_custom_target(zip ALL DEPENDS miner COMMAND
    ${CMAKE_COMMAND} -E tar "zcvf" "vulkanXMRMiner.tgz"
    "miner"
    "spirv"
    "glslangValidator"
    )
endif()

if(NOT MSVC)
# same as Linux, but add mingw runtime DLLs.
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	add_custom_target(zip ALL DEPENDS miner COMMAND
    ${CMAKE_COMMAND} -E tar "cvf" "vulkanXMRMiner.zip" --format=zip
    "miner.exe"
    "spirv"
    "glslangValidator.exe"
    "libgcc_s_seh-1.dll"
    "libstdc++-6.dll"
    "libwinpthread-1.dll"
    )
endif()
endif()

