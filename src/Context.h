//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include "SearchEngine.h"

class Context {
public:
	std::string file;
	std::string search;
	int selectedFile = -1;
	int selectedResult = -1;
	bool isFirstFrame = true;

	SearchEngine engine;
};

extern Context context;