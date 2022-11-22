# hkdaemon/hkdaemon
cmake_minimum_required (VERSION 3.22)

# Get a list of all code files matching GLOB expressions
file(GLOB HEADERS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"include/*.h*"
)
file(GLOB SRCS
	RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
	CONFIGURE_DEPENDS
	"src/*.c*"
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

# Create the DLL
add_executable(hkdaemon "${SRCS}" "${RESOURCES}")

# Configure the DLL
target_sources(hkdaemon PRIVATE "${HEADERS}")

set_property(TARGET hkdaemon PROPERTY CXX_STANDARD 20)
set_property(TARGET hkdaemon PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET hkdaemon PROPERTY WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_include_directories(hkdaemon PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/rc")

target_link_libraries(hkdaemon PRIVATE shared)
