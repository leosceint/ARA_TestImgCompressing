#include <string>
#include <iostream>
#include <filesystem>
#include <vector>

#include "ImgHelper.h"

using namespace std;

void zstd_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation = 5, const string& FileExtension=".zst");

void lz4_test_def(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation = 5, const string& FileExtension = ".lz4");

void lz4_test_hc(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation = 5, const string& FileExtension = ".lz4");

void snappy_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation = 5, const string& FileExtension = ".sz");

int main(int argc, char* argv[])
{
	string path_InputImages = ".\\InputImages";
	string path_CompressedImages = ".\\CompressedImages";
	string compressed_Algorithm = "zstd";

	if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (strcmp(argv[i], "-d") == 0)
			{
				++i;
				path_InputImages = argv[i];
				cout << "Input directory --> " << path_InputImages << endl;
			}
			else if (strcmp(argv[i], "-a") == 0)
			{
				++i;
				compressed_Algorithm = argv[i];
				cout << "Algorithm type --> " << compressed_Algorithm << endl;
			}
			else
			{
				cout << "Unknown params... Use default" << endl;
			}

		}
	}

	vector<filesystem::path> ImgsPathList;

	ImgsPathList = ImgHelper::MakePathList(path_InputImages);
	
	ImgHelper::MakeOrClearDirectory(path_CompressedImages);

	if (compressed_Algorithm == "zstd")
		zstd_test(ImgsPathList, path_CompressedImages, ImgsPathList.size());
	else if (compressed_Algorithm == "lz4_def" || compressed_Algorithm == "lz4")
		lz4_test_def(ImgsPathList, path_CompressedImages, ImgsPathList.size());
	else if (compressed_Algorithm == "lz4_hc")
		lz4_test_hc(ImgsPathList, path_CompressedImages, ImgsPathList.size());
	else if (compressed_Algorithm == "snappy")
		snappy_test(ImgsPathList, path_CompressedImages, ImgsPathList.size());
	else
		cout << "Unknown Algorithm!!! " << endl;
	

	cout << endl << "PRESS ANY KEY TO END ... " << endl;
	getchar();
	return 0;
}
