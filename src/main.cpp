#include <project/main.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <thread>
#include <functional>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <windows.h>
#include <filesystem>

int threadsFinished = 0;
std::vector<std::string> allLines;
std::vector<int> countList;
const int threadThreshold = 4000;

/// <summary>
/// Get the number of available device threads.
/// </summary>
/// <returns>int threads | the number of available threads.</returns>
int getThreads()
{
	// gets number of available threads, 1 if it errors
	unsigned int threads = std::min<unsigned int>(std::thread::hardware_concurrency(), 1);
	threads = 8; // testing

	std::cout << "Threads: " << threads << std::endl;
	return threads;
}


/// <summary>
/// Get the available device memory.
/// </summary>
/// <returns>int threads | the number of available threads.</returns>
ULONGLONG getMemory() {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	return static_cast<uintmax_t>(status.ullAvailPhys * 0.75); // makes sure to not use all available memory
}


/// <summary>
/// Counts the occurrences of a word in a line.
/// </summary>
/// <param name="line">Desired line.</param>
/// <param name="word">Desired word.</param>
/// <returns>int count | the number of occurrences. </returns>
void countOccurrences(const int threadNum, const std::string& line, const std::string word)
{

	std::string lowerLine = line;

	// remove line endings
	std::replace(lowerLine.begin(), lowerLine.end(), '\n', ' ');
	std::replace(lowerLine.begin(), lowerLine.end(), '\r', ' ');


	// set line and word to lowercase
	std::transform(lowerLine.begin(), lowerLine.end(), lowerLine.begin(), ::tolower);
	std::string lowerWord = word;
	std::transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);


	// remove punctuation
	lowerLine.erase(std::remove_if(lowerLine.begin(), lowerLine.end(), ::ispunct), lowerLine.end());


	auto count_time_start = std::chrono::high_resolution_clock::now();

	// Count the number of occurrences of the given word by incremementing pos.
	int pos = 0;
	int count = 0;
	while (true) {
		pos = lowerLine.find(lowerWord, pos);

		// stop when pos reaches end
		if (pos != std::string::npos) {
			count++;
			pos++;
		}
		else
			break;
	}

	auto count_time_stop = std::chrono::high_resolution_clock::now();
	auto count_time_dura = std::chrono::duration_cast<std::chrono::microseconds>(count_time_stop - count_time_start);
	std::cout << "Thread counting time: " << count_time_dura.count() << std::endl;

	countList[threadNum] = count;

	threadsFinished++;
}

void main() {

	auto startTotalTime = std::chrono::high_resolution_clock::now();

	int threadCount = getThreads();

	uintmax_t memory = getMemory();

	std::cout << "Memory: " << memory << " bytes" << std::endl;


	// -------------------------------- File Management --------------------------------

	auto startDataGatherTime = std::chrono::high_resolution_clock::now();

	std::string fileLines;
	std::filesystem::path directoryPath = std::filesystem::current_path().parent_path().parent_path().parent_path() / "src/texts";
	std::string directory = directoryPath.string();
	std::string word = "lorem";


	// for each text file in the directory
	for (const auto& textFile : std::filesystem::directory_iterator(directory)) {

		std::string filePath = textFile.path().string();
		std::ifstream inputFile(filePath);

		// If file doesnt open
		if (!inputFile.is_open()) {
			std::cout << "Error, file unopened" << std::endl;
			exit(EXIT_FAILURE);
		}

		std::cout << "File " << textFile << "opened" << std::endl;

		// -------- Storing file --------

		std::ostringstream buffer;

		std::uintmax_t fileSize = std::filesystem::file_size(filePath);
		std::cout << "File size: " << fileSize << " Bytes" << std::endl;


		// min of fileSize or memory and store as chunk
		int minimum = std::min<uintmax_t>(memory, fileSize);
		std::vector<char> chunk(minimum);


		// while not at the end of file
		while (!inputFile.eof()) {

			// read as much data within memory size and write to buffer.
			inputFile.read(chunk.data(), memory);
			buffer.write(chunk.data(), inputFile.gcount());
		}

		fileLines += buffer.str();
		inputFile.close();
		buffer.clear();
	}

	auto stopDataGatherTime = std::chrono::high_resolution_clock::now();
	auto stopDataGatherTime_duration = std::chrono::duration_cast<std::chrono::microseconds>(stopDataGatherTime - startDataGatherTime);
	std::cout << "Total file management time: " << stopDataGatherTime_duration.count() << std::endl;


	// -------------------------------- Thread Setup --------------------------------

	auto startTotalCountingTime = std::chrono::high_resolution_clock::now();

	int chunkSize = fileLines.size() / threadCount;
	countList.resize(threadCount);
	int endLine = 0;
	std::vector<std::thread> threads;

	std::cout << "chunkSize: " << chunkSize << std::endl;
	
	
	// if chunk size doesnt meet threshold, set to 1 thread to reduce overhead
	if (chunkSize < threadThreshold)
		threadCount = 1;

	
	// setup each thread
	for (int i = 0; i < threadCount; i++) {

		//add any change in endline to remove any potential overlap
		int startLine = (i * chunkSize) + endLine % chunkSize;


		if (i == threadCount - 1)
			endLine = fileLines.size();
		else
			endLine = startLine + chunkSize;


		// incremenets the endline position until it ends on a line ending rather than characters
		while (endLine < fileLines.size() && fileLines[endLine] != '\n' && fileLines[endLine] != '\r')
			endLine++;
		

		std::string chunk = fileLines.substr(startLine, static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(endLine) - startLine);

		threads.emplace_back(countOccurrences, i, chunk, word);
		//std::cout << i << " chunk: " << chunk << std::endl;

	}

	// Join all threads
	for (auto& thread : threads) {
		thread.join();
	}

	auto stopTotalCountingTime = std::chrono::high_resolution_clock::now();
	auto stopTotalCountingTimeDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopTotalCountingTime - startTotalCountingTime);
	std::cout << "Total Counting Time: " << stopTotalCountingTimeDuration.count() << std::endl;



	// -------------------------------- Output --------------------------------

	int totalCount = 0;

	// once threads finish output results
	while (true) {
		if (threadsFinished == threadCount) {
			allLines.clear();

			for (int i = 0; i < threadCount; i++) {
				totalCount += countList[i];
			}
			std::cout << "Total Counted: " << totalCount << std::endl;
			break;
		}
	}

	auto stopTotalTime = std::chrono::high_resolution_clock::now();
	auto TotalTimeDuration = std::chrono::duration_cast<std::chrono::microseconds>(stopTotalTime - startTotalTime);
	std::cout << "Total time: " << TotalTimeDuration.count() << std::endl;
}
