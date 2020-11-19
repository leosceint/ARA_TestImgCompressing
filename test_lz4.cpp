#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "lz4hc.h"

using namespace std;

void lz4_test_def(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
# define LZ4_MEMORY_USAGE 20


	cout << endl << "-== LZ4 (default) test start ==-" << endl;

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
			int size_status = LZ4_compress_fast(src, dst, static_cast<int>(src_size), dst_size, 1);
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

void lz4_test_hc(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
# define LZ4_MEMORY_USAGE 20


	cout << endl << "-== LZ4 (High Compression) test start ==-" << endl;

	int size_stateHC = LZ4_sizeofStateHC();
	LZ4_streamHC_t* stateHC = reinterpret_cast<LZ4_streamHC_t*>(malloc(size_stateHC));
	
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
			int size_status = LZ4_compress_HC_extStateHC(stateHC, src, dst, static_cast<int>(src_size), dst_size, 2);
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

	free(stateHC);
}