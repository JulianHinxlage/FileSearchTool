//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "SearchEngine.h"
#include "strutil.h"
#include <regex>

void SearchEngine::search(const std::string &fileNameInput, const std::string& searchPattern) {
	files.clear();

	auto fileNames = split(fileNameInput, ";");
	for (int fileId = 0; fileId < fileNames.size(); fileId++) {
		auto& fileName = fileNames[fileId];
		File file;
		file.id = fileId;
		file.name = fileName;

		std::string data = readFile(fileName);
		if (data.size() > 0) {
			auto lines = split(data, "\n", true);
			file.lines = lines;

			std::smatch match;
			std::regex regex;
			try {
				regex = std::regex(searchPattern);
			}catch(...) {
				//invalid regex
			}
			int column = 0;
			int line = 0;

			while (std::regex_search(data, match, regex)) {
				int offset = match.prefix().length();
				column += offset;
				while (lines.size() > 0 && column > lines[0].length()) {
					line++;
					column -= lines[0].length() + 1;
					lines.erase(lines.begin());
				}


				Result result;
				result.column = column + 1;
				result.line = line + 1;
				result.fileId = fileId;
				result.text = match[0].str();
				file.results.push_back(result);
				data = match.suffix();

				column += result.text.length();
			}
		}
		files.push_back(file);
	}
}
