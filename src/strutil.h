//
// Copyright (c) 2022 Julian Hinxlage. All rights reserved.
//

#pragma once

#include "pch.h"

std::string readFile(const std::string& file);

void writeFile(const std::string& file, const std::string& text);

std::vector<std::string> split(const std::string& string, const std::string& delimiter, bool includeEmpty = false);

std::vector<std::string> split(const std::vector<std::string>& strings, const std::string& delimiter, bool includeEmpty = false);

std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

std::string replace(const std::string& string, const std::string& search, const std::string& replacement);
