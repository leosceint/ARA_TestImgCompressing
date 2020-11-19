#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "zstd.h"

void zstd_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const string& FileExtension)
{
	cout << endl << "-== Zstandart test start ==-" << endl;
	
	list<vector<char>> ImgsList;
	list<string> NamesList;
	vector<char> img;

	ZSTD_CCtx* zstd_compression_context = ZSTD_createCCtx();

	unsigned int counter = 0;

	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		if (counter < 5)
		{
			if (ImgHelper::ReadImgAsArray(*iter, img))
			{
				ImgsList.push_back(img);
				NamesList.push_back((iter->filename()).string());
				++counter;
			}
		}
		else
		{
			cout << endl << "Counter --> " << counter << endl;
			unsigned int file_number = 0;
			list<string>::iterator name_list_iter = NamesList.begin();
			auto t_old_fps = chrono::steady_clock::now();

			for (list<vector<char>>::iterator list_iter = ImgsList.begin(); list_iter != ImgsList.end(); ++list_iter)
			{
				size_t src_size = list_iter->size();
				size_t dst_size = ZSTD_compressBound(src_size);
				char* src = new char[src_size];
				src = list_iter->data();
				char* dst = new char[dst_size];

				auto t_1 = chrono::steady_clock::now();
				size_t size_status = ZSTD_compressCCtx(zstd_compression_context, dst, dst_size, src, src_size, 1);
				auto t_2 = chrono::steady_clock::now();
				auto t_new_fps = t_2;
				auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
				auto delta_fps = chrono::duration_cast<chrono::milliseconds>(t_new_fps - t_old_fps);
				float fps = 1000 / static_cast<float>(delta_fps.count());
				t_old_fps = t_new_fps;
				src = nullptr;

				if (!ZSTD_isError(size_status))
				{
					cout << endl;
					cout << "#" << file_number+1 << endl;
					cout << "<Size> original File --> " << src_size << endl;
					cout << "<File> " << *name_list_iter << " be compressed" << endl;
					cout << "<Size> compressed File --> " << size_status << endl;
					cout << "<Ratio> --> " << src_size / static_cast<float>(size_status) << endl;
					cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;
					cout << "<FPS> --> " << fps << endl;

					char* output_buffer = new char[size_status];
					memcpy(output_buffer, dst, size_status);
					string compressed_FilePath = PathOfCompressed + "\\" + *name_list_iter + FileExtension;
					ImgHelper::SaveData(compressed_FilePath, output_buffer, size_status);
					
					++name_list_iter;
					++file_number;
					delete[] output_buffer;
					//break;
				}

				delete[] dst;
				delete[] src;
			}

			counter = 0;
			ImgsList.clear();
			NamesList.clear();
			--iter;
		}
	}
	ZSTD_freeCCtx(zstd_compression_context);
}
