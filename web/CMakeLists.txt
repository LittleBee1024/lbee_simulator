set(TARGET web_api)
file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_LIST_DIR} "*.cpp")

add_executable(${TARGET} ${SRC_CPP})
set(CMAKE_EXECUTABLE_SUFFIX ".js")

# libutil.a is located in "./cpp/lib"
# libdemo.a is built with cpp files in "./cpp/src"
target_link_libraries(${TARGET}
  PRIVATE
    util
    demo
)

# Set target options
# "./cpp/lib" has cpp libraries
target_link_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/cpp/lib)
target_include_directories(${TARGET} PRIVATE (${CMAKE_SOURCE_DIR}/cpp/lib))

# add "--no-entry" because web_api.cpp doesn't have main function
target_compile_options(${TARGET} PRIVATE -g)
target_link_options(${TARGET} PRIVATE --no-entry -g -s "EXPORTED_FUNCTIONS=['_malloc', '_free']")
# CMAKE_CXX_FLAGS will affect both compile and link commands
# set(CMAKE_CXX_FLAGS "-O0")

set(WEB_OUTUPT_DIR ${CMAKE_BINARY_DIR}/html)
file(MAKE_DIRECTORY ${WEB_OUTUPT_DIR})
set_target_properties(${TARGET} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${WEB_OUTUPT_DIR}
)

# Copy js and html files to output folder
add_custom_command(
  TARGET ${TARGET} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${WEB_OUTUPT_DIR}
)


