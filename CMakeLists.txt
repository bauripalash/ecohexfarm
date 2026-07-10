cmake_minimum_required(VERSION 3.24...3.30)
project(ecohex)

include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
		"Debug"
		)
endif()

set(IS_OS_WIN FALSE)
set(IS_OS_LINUX FALSE)
set(IS_OS_MACOS FALSE)
set(IS_OS_WEB FALSE)

set(IS_GCC FALSE)
set(IS_CLANG FALSE)
set(IS_MSVC FALSE)
set(IS_EMCC FALSE)

set(IS_DEBUG_BUILD FALSE)
set(IS_RELEASE_BUILD FALSE)


# Dependencies
set(RAYLIB_VERSION 6.0)
set(RAYLIB_COMMIT caadb48e259028233515777a3e6402040c497309)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz"
	OVERRIDE_FIND_PACKAGE
)

#FIND_PACKAGE_ARGS
FetchContent_MakeAvailable(raylib)


# Our Project
add_executable(${PROJECT_NAME})
add_subdirectory(src)

# ================== OS CHECK ==================

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows" OR
	CMAKE_SYSTEM_NAME STREQUAL "CYGWIN" OR
	CMAKE_SYSTEM_NAME STREQUAL "MSYS")

	set(IS_OS_WIN TRUE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set(IS_OS_LINUX TRUE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(IS_OS_MACOS TRUE)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
	set(IS_OS_WEB TRUE)
	set(IS_EMCC TRUE)
endif()

# ================== END OS CHECK ==================


# ================== COMPILER CHECK ==================

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	set(IS_GCC TRUE)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
        CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
	set(IS_CLANG TRUE)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
	set(IS_MSVC TRUE)
elseif(CMAKE_C_COMPILER_ID STREQUAL "TinyCC")
    set(IS_GFX_BUILD NO)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(IS_DEBUG_BUILD TRUE)
else()
	set(IS_RELEASE_BUILD TRUE)
endif()

# ================== END COMPILER CHECK ==================


if(IS_DEBUG_BUILD)
	target_compile_definitions(${PROJECT_NAME} PRIVATE MI_TRACK_ASAN=1)
	add_compile_definitions(HEXGAME_PB_BUILD_DEBUG)
else()
	add_compile_definitions(HEXGAME_PB_BUILD_RELEASE)
endif()


if (IS_OS_WIN)
	add_compile_definitions(HEXGAME_PB_OS_WIN)
elseif (IS_OS_LINUX)
	add_compile_definitions(HEXGAME_PB_OS_LINUX)
elseif (IS_OS_MACOS)
	add_compile_definitions(HEXGAME_PB_OS_MAC)
elseif (IS_OS_WEB)
	add_compile_definitions(HEXGAME_PB_OS_WEB)
endif()



if (IS_OS_LINUX AND IS_DEBUG_BUILD)
	set(HEXGAME_PB_DEBUG_CFLAGS
		"-g3"
		"-pedantic"
		"-Wall"
		"-fsanitize=address"
		"-fsanitize=undefined"
		"-fsanitize=float-cast-overflow"
		"-fdiagnostics-color=always"
	)
	set(HEXGAME_PB_DEBUG_LINKER_FLAGS
		"-fsanitize=address"
		"-fsanitize=undefined"
		"-fsanitize=float-cast-overflow"
	)

	target_compile_options(${PROJECT_NAME} PRIVATE ${HEXGAME_PB_DEBUG_CFLAGS})
	target_link_options(${PROJECT_NAME} PRIVATE ${HEXGAME_PB_DEBUG_LINKER_FLAGS})
endif()



set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if ("${PLATFORM}" STREQUAL "Web")
    add_custom_command(
        TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/../resources
    )
    #DEPENDS ${PROJECT_NAME}
else()
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    )
    #DEPENDS ${PROJECT_NAME}
endif()


set(raylib_VERBOSE 1)

target_link_libraries(${PROJECT_NAME} raylib)

# Web Configurations
if ("${PLATFORM}" STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
    target_link_options(${PROJECT_NAME} PUBLIC
		-sUSE_GLFW=3
		--preload-file resources
		--shell-file ${CMAKE_SOURCE_DIR}/src/minshell.html
		)
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
