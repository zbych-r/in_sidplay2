#pragma once

#include <vector>
#include <string>

std::vector<std::string> split(const std::string &text, char sep);

void replaceAll(std::string& stringToReplace, const char* stringToFind, const char* replacement);
