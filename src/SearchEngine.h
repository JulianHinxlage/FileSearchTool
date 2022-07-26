//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"

class SearchStep {
public:
	std::string pattern;
	std::string pattern2;
	enum Type {
		SEARCH,
		FILTER,
		INBETWEEN,
		VICINITY,
	};
	Type type;
	bool active = true;
	bool negate = false;

	static const char* typeToString(Type type) {
		switch (type)
		{
		case SearchStep::SEARCH:
			return "SEARCH";
		case SearchStep::FILTER:
			return "FILTER";
		case SearchStep::VICINITY:
			return "VICINITY";
		case SearchStep::INBETWEEN:
			return "INBETWEEN";
		default:
			return "NONE";
		}
	}

	bool stringSearch;
	bool matchCase;
	bool matchFullLine;
};

class SearchEngine {
public:
	class Result {
	public:
		int fileId;
		int line;
		int column;
		int position;
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
	std::vector<SearchStep> steps;

	void search(const std::vector<std::string>& fileNames);

private:
	void normalSerach(SearchStep& step);
	void filterSerach(SearchStep& step);
	void vicinitySerach(SearchStep& step);
	void inbetweenSerach(SearchStep& step);
};
