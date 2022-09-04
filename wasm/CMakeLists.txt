# Command to generate lexer using flex tool
set(FLEX_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")
file(MAKE_DIRECTORY ${FLEX_BINARY_DIR})
set(FLEX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/assembler/yas.lex")
set(FLEX_OUT "${FLEX_BINARY_DIR}/yas_gen.cpp")
add_custom_command(
   OUTPUT ${FLEX_OUT}
   COMMAND flex -P yas_ -o ${FLEX_OUT} ${FLEX_SRC}
   COMMENT "Flex tool generates lexer source code"
   VERBATIM
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/_common COMMON_SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/assembler ASSEMBLER_SRCS)
file(GLOB SRCS LIST_DIRECTORIES false ${CMAKE_CURRENT_LIST_DIR} "*.cpp")

# message(STATUS "FLEX_OUT = ${FLEX_OUT}")
# message(STATUS "COMMON_SRCS = ${COMMON_SRCS}")
# message(STATUS "ASSEMBLER_SRCS = ${ASSEMBLER_SRCS}")
# message(STATUS "SRCS = ${SRCS}")

set(TARGET web_api)
# Pass in all target dependencies
add_executable(${TARGET}
   ${SRCS}
   ${FLEX_OUT}
   ${COMMON_SRCS}
   ${ASSEMBLER_SRCS}
)

# The option should not have any white space, use semicolon to split different options
separate_arguments(COMPILE_OPTION UNIX_COMMAND "-Wall -Wno-misleading-indentation -I${CMAKE_CURRENT_SOURCE_DIR}")
separate_arguments(LINK_OPTION UNIX_COMMAND "--no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']'")

set_target_properties(${TARGET} PROPERTIES
   COMPILE_OPTIONS "${COMPILE_OPTION}"
   LINK_OPTIONS "${LINK_OPTION}"
   RUNTIME_OUTPUT_DIRECTORY ${OUTUPT_DIR}/wasm
)
