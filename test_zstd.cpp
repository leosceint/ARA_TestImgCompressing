#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "zstd.h"

void zstd_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
	cout << endl << "-== Zstandart test start ==-" << endl;

	vector<char> img;
	ZSTD_CCtx* zstd_compression_context = ZSTD_createCCtx();

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
			size_t size_status = ZSTD_compressCCtx(zstd_compression_context, dst, dst_size, src, src_size, 1);
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
	ZSTD_freeCCtx(zstd_compression_context);
}
