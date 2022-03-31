//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#include "pch.h"
#include "Context.h"
#include "strutil.h"
#include <imgui/imgui_internal.h>

Context context;

void Context::init() {
    ImGuiSettingsHandler handler;
    handler.TypeName = "OpenFlags";
    handler.TypeHash = ImHashStr("OpenFlags");
    handler.ReadOpenFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) -> void* {
        if (std::string(name) == "") {
            return (void*)1;
        }
        else {
            return nullptr;
        }
    };
    handler.ReadLineFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {
        auto parts = split(line, "=");
        if (parts.size() >= 2) {
            try {
                if (parts[0] == "input") {
                    context.openInput = std::stoi(parts[1]);
                }else if (parts[0] == "search") {
                    context.openSearch = std::stoi(parts[1]);
                }
                else if (parts[0] == "results") {
                    context.openResults = std::stoi(parts[1]);
                }
                else if (parts[0] == "file") {
                    context.openFile = std::stoi(parts[1]);
                }
                else if (parts[0] == "output") {
                    context.openOutput = std::stoi(parts[1]);
                }
            }
            catch (...) {}
        }
    };
    handler.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
        buf->append("[OpenFlags][]\n");
        buf->appendf("%s=%i\n", "input", (int)context.openInput);
        buf->appendf("%s=%i\n", "search", (int)context.openSearch);
        buf->appendf("%s=%i\n", "results", (int)context.openResults);
        buf->appendf("%s=%i\n", "file", (int)context.openFile);
        buf->appendf("%s=%i\n", "output", (int)context.openOutput);
        buf->appendf("\n");
    };
    ImGui::GetCurrentContext()->SettingsHandlers.push_back(handler);
}

void Context::inputText(std::string& text, const std::string& label, const std::string& hint) {
    static std::string inputBuffer;
    if (inputBuffer.size() == 0) {
        inputBuffer.resize(1024 * 16);
    }
    strcpy(inputBuffer.data(), text.c_str());
    ImGui::InputTextWithHint(label.c_str(), hint.c_str(), inputBuffer.data(), inputBuffer.capacity() - 1);
    text = inputBuffer.c_str();
}
