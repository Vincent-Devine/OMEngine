add_library(OMEngine)

set(SOURCES
	Source/HelloWorld.cpp
)

set(HEADERS
	Source/HelloWorld.hpp
)

target_sources(OMEngine
	PRIVATE 
		${SOURCES}

	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			${CMAKE_CURRENT_SOURCE_DIR}/Source
		FILES 
			${HEADERS}

	INTERFACE
		Source/OMEngine.hpp
)

target_compile_definitions(OMEngine
	PUBLIC
		$<$<CONFIG:Debug>:OM_DEBUG>
		$<$<CONFIG:RelWithDebInfo>:OM_DEVELOPMENT>
		$<$<CONFIG:Release>:OM_RELEASE>
)