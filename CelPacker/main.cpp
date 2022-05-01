#include <string>
#include <iostream>
#include <list>
#include <vector>
#include "functions.h"

void readArguments(const char* array[], const int size,
	std::string& inputFileName, std::string& outputFileName, int& status, size_t& startFromFrameIndex)
{
	std::string outputFormat;
	for (int i = 0; i < size; ++i) {
		if (array[i][0] == '-' && array[i][2] == '\0') {
			switch (array[i][1]) {
			case 'i':
				if(i < size - 1)
				inputFileName = std::string(array[i + 1]);
				break;
			case 'o':
				if (i < size - 1)
				outputFileName = std::string(array[i + 1]);
				break;
			case 'u':
				status = 1;
				break;
			case 'p':
				status = 0;
				break;
			case 'f':
				if (i < size - 1)
					startFromFrameIndex = atoi(array[i + 1]);
				break;
			default:
				throw 4;
			}
		}
	}
}

int main(const int argc, const char* argv[]) {
	if (argc <= 1) {
		std::cout
			<< "How To Use:\nto pack \"input_file_###.cel\" in one file\n"
			<< "> TH2CelPacker.exe -p -i \"input_file_0.cel\" -o \"output_file.cel\"\n"
			<< "to unpack \"input_file.cel\" in \"output_file_###.cel\"\n"
			<< "> TH2CelPacker.exe -u -i \"input_file.cel\" -o \"output_file.cel\"\n\n"
			<< "Press any key to close this program..."
			<< std::endl;
		std::cin.ignore();
		return -1;
	}

	std::string inputFileName("input.cel");
	std::string outputFileName("output.cel");
	int status = 0; // pack/unpack
	size_t startFromFrameIndex = 0; // first frame

	readArguments(argv, argc, inputFileName, outputFileName, status, startFromFrameIndex);

	std::cout
		<< "Input: " << inputFileName
		<< "\nOutput: " << outputFileName
		<< "\nMode: " << ( status ? "unpack" : "pack" )
		<< std::endl;

	std::string::size_type dotSplitter;

	dotSplitter = inputFileName.rfind(".");
	std::string inFileName(inputFileName.substr(0, dotSplitter));
	std::string inFileType(inputFileName.substr(dotSplitter, inputFileName.size()));

	dotSplitter = outputFileName.rfind(".");
	std::string outFileName(outputFileName.substr(0, dotSplitter));
	std::string outFileType(outputFileName.substr(dotSplitter, outputFileName.size()));

	std::string::size_type underscoreSplitter = inFileName.rfind("_");

	std::list<std::vector<uint8_t>> frames;

	if (underscoreSplitter < inFileName.size()
		&& inputFileName[underscoreSplitter + 1] >= '0'
		&& inputFileName[underscoreSplitter + 1] <= '9')
	{
		inFileName = std::string(inputFileName.substr(0, underscoreSplitter+1));
		std::cout << "Reading files by name " << inFileName << "###" << inFileType << std::endl;
		auto&& files = openFiles(inFileName, inFileType, startFromFrameIndex);
		std::cout << "Total files: " << files.size() << std::endl;
		frames = celsToFrames(files);
	}
	else {
		auto&& file = getFileData(inFileName + inFileType);
		frames = celFileToFrames(file);
	}

	if (startFromFrameIndex) std::cout << "First frame index: " << startFromFrameIndex << std::endl;
	std::cout << "Frames amount: " << frames.size() << std::endl;
	std::cout << "File export by name: " << outFileName << (status ? "_###" : "") << outFileType << std::endl;

	if(!status){
		auto&& newfile = framesToCelFile(frames);
		std::cout << "Output file size: " << newfile.size() << " byte" << std::endl;
		saveCelFile(newfile, outFileName + outFileType);
	}
	else {
		std::list<std::vector<uint8_t>> fileList = framesToCels(frames);
		size_t allFilesSize = 0;
		int i = 0;
		for (auto&& file : fileList) {
			saveCelFile(file, outFileName + "_" + std::to_string(i) + outFileType);
			++i;
			allFilesSize += file.size();
		}
		std::cout << "Total output files: " << frames.size() << " (" << allFilesSize << " byte)" << std::endl;
	}

	return 0;
}

