# hkdaemon/hkdaemon
cmake_minimum_required (VERSION 3.22)

# Get a list of all code files matching GLOB expressions
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.h*"
)
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"*.c*"
)

# Generate resources
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rc")
#	make a version header
MAKE_VERSION_HEADER("${CMAKE_CURRENT_BINARY_DIR}/rc/version.h" "hkdaemon" "${hkdaemon_VERSION}")

include(CopyrightMaker)
MAKE_COPYRIGHT_NOTICE(hkdaemon_COPYRIGHT "radj307" "2022")
MAKE_COPYRIGHT_HEADER_FROM_NOTICE("${CMAKE_CURRENT_BINARY_DIR}/rc/copyright.h" "${hkdaemon_COPYRIGHT}")
include(ResourceMaker)
MAKE_STRINGRC_VERSIONINFO(
	hkdaemon_VERSIONINFO
	"${hkdaemon_VERSION}"
	"${hkdaemon_COPYRIGHT}"
	"radj307"
	"hkdaemon"
	"Volume Control Hotkey Daemon responsible for detecting hotkey presses and executing the associated actions."
)
MAKE_RESOURCE("${CMAKE_CURRENT_BINARY_DIR}/rc/hkdaemon.rc" "${hkdaemon_VERSIONINFO}")

file(GLOB RESOURCES
	CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_BINARY_DIR}/rc/*"
)

# Create the executable
add_executable(hkdaemon "${SRCS}" "${RESOURCES}")

# Configure the executable
target_sources(hkdaemon PRIVATE "${HEADERS}")

set_property(TARGET hkdaemon PROPERTY CXX_STANDARD 20)
set_property(TARGET hkdaemon PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET hkdaemon PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(hkdaemon PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_options(hkdaemon PRIVATE "/Zc:__cplusplus")

include(FetchContent)
FetchContent_Declare(nlohmann_json
	GIT_REPOSITORY	https://github.com/nlohmann/json.git
	GIT_TAG			develop
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(hkdaemon PRIVATE TermAPI filelib nlohmann_json::nlohmann_json)
