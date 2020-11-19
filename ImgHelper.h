#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iterator>

using namespace std;

class ImgHelper
{
public:
	static vector<filesystem::path> MakePathList(const string& path);

	static void ShowPathList(vector<filesystem::path>& PathList);

	static void MakeOrClearDirectory(const string& path);

	static bool ReadImgAsArray(const filesystem::path& path, vector<char>& ImgArray);

	static void SaveData(const string& path, const char* buffer, const size_t buffer_size);

	static void SaveData(const string& path, const string& buffer);

private:
	ImgHelper() {};
	~ImgHelper() {};
	ImgHelper(ImgHelper const&) = delete;
	ImgHelper& operator= (ImgHelper const&) = delete;
};

