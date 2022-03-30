//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"

class SearchEngine {
public:
	class Result {
	public:
		int fileId;
		int line;
		int column;
		std::string text;
	};

	class File {
	public:
		int id;
		std::string name;
		std::string content;
		std::vector<std::string> lines;
		std::vector<Result> results;
	};

	std::vector<File> files;

	void search(const std::string& fileNameInput, const std::string& searchPattern);
};
