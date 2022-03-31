//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "InputWindow.h"
#include "Context.h"
#include "strutil.h"
#include <imgui.h>

void InputWindow::update() {
	if (context.openInput) {
		if (ImGui::Begin("Input", &context.openInput, ImGuiWindowFlags_NoCollapse)) {
			context.inputText(directory, "directory", "");
			context.inputText(files, "files", "");
		}
		ImGui::End();
	}
}

bool wildcardMatch(const std::string& file, const std::string& pattern) {
    std::string::const_iterator str_it = file.begin();
    for (std::string::const_iterator pat_it = pattern.begin(); pat_it != pattern.end();
        ++pat_it) {
        switch (*pat_it) {
        case '?':
            if (str_it == file.end()) {
                return false;
            }

            ++str_it;
            break;
        case '*': {
            if (pat_it + 1 == pattern.end()) {
                return true;
            }

            const size_t max = strlen(&*str_it);
            for (size_t i = 0; i < max; ++i) {
                if (wildcardMatch(&*(pat_it + 1), &*(str_it + i))) {
                    return true;
                }
            }

            return false;
        }
        default:
            if (*str_it != *pat_it) {
                return false;
            }

            ++str_it;
        }
    }

    return str_it == file.end();
}

std::vector<std::string> InputWindow::getFiles() {
	std::vector<std::string> fileList;
	auto patterns = split(files, ";");
    std::string dir = directory;
    if (directory.size() > 0 && directory.back() != '\\' && directory.back() != '/') {
        dir += "\\";
    }
	for (auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
		if (!std::filesystem::is_directory(entry)) {
			std::string file = entry.path().string().substr(dir.size());
			std::cout << file << std::endl;
			for (auto& pattern : patterns) {
                if (wildcardMatch(file, pattern)) {
					fileList.push_back(entry.path().string());
                    break;
				}
			}
		}
	}
	return fileList;
}
