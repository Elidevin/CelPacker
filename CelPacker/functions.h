#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>

void
saveCelFile(const std::vector<uint8_t>& celdata, const std::string& filename);

std::vector<uint8_t>
getFileData(const std::string& filename);

std::list<std::vector<uint8_t>>
celFileToFrames(const std::vector<uint8_t>& filedata);

std::vector<uint8_t>
framesToCelFile(const std::list<std::vector<uint8_t>>& frames);

std::list<std::vector<uint8_t>>
openFiles(const std::string& fileName, const std::string& fileType, size_t startFromFrameIndex = 0);

std::list<std::vector<uint8_t>>
celsToFrames(const std::list<std::vector<uint8_t>>& cels);

std::list<std::vector<uint8_t>>
framesToCels(const std::list<std::vector<uint8_t>>& frames);
