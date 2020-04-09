cmake_minimum_required(VERSION 2.8.10)

project(char_module_tests)

add_executable(lab2_char_stage2_test lab2_char_stage2_test.c)
add_executable(lab1_char_stage1_test lab1_char_stage1_test.c)

install(TARGETS lab2_char_stage2_test RUNTIME DESTINATION bin)
install(TARGETS lab1_char_stage1_test RUNTIME DESTINATION bin)
