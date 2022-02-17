# Copy js and html files to output folder
add_custom_target(
   copy_ui ALL
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR} ${OUTUPT_DIR}
)
