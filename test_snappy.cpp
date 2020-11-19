#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "snappy-c.h"

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
