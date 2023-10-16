cmake_minimum_required(VERSION 3.26)
project(RCOM C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(RCOM
        aplicacao.c
        aplicacao.h
        flags.h
        main.c
        protocolo.c
        protocolo.h)
