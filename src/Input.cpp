//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "Input.h"
#include "Context.h"
#include "strutil.h"
#include <imgui.h>

#if WIN32
#include <Windows.h>
#endif

std::string openFileDialog(const char* pattern) {
#if WIN32
	OPENFILENAME ofn = { 0 };
	TCHAR szFile[260] = { 0 };
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = (HWND)context.window.getContext();
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = pattern;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileName(&ofn) == TRUE) {
		std::string file = ofn.lpstrFile;
		return file;
	}
	else {
		return "";
	}
#else
	return "";
#endif
}

void Input::update() {
	if (context.openInput) {
		if (ImGui::Begin("Input", &context.openInput, ImGuiWindowFlags_NoCollapse)) {
			context.inputText(directory, "directory", "");
			
			ImGui::SameLine();
			if (ImGui::Button("...")) {
				std::string file = openFileDialog(".*\0*.*\0");
				if (!file.empty()) {

					auto path = std::filesystem::path(file);
					if (std::filesystem::is_directory(path)) {
						directory = file;
					}
					else {
						directory = path.parent_path().string();
					}

				}
			}
			
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

std::vector<std::string> Input::getFiles() {
	std::vector<std::string> fileList;
	auto patterns = split(files, ";");
    std::string dir = directory;
    if (directory.size() > 0 && directory.back() != '\\' && directory.back() != '/') {
        dir += "\\";
    }

    try {
	    for (auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
		    if (!std::filesystem::is_directory(entry)) {
			    std::string file = entry.path().string().substr(dir.size());
			    std::cout << file << std::endl;
			    for (auto& pattern : patterns) {
                    if (wildcardMatch(replace(file, "/", "\\"), replace(pattern, "/", "\\"))) {
					    fileList.push_back(entry.path().string());
                        break;
				    }
			    }
		    }
	    }
    }
    catch (...) {}

	return fileList;
}
