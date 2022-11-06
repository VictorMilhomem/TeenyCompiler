cmake_minimum_required(VERSION 3.21)
project(TinyCompiler)

set(CMAKE_CXX_STANDARD 14)

add_executable(TinyCompiler main.cpp src/lexer.h src/error.h src/compiler.h src/compiler.cpp src/parser.h src/emmiter.h)
