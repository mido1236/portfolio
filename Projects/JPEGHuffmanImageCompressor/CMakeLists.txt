cmake_minimum_required(VERSION 3.30)
project(JPEGHuffmanImageCompressor)

set(CMAKE_CXX_STANDARD 20)

add_executable(JPEGHuffmanImageCompressor main.cpp
        pixel.h
        huffmannode.h)
