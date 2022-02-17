# Making a library
file(GLOB_RECURSE SRC_CPP ${CMAKE_CURRENT_LIST_DIR} "*.cpp")
add_library(demo STATIC ${SRC_CPP})

target_include_directories(demo
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
