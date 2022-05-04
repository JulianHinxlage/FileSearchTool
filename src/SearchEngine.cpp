//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "SearchEngine.h"
#include "strutil.h"
#include <regex>

void SearchEngine::search(const std::vector<std::string>& fileNames) {
	//load files
	files.clear();
	for (int fileId = 0; fileId < fileNames.size(); fileId++) {
		auto& fileName = fileNames[fileId];
		File file;
		file.id = fileId;
		file.name = fileName;

		std::string data = readFile(fileName);
		file.content = data;

		if (data.size() > 0) {
			auto lines = split(data, "\n", true);
			file.lines = lines;
		}

		Result result;
		result.column = 1;
		result.line = 1;
		result.position = 0;
		result.fileId = fileId;
		result.text = data;
		file.results.push_back(result);

		files.push_back(file);
	}

	//execute steps
	for (auto& step : steps) {
		if (step.active) {
			switch (step.type)
			{
			case SearchStep::SEARCH:
				normalSerach(step);
				break;
			case SearchStep::FILTER:
				filterSerach(step);
				break;
			case SearchStep::VICINITY:
				vicinitySerach(step);
				break;
			case SearchStep::INBETWEEN:
				inbetweenSerach(step);
				break;
			default:
				break;
			}
		}
	}
}

void SearchEngine::normalSerach(SearchStep& step) {
	for (auto& file : files) {
		auto results = file.results;
		file.results.clear();
		auto lines = file.lines;

		for (auto& r : results) {
			std::string data = r.text;

			std::smatch match;
			std::regex regex;
			try {
				regex = std::regex(step.pattern);
			}
			catch (...) {
				//invalid regex
			}
			int column = r.column - 1;
			int line = r.line - 1;
			int position = r.position;

			while (std::regex_search(data, match, regex)) {
				int offset = match.prefix().length();
				column += offset;
				position += offset;
				while (line < lines.size() && column > lines[line].length()) {
					column -= lines[line].length() + 1;
					line++;
				}

				Result result;
				result.column = column + 1;
				result.line = line + 1;
				result.position = position;
				result.fileId = file.id;
				result.text = match[0].str();
				file.results.push_back(result);
				data = match.suffix();

				column += result.text.length();
				position += result.text.length();

				if (offset + result.text.length() == 0) {
					if (data.size() > 0) {
						data = data.substr(1);
					}
					else {
						break;
					}
				}
			}
		}
	}
}

void SearchEngine::filterSerach(SearchStep& step) {
	for (auto& file : files) {

		auto results = file.results;
		file.results.clear();

		for (auto& r : results) {
			std::string data = r.text;

			std::smatch match;
			std::regex regex;
			try {
				regex = std::regex(step.pattern);
			}
			catch (...) {
				//invalid regex
			}

			if (std::regex_search(data, match, regex) != step.negate) {
				file.results.push_back(r);
			}
		}
	}
}

void SearchEngine::vicinitySerach(SearchStep& step) {

}

void SearchEngine::inbetweenSerach(SearchStep& step) {
	for (auto& file : files) {
		auto results = file.results;
		file.results.clear();
		auto lines = file.lines;

		std::vector<Result> results1;
		std::vector<Result> results2;


		for (auto& r : results) {
			std::string data = r.text;

			std::smatch match;
			std::regex regex;
			try {
				regex = std::regex(step.pattern);
			}
			catch (...) {
				//invalid regex
			}
			int column = r.column - 1;
			int line = r.line - 1;
			int position = r.position;

			while (std::regex_search(data, match, regex)) {
				int offset = match.prefix().length();
				column += offset;
				position += offset;
				while (line < lines.size() && column > lines[line].length()) {
					column -= lines[line].length() + 1;
					line++;
				}

				Result result;
				result.column = column + 1;
				result.line = line + 1;
				result.position = position;
				result.fileId = file.id;
				result.text = match[0].str();
				results1.push_back(result);
				data = match.suffix();

				column += result.text.length();
				position += result.text.length();

				if (offset + result.text.length() == 0) {
					if (data.size() > 0) {
						data = data.substr(1);
					}
					else {
						break;
					}
				}
			}
		}

		for (auto& r : results) {
			std::string data = r.text;

			std::smatch match;
			std::regex regex;
			try {
				regex = std::regex(step.pattern2);
			}
			catch (...) {
				//invalid regex
			}
			int column = r.column - 1;
			int line = r.line - 1;
			int position = r.position;

			while (std::regex_search(data, match, regex)) {
				int offset = match.prefix().length();
				column += offset;
				position += offset;
				while (line < lines.size() && column > lines[line].length()) {
					column -= lines[line].length() + 1;
					line++;
				}

				Result result;
				result.column = column + 1;
				result.line = line + 1;
				result.position = position;
				result.fileId = file.id;
				result.text = match[0].str();
				results2.push_back(result);
				data = match.suffix();

				column += result.text.length();
				position += result.text.length();

				if (offset + result.text.length() == 0) {
					if (data.size() > 0) {
						data = data.substr(1);
					}
					else {
						break;
					}
				}
			}
		}


		for (int i1 = 0; i1 < results1.size(); i1++) {
			int i2 = 0;
			for (i2 = 0; i2 < results2.size(); i2++) {
				auto& r1 = results1[i1];
				auto& r2 = results2[i2];
				bool r1Greater = false;
				if (r1.line > r2.line) {
					r1Greater = true;
				}
				else if (r1.line == r2.line) {
					r1Greater = r1.column > r2.column;
				}

				if (!r1Greater) {
					break;
				}
			}

			if (i2 < results2.size() && i1 + 1 < results1.size()) {
				auto& r1 = results1[i1+1];
				auto& r2 = results2[i2];
				bool r1Greater = false;
				if (r1.line > r2.line) {
					r1Greater = true;
				}
				else if (r1.line == r2.line) {
					r1Greater = r1.column > r2.column;
				}

				if (r1Greater) {
					auto& r1 = results1[i1];

					Result result;
					result.line = r1.line;
					result.column = r1.column + r1.text.length();//todo: determin line number
					result.fileId = file.id;
					int p1 = r1.position + r1.text.length();
					int p2 = r2.position;
					if (p2 >= p1) {
						result.text = file.content.substr(p1, p2 - p1);
					}
					file.results.push_back(result);
				}
			}

		}

	}
}
