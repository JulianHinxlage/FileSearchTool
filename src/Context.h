//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"
#include "SearchEngine.h"
#include "TextEditor/TextEditor.h"
#include "Window.h"
#include "Input.h"

class Context {
public:
	Input input;
	Window window;

	int selectedFile = -1;
	int selectedResult = -1;
	bool isFirstFrame = true;

	SearchEngine engine;
	TextEditor editor;
	int currentFileId = -1;

	bool outputFullLines = false;
	std::string outputFile;

	//open flags
	bool openInput;
	bool openSearch;
	bool openResults;
	bool openFile;
	bool openOutput;

	void init();
	void inputText(std::string& text, const std::string& label, const std::string& hint = "");
};

extern Context context;