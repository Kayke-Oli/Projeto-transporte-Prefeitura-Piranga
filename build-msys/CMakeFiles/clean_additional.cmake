# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\core_utils_tests_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\core_utils_tests_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\report_exporter_tests_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\report_exporter_tests_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\sistema_prefeitura_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\sistema_prefeitura_autogen.dir\ParseCache.txt]]
  "core_utils_tests_autogen"
  "report_exporter_tests_autogen"
  "sistema_prefeitura_autogen"
  )
endif()
