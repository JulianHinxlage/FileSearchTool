//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "strutil.h"
#include "Window.h"
#include "SearchEngine.h"
#include "Context.h"
#include <imgui.h>

void inputText(std::string& text, const std::string& label, const std::string& hint = "") {
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

		int count = 0;
		for (auto& f : context.engine.files) {
			count += f.results.size();
		}
		ImGui::Text("Total matches: %i", count);


		for (int fileId = 0; fileId < context.engine.files.size(); fileId++) {
			auto& f = context.engine.files[fileId];
			if (f.results.size() > 0) {

				std::string lable = std::string("(") + std::to_string(f.results.size()) + ") " + f.name;

				if (ImGui::TreeNodeEx(lable.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

					if (ImGui::BeginTable("", 3)) {

						ImGui::TableSetupColumn("text");
						ImGui::TableSetupColumn("line", ImGuiTableColumnFlags_WidthFixed, 45);
						ImGui::TableSetupColumn("column", ImGuiTableColumnFlags_WidthFixed, 45);

						//ImGui::TableNextColumn();
						//ImGui::Text("text");
						//ImGui::TableNextColumn();
						//ImGui::Text("line");
						//ImGui::TableNextColumn();
						//ImGui::Text("column");

						for (int resultId = 0; resultId < f.results.size(); resultId++) {
							ImGui::PushID(resultId);
							auto& r = f.results[resultId];
							ImGui::TableNextColumn();

							bool selected = false;
							if (ImGui::Selectable(r.text.c_str(), &selected)) {}
							if (ImGui::IsItemActivated()) {
								context.selectedFile = fileId;
								context.selectedResult = resultId;

								int c = r.column - 1;
								auto &l = f.lines[r.line - 1];
								for (int i = 0; i < std::min(r.column, (int)l.size()); i++) {
									if (l[i] == '\t') {
										c += context.editor.GetTabSize() - 1;
									}
								}

								if (f.id != context.currentFileId) {
									context.editor.SetText(f.content);
									context.currentFileId = f.id;
								}
								context.editor.SetReadOnly(true);
								context.editor.SetShowWhitespaces(false);
								context.editor.SetSelection(TextEditor::Coordinates(r.line - 1, c), TextEditor::Coordinates(r.line - 1, c + r.text.size()));
								context.editor.SetCursorPosition(TextEditor::Coordinates(r.line - 1, c));


								TextEditor::Breakpoints points;
								for (int resultId = 0; resultId < f.results.size(); resultId++) {
									auto& r = f.results[resultId];
									points.insert(r.line);
								}
								context.editor.SetBreakpoints(points);
								
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
	if (ImGui::Begin("File")) {
		context.editor.Render("##");
	}
	ImGui::End();
}

void windowOutput() {
	if (ImGui::Begin("Output", nullptr)) {
		inputText(context.outputFile, "file");
		ImGui::Checkbox("full lines", &context.outputFullLines);

		bool generate = false;

		if (ImGui::Button("Generate")) {
			if (std::filesystem::exists(context.outputFile)) {
				ImGui::OpenPopup("exists");
			}
			else {
				generate = true;
			}
		}

		if (ImGui::BeginPopup("exists")) {

			ImGui::Text("The file already exists.\nDo you want to override it?");
			if (ImGui::Button("Override")) {
				generate = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (generate) {
			std::ofstream stream(context.outputFile);

			for (auto& f : context.engine.files) {
				for (auto& r : f.results) {
					if (context.outputFullLines) {
						stream << f.lines[r.line - 1] << "\n";
					}
					else {
						stream << r.text << "\n";
					}
				}
			}
			stream.close();

			context.selectedFile = -1;
			context.selectedResult = -1;
			context.currentFileId = -1;

			context.editor.SetText(readFile(context.outputFile));
			context.editor.SetReadOnly(true);
			context.editor.SetShowWhitespaces(false);

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
	if (config.size() > 2) {
		context.outputFile = config[2];
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
		windowOutput();

		window.updateEnd();
	}

	writeFile("project.ini", join({ context.file, context.search, context.outputFile }, "\n"));
	window.shutdown();
	return 0;
}

#if _WIN32
int __stdcall WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow) {
	return main(nCmdShow, (char**)lpCmdLine);
}
#endif
