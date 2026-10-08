add_executable(OMEditor)

set(SOURCES
	Source/main.cpp
)

target_sources(OMEditor 
	PRIVATE	${SOURCES}
)

target_link_libraries(OMEditor
	PRIVATE
		OMEngine
)