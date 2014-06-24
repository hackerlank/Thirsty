#pragma once

#include <string>

/**
* stringPrintf is much like printf but deposits its result into a
* string. Two signatures are supported: the first simply returns the
* resulting string, and the second appends the produced characters to
* the specified string and returns a reference to it.
*/
std::string stringPrintf(const char* format, ...);

/** Similar to stringPrintf, with different signiture.
*/
void stringPrintf(std::string* out, const char* fmt, ...);

std::string& stringAppendf(std::string* output, const char* format, ...);
