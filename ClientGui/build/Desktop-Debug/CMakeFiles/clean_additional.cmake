# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/gui_client_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/gui_client_autogen.dir/ParseCache.txt"
  "gui_client_autogen"
  )
endif()
