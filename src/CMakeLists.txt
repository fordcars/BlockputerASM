set(SOURCES
	main.cpp
	Assembler.cpp
)

set(HEADERS
    getopt.h
	Assembler.hpp
)

add_executable(
	BlockputerASM
	${SOURCES}
	${HEADERS}
)

target_include_directories(BlockputerASM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})