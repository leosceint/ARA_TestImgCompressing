#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>

#include "ImgHelper.h"

#include "zstd.h"
#include "lz4.h"
#include "snappy-c.h"

using namespace std;

void zstd_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension=".zst");

void lz4_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension = ".lz4");

void snappy_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension = ".sz");

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

	if(compressed_Algorithm == "zstd")
		zstd_test(ImgsPathList, path_CompressedImages);
	else if(compressed_Algorithm == "lz4")
		lz4_test(ImgsPathList, path_CompressedImages);
	else if(compressed_Algorithm == "snappy")
		snappy_test(ImgsPathList, path_CompressedImages);
	

	cout << endl << "PRESS ANY KEY TO END ... " << endl;
	getchar();
	return 0;
}

void zstd_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
	cout << endl << "-== Zstandart test start ==-" << endl;

	vector<char> img;

	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		if (ImgHelper::ReadImgAsArray(*iter, img))
		{
			size_t src_size = img.size();
			size_t dst_size = ZSTD_compressBound(src_size);

			char* src = new char[src_size];
			src = img.data();
			char* dst = new char[dst_size];

			auto t_1 = chrono::steady_clock::now();
			size_t size_status = ZSTD_compress(dst, dst_size, src, src_size, 1);
			auto t_2 = chrono::steady_clock::now();
			auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
			src = nullptr;

			if (!ZSTD_isError(size_status))
			{
				cout << "<Size> original File --> " << src_size << endl;
				cout << "<File> " << *iter << " be compressed" << endl;
				cout << "<Size> compressed File --> " << size_status << endl;
				cout << "<Ratio> --> " << src_size / static_cast<float>(size_status) << endl;
				cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;

				char* output_buffer = new char[size_status];
				memcpy(output_buffer, dst, size_status);
				string compressed_FilePath = PathOfCompressed + "\\" + (iter->filename()).string() + FileExtension;
				ImgHelper::SaveData(compressed_FilePath, output_buffer, size_status);

				delete[] output_buffer;
				//break;
			}

			delete[] dst;
			delete[] src;
		}
	}
}

void lz4_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
	cout << endl << "-== LZ4 test start ==-" << endl;
	
	vector<char> img;

	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		if (ImgHelper::ReadImgAsArray(*iter, img))
		{
			size_t src_size = img.size();
			int dst_size = LZ4_compressBound(static_cast<int>(src_size));
			
			char* src = new char[src_size];
			src = img.data();
			char* dst = new char[dst_size];

			auto t_1 = chrono::steady_clock::now();
			size_t size_status = LZ4_compress_default(src, dst, static_cast<int>(src_size), dst_size);
			auto t_2 = chrono::steady_clock::now();
			auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
			src = nullptr;

			if (size_status > 0)
			{
				cout << "<Size> original File --> " << src_size << endl;
				cout << "<File> " << *iter << " be compressed" << endl;
				cout << "<Size> compressed File --> " << size_status << endl;
				cout << "<Ratio> --> " << src_size / static_cast<float>(size_status) << endl;
				cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;

				char* output_buffer = new char[size_status];
				memcpy(output_buffer, dst, size_status);
				string compressed_FilePath = PathOfCompressed + "\\" + (iter->filename()).string() + FileExtension;
				ImgHelper::SaveData(compressed_FilePath, output_buffer, size_status);

				delete[] output_buffer;
				//break;
			}

			delete[] dst;
			delete[] src;
		}
	}
}

void snappy_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
	cout << endl << "-== snappy test start ==-" << endl;

	vector<char> img;

	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		if (ImgHelper::ReadImgAsArray(*iter, img))
		{
			size_t src_size = img.size();
			size_t dst_size = snappy_max_compressed_length(src_size);
			char* src = new char[src_size];
			src = img.data();
			char* dst = new char[dst_size];

			auto t_1 = chrono::steady_clock::now();
			unsigned int status = snappy_compress(src, src_size, dst, &dst_size);
			auto t_2 = chrono::steady_clock::now();
			auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
			src = nullptr;

			if (status == SNAPPY_OK)
			{
				cout << "<Size> original File --> " << src_size << endl;
				cout << "<File> " << *iter << " be compressed" << endl;
				cout << "<Size> compressed File --> " << dst_size << endl;
				cout << "<Ratio> --> " << src_size / static_cast<float>(dst_size) << endl;
				cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;

				char* output_buffer = new char[dst_size];
				memcpy(output_buffer, dst, dst_size);
				string compressed_FilePath = PathOfCompressed + "\\" + (iter->filename()).string() + FileExtension;
				ImgHelper::SaveData(compressed_FilePath, output_buffer, dst_size);

				delete[] output_buffer;
				//break;
			}

			delete[] dst;
			delete[] src;
		}
	}
}
