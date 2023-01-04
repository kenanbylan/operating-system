cmake_minimum_required(VERSION 3.24)
project(OSProject C)

set(CMAKE_C_STANDARD 99)


add_executable(OSProject main.c findtopk.c findtopk_thread.c findtopk.c)
