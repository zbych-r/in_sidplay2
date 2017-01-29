#pragma once

#include <vector>

std::vector<std::string> split(const std::string &text, char sep);

void replaceAll(std::string& stringToReplace, const char* stringToFind, const char* replacement);
