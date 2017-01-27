#pragma once

#include <stdlib.h>
#include <fstream>

struct StillBlock
{
public:
	StillBlock();
	~StillBlock();

	std::string TITLE;
	std::string NAME;
	std::string ARTIST;
	std::string AUTHOR;
	std::string COMMENT;
};

