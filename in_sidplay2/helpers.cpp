#include "StdAfx.h"
#include "helpers.h"

std::vector<std::string> split(const std::string &text, char sep) {
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	return tokens;
}

void replaceAll(std::string& stringToReplace, const char* stringToFind, const char* replacement)
{
	size_t index = 0;
	int toFindLen = strlen(stringToFind);
	int replacementLen = strlen(replacement);
	while (true) {
		/* Locate the substring to replace. */
		index = stringToReplace.find(stringToFind, index);
		if (index == std::string::npos) break;

		/* Make the replacement. */
		stringToReplace.replace(index, toFindLen, replacement);

		/* Advance index forward so the next iteration doesn't pick it up as well. */
		index += replacementLen;
	}
}
