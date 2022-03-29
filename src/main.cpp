//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "strutil.h"
#include "Window.h"
#include "SearchEngine.h"
#include "Context.h"
#include <imgui.h>
#include <regex>

void inputText(std::string& text, const std::string& label, const std::string& hint) {
	static std::string inputBuffer;
	if (inputBuffer.size() == 0) {
		inputBuffer.resize(1024 * 16);
	}
	strcpy(inputBuffer.data(), text.c_str());
	ImGui::InputTextWithHint(label.c_str(), hint.c_str(), inputBuffer.data(), inputBuffer.capacity() - 1);
	text = inputBuffer.c_str();
}

void windowSearch() {
	if (ImGui::Begin("Search")) {

		inputText(context.file, "file", "");
		inputText(context.search, "string", "");

		if (ImGui::Button("Search") || context.isFirstFrame) {
			context.isFirstFrame = false;
			context.engine.search(context.file, context.search);
		}
	}
	ImGui::End();
}

void windowResults() {
	if (ImGui::Begin("Results")) {
		for (int fileId = 0; fileId < context.engine.files.size(); fileId++) {
			auto& f = context.engine.files[fileId];
			if (f.results.size() > 0) {

				if (ImGui::TreeNodeEx(f.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

					if (ImGui::BeginTable("", 3)) {

						ImGui::TableSetupColumn("text");
						ImGui::TableSetupColumn("line", ImGuiTableColumnFlags_WidthFixed, 45);
						ImGui::TableSetupColumn("column", ImGuiTableColumnFlags_WidthFixed, 45);

						ImGui::TableNextColumn();
						ImGui::Text("text");
						ImGui::TableNextColumn();
						ImGui::Text("line");
						ImGui::TableNextColumn();
						ImGui::Text("column");

						for (int resultId = 0; resultId < f.results.size(); resultId++) {
							ImGui::PushID(resultId);
							auto& r = f.results[resultId];
							ImGui::TableNextColumn();

							bool selected = false;
							if (ImGui::Selectable(r.text.c_str(), &selected)) {}
							if (ImGui::IsItemActivated()) {
								context.selectedFile = fileId;
								context.selectedResult = resultId;
							}

							ImGui::TableNextColumn();
							ImGui::Text("%i", r.line);
							ImGui::TableNextColumn();
							ImGui::Text("%i", r.column);
							ImGui::TableNextRow();
							ImGui::PopID();
						}


						ImGui::EndTable();
					}

					ImGui::TreePop();
				}
			}

		}
	}
	ImGui::End();
}

void windowFile() {
	if (ImGui::Begin("File", nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar)) {
		if (ImGui::BeginTable("", 2)) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 30);

			if (context.selectedFile != -1) {
				if (context.engine.files.size() > context.selectedFile) {
					auto& f = context.engine.files[context.selectedFile];
					if (f.results.size() > context.selectedResult) {
						auto& r = f.results[context.selectedResult];
						for (int line = 0; line < f.lines.size(); line++) {
							if (line + 1 == r.line) {
								ImGui::TableNextColumn();
								ImGui::Text("%i", line + 1);
								ImGui::TableNextColumn();
								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
								ImGui::Text("%s", f.lines[line].c_str());
								ImGui::PopStyleColor(1);
								ImGui::TableNextRow();
							}
							else {
								ImGui::TableNextColumn();
								ImGui::Text("%i", line + 1);
								ImGui::TableNextColumn();
								ImGui::Text("%s", f.lines[line].c_str());
								ImGui::TableNextRow();
							}
						}
					}
					else {
						for (int line = 0; line < f.lines.size(); line++) {
							ImGui::TableNextColumn();
							ImGui::Text("%i", line + 1);
							ImGui::TableNextColumn();
							ImGui::Text("%s", f.lines[line].c_str());
							ImGui::TableNextRow();
						}
					}
				}
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

int main(int argc, char* args[]) {
	Window window;
	window.init(1080, 720, "FileSearchTool", 2);
	
	auto config = split(readFile("project.ini"), "\n");
	if (config.size() > 0) {
		context.file = config[0];
	}
	if (config.size() > 1) {
		context.search = config[1];
	}

	if (!std::filesystem::exists("layout.ini")) {
		if (std::filesystem::exists("../../../layout.ini")) {
			std::filesystem::copy_file("../../../layout.ini", "layout.ini");
		}
	}
	ImGui::GetIO().IniFilename = "layout.ini";

	while (window.isOpen()) {
		window.updateBegin();
		ImGui::DockSpaceOverViewport();

		windowSearch();
		windowResults();
		windowFile();

		window.updateEnd();
	}

	writeFile("project.ini", join({ context.file, context.search }, "\n"));
	window.shutdown();
	return 0;
}

#if _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
	return main(nCmdShow, (char**)lpCmdLine);
}
#endif
