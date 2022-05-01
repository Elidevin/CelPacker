#include "functions.h"

void saveCelFile(const std::vector<uint8_t>& celdata, const std::string& filename) {
	std::fstream file(filename, std::ios::binary | std::ios::out);
	file.write((const char*)&celdata[0], celdata.size());
}

std::vector<uint8_t> getFileData(const std::string& filename) {
	std::vector<uint8_t> filedata;
	std::fstream file;
	file.open(filename, std::ios::binary | std::ios::ate | std::ios::in);
	if (!file.is_open()) return filedata;

	filedata.reserve(static_cast<size_t>(file.tellg()));
	file.seekg(std::ios::beg);
	file.unsetf(std::ios::skipws);
	filedata.insert(filedata.begin(), std::istream_iterator<uint8_t>(file), std::istream_iterator<uint8_t>());

	file.seekg(std::ios::beg);
	file.clear();
	file.close();

	return filedata;
}

std::list<std::vector<uint8_t>> openFiles(const std::string& fileName, const std::string& fileType, size_t startFromFrameIndex) {
	std::list<std::vector<uint8_t>> files;
	for (size_t i = startFromFrameIndex; i < 0xFFFFFFFF; ++i) {
		std::string fullfilename(fileName + std::to_string(i) + fileType);
		std::vector<uint8_t>
			tmp = getFileData(fullfilename);
		if (tmp.size()) files.emplace_back(tmp);
		else break;
	}
	return files;
}

std::list<std::vector<uint8_t>> celsToFrames(const std::list<std::vector<uint8_t>>& cels) {
	std::list<std::vector<uint8_t>> result;
	for (auto&& cel : cels)
		result.splice(result.end(), celFileToFrames(cel));
	return result;
}


std::list<std::vector<uint8_t>> celFileToFrames(const std::vector<uint8_t>& filedata) {
	if (filedata.size() < 8) throw 2;

	auto&& filedata32 = *reinterpret_cast<const std::vector<uint32_t>*>(&filedata);
	uint32_t cnt = *reinterpret_cast<const uint32_t*>(&filedata[0]);
	uint32_t fileSizeByHeader = filedata32[cnt + 1];
	if (filedata.size() != fileSizeByHeader) throw 1;

	std::list<std::vector<uint8_t>> frames;

	for (uint32_t i = 1; i <= cnt; ++i) {
		std::vector<uint8_t> framedata;
		size_t start_offset = filedata32[i];
		size_t end_offset = filedata32[i + 1];

		for (size_t j = start_offset; j < end_offset; ++j)
			framedata.push_back(filedata[j]);

		frames.emplace_back(framedata);
	}

	return frames;
}

std::vector<uint8_t> framesToCelFile(const std::list<std::vector<uint8_t>>& frames) {
	std::vector<uint8_t> celData;
	uint32_t framesAmount = static_cast<uint32_t>(frames.size());
	uint32_t headerSize = 4 + framesAmount * 4 + 4;

	auto&& celData32 = *reinterpret_cast<std::vector<uint32_t>*>(&celData);
	celData32.push_back(framesAmount);

	uint32_t offset = framesAmount * 4 + 4 + 4;
	for (auto&& frame : frames) {
		celData32.push_back(offset);
		offset += static_cast<uint32_t>(frame.size());
	}
	celData32.push_back(offset);

	for (auto&& frame : frames)
		celData.insert(celData.end(), frame.begin(), frame.end());

	return celData;
}

std::list<std::vector<uint8_t>> framesToCels(const std::list<std::vector<uint8_t>>& frames) {
	std::list<std::vector<uint8_t>> cels;
	uint32_t framesAmount = 1;
	uint32_t headerSize = 4 + framesAmount * 4 + 4;

	for (auto&& frame : frames) {
		std::vector<uint8_t> celData;
		auto&& celData32 = *reinterpret_cast<std::vector<uint32_t>*>(&celData);
		celData32.push_back(framesAmount);

		uint32_t offset = framesAmount * 4 + 4 + 4;
	
		celData32.push_back(offset);
		offset += static_cast<uint32_t>(frame.size());
		celData32.push_back(offset);

		celData.insert(celData.end(), frame.begin(), frame.end());

		cels.emplace_back(celData);
	}

	return cels;
}
