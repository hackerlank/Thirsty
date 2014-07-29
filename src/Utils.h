/**
 *  @file   Utils.h
 *  @author ichenq@gmail.com
 *  @date   Jul 14, 2014
 *  @brief  Utility function or macros
 *
 */

#pragma once

#include <stdint.h>
#include <string>

// get current tick count(in nanoseconds), for time measurements
uint64_t getNowTickCount();

// binary data to hex representation
std::string bin2hex(const void* input, size_t length);

// given a file's name, return it's size
int32_t GetFileLength(const char* filename);

