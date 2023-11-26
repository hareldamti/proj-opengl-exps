#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>

#define ERROR_EXIT(...) { fprintf(stderr, __VA_ARGS__); exit(1); }
#define ERROR_RETURN(R, ...) { fprintf(stderr, __VA_ARGS__); return R; }
#define LOG_DEBUG(...) { fprintf(stderr,"DEBUG:\t"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define LOG_ERROR(...) { fprintf(stderr,"ERROR:\t"); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); }
#define ASSERT(x) if(!(x)) ERROR_EXIT("ASSERSION: %s\n",#x)

std::string read_file(const std::string& file_path);