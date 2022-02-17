set(TARGET web_api)
file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_LIST_DIR} "*.cpp")

add_executable(${TARGET} ${SRC_CPP})

set_target_properties(${TARGET} PROPERTIES
   COMPILE_OPTIONS "-Wall"
   LINK_OPTIONS "--no-entry -s 'EXPORTED_FUNCTIONS=['_malloc', '_free']'"
   RUNTIME_OUTPUT_DIRECTORY ${OUTUPT_DIR}/wasm
)
