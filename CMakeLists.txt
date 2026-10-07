cmake_minimum_required(VERSION 3.30)
project(OMEngine)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(OMEngine)
target_sources(OMEngine
	PRIVATE
		OMEngine/Source/HelloWorld.cpp

	PUBLIC
		FILE_SET HEADERS
		BASE_DIRS
			OMEngine/Source
		FILES
			OMEngine/Source/HelloWorld.hpp

	INTERFACE
		OMEngine/Source/OMEngine.hpp
)

add_executable(OMEditor)
target_sources(OMEditor 
	PRIVATE
		OMEditor/Source/main.cpp
)
target_link_libraries(OMEditor
	PRIVATE
		OMEngine
)