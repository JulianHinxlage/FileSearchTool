//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"

class InputWindow {
public:
	std::string directory;
	std::string files;

	void update();

	std::vector<std::string> getFiles();
};
