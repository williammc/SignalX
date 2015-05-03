# Tests ========================================================================
enable_testing()  # has to be in root CMakeLists.txt for target 'test' to work!
file(GLOB test_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
set(ALL_LIBRARIES "")
foreach(filename ${test_sources})
  get_filename_component(test_name ${filename} NAME_WE)
  cmaker_add_test(${test_name})
  set_property(TARGET ${test_name} PROPERTY FOLDER "signalx_tests")
endforeach()