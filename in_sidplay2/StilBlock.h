#pragma once

#include <stdlib.h>
#include <fstream>

struct StilBlock
{
public:
	StilBlock();
	~StilBlock();

	std::string TITLE;
	std::string NAME;
	std::string ARTIST;
	std::string AUTHOR;
	std::string COMMENT;
};

