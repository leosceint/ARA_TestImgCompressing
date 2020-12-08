#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "snappy-c.h"

void compression_snappy(list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension);

void snappy_test(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation, const string& FileExtension)
{
	cout << endl << "-== snappy test start ==-" << endl;

	list<vector<char>> ImgsList;
	list<string> NamesList;
	vector<char> img;

	unsigned int counter = 0;

	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		if (counter < segmentation)
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
			compression_snappy(ImgsList, NamesList, PathOfCompressed, FileExtension);

			counter = 0;
			ImgsList.clear();
			NamesList.clear();
			--iter;
		}
	}

	if (ImgsList.size())
	{
		cout << endl << "Counter --> " << counter << endl;
		compression_snappy(ImgsList, NamesList, PathOfCompressed, FileExtension);
	}

	ImgsList.clear();
	NamesList.clear();
}

void compression_snappy(list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension)
{
	unsigned int file_number = 0;
	list<string>::iterator name_list_iter = Names.begin();
	auto t_old_fps = chrono::steady_clock::now();
	auto t_1_avr_fps = t_old_fps;
	chrono::steady_clock::time_point t_2_avr_fps;

	for (list<vector<char>>::iterator list_iter = Imgs.begin(); list_iter != Imgs.end(); ++list_iter)
	{
		size_t src_size = list_iter->size();
		size_t dst_size = snappy_max_compressed_length(src_size);
		char* src = new char[src_size];
		src = list_iter->data();
		char* dst = new char[dst_size];

		auto t_1 = chrono::steady_clock::now();
		unsigned int status = snappy_compress(src, src_size, dst, &dst_size);
		auto t_2 = chrono::steady_clock::now();
		auto t_new_fps = t_2;
		auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
		auto delta_fps = chrono::duration_cast<chrono::milliseconds>(t_new_fps - t_old_fps);
		float fps = 1000 / static_cast<float>(delta_fps.count());
		t_old_fps = t_new_fps;
		t_2_avr_fps = t_new_fps;
		src = nullptr;

		if (status == SNAPPY_OK)
		{
			cout << endl;
			cout << "#" << file_number + 1 << endl;
			cout << "<Size> original File --> " << src_size << endl;
			cout << "<File> " << *name_list_iter << " be compressed" << endl;
			cout << "<Size> compressed File --> " << dst_size << endl;
			cout << "<Ratio> --> " << src_size / static_cast<float>(dst_size) << endl;
			cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;
			cout << "<FPS> --> " << fps << endl;

			char* output_buffer = new char[dst_size];
			memcpy(output_buffer, dst, dst_size);
			string compressed_FilePath = PathOfCompressed + "\\" + *name_list_iter + FileExtension;
			ImgHelper::SaveData(compressed_FilePath, output_buffer, dst_size);

			++name_list_iter;
			++file_number;
			delete[] output_buffer;
			//break;
		}

		delete[] dst;
		delete[] src;
	}

	float delta_avr_s = chrono::duration_cast<chrono::milliseconds>(t_2_avr_fps - t_1_avr_fps).count() / 1000.0f;
	cout << endl << "<Average in time FPS> : " << file_number / delta_avr_s << endl;
}
