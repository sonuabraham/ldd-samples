cmake_minimum_required(VERSION 2.8.10)

project(char_module_tests)

add_executable(lab1_char_init_test char/lab1_char_init_test.c)
add_executable(lab2_char_read_write_test char/lab2_char_read_write_test.c)
add_executable(lab3_char_ioctl_test char/lab3_char_ioctl_test.c)
add_executable(lab4_char_seek_test char/lab4_char_seek_test.c)
add_executable(lab5_char_poll_test char/lab5_char_poll_test.c)
add_executable(lab5_char_select_test char/lab5_char_select_test.c)
add_executable(lab6_char_fasync_test char/lab6_char_fasync_test.c)

install(TARGETS lab1_char_init_test RUNTIME DESTINATION bin)
install(TARGETS lab2_char_read_write_test RUNTIME DESTINATION bin)
install(TARGETS lab3_char_ioctl_test RUNTIME DESTINATION bin)
install(TARGETS lab4_char_seek_test RUNTIME DESTINATION bin)
install(TARGETS lab5_char_poll_test RUNTIME DESTINATION bin)
install(TARGETS lab5_char_select_test RUNTIME DESTINATION bin)
install(TARGETS lab6_char_fasync_test RUNTIME DESTINATION bin)


