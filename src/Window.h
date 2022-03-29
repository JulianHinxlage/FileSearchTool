//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"

class Window {
public:
	bool init(int width, int height, const std::string &title, int swapInterval = 1);
	void updateBegin();
	void updateEnd();
	void shutdown();

	void close();
	bool isOpen();
private:
	void *window;
};