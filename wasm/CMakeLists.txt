# Command to generate lexer using flex tool
set(FLEX_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
file(MAKE_DIRECTORY ${FLEX_BINARY_DIR})
set(FLEX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.lex")
set(FLEX_OUT "${FLEX_BINARY_DIR}/lexer_gen.cpp")
add_custom_command(
   OUTPUT ${FLEX_OUT}
   COMMAND flex -P yas -o ${FLEX_OUT} ${FLEX_SRC}
   COMMENT "Flex tool generates lexer source code"
   VERBATIM
)

file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_LIST_DIR} "*.cpp")
list(FILTER SRC_CPP EXCLUDE REGEX ".*test_yas\\.cpp$")
list(REMOVE_DUPLICATES SRC_CPP)

# message(STATUS ${SRC_CPP} ${FLEX_OUT})

set(TARGET web_api)
# Pass in all target dependencies
add_executable(${TARGET} ${FLEX_OUT} ${SRC_CPP})

# The option should not have any white space, use semicolon to split different options
separate_arguments(COMPILE_OPTION UNIX_COMMAND "-Wall -Wno-misleading-indentation -I${CMAKE_CURRENT_SOURCE_DIR}/src")
separate_arguments(LINK_OPTION UNIX_COMMAND "--no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']'")

set_target_properties(${TARGET} PROPERTIES
   COMPILE_OPTIONS "${COMPILE_OPTION}"
   LINK_OPTIONS "${LINK_OPTION}"
   RUNTIME_OUTPUT_DIRECTORY ${OUTUPT_DIR}/wasm
)
