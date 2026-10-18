add_library(OMEngine)

set(SOURCES
	Source/Core/Engine.cpp
	Source/Core/Log/Logger.cpp
)

set(HEADERS
	Source/Core/Engine.hpp
	Source/Core/Log/Logger.hpp
	Source/Core/Log/Assertion.hpp
	Source/Core/Types/BaseType.hpp
	Source/Utils/OMResult.hpp
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