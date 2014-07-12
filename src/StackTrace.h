/**
 *  @file   StackTrace.h
 *  @author ichenq@gmail.com
 *  @date   Jul 12, 2014
 *  @brief  stack trace
 *
 */

#pragma once

#include <string>

std::string     getStackTrace(int maxDepth = 0);
