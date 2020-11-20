#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <filesystem>
#include <chrono>

#include "ImgHelper.h"

#include "lz4hc.h"

using namespace std;

void compression_lz4_def(list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension);

void compression_lz4_hc(LZ4_streamHC_t* stateHC, list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension);

void lz4_test_def(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation, const string& FileExtension)
{
# define LZ4_MEMORY_USAGE 20


	cout << endl << "-== LZ4 (default) test start ==-" << endl;

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
			compression_lz4_def(ImgsList, NamesList, PathOfCompressed, FileExtension);

			counter = 0;
			ImgsList.clear();
			NamesList.clear();
			--iter;
		}
	}

	if (ImgsList.size())
	{
		cout << endl << "Counter --> " << counter << endl;
		compression_lz4_def(ImgsList, NamesList, PathOfCompressed, FileExtension);
	}

	ImgsList.clear();
	NamesList.clear();
}

void lz4_test_hc(vector<filesystem::path>& PathList, const string& PathOfCompressed, const unsigned int segmentation, const string& FileExtension)
{
# define LZ4_MEMORY_USAGE 20


	cout << endl << "-== LZ4 (High Compression) test start ==-" << endl;

	list<vector<char>> ImgsList;
	list<string> NamesList;
	vector<char> img;
	
	int size_stateHC = LZ4_sizeofStateHC();
	LZ4_streamHC_t* stateHC = reinterpret_cast<LZ4_streamHC_t*>(malloc(size_stateHC));
	
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
			compression_lz4_hc(stateHC, ImgsList, NamesList, PathOfCompressed, FileExtension);

			counter = 0;
			ImgsList.clear();
			NamesList.clear();
			--iter;
		}
	}

	if (ImgsList.size())
	{
		cout << endl << "Counter --> " << counter << endl;
		compression_lz4_hc(stateHC, ImgsList, NamesList, PathOfCompressed, FileExtension);
	}

	free(stateHC);
	ImgsList.clear();
	NamesList.clear();
}

void compression_lz4_def(list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension)
{
	unsigned int file_number = 0;
	list<string>::iterator name_list_iter = Names.begin();
	auto t_old_fps = chrono::steady_clock::now();

	for (list<vector<char>>::iterator list_iter = Imgs.begin(); list_iter != Imgs.end(); ++list_iter)
	{
		size_t src_size = list_iter->size();
		int dst_size = LZ4_compressBound(static_cast<int>(src_size));

		char* src = new char[src_size];
		src = list_iter->data();
		char* dst = new char[dst_size];

		auto t_1 = chrono::steady_clock::now();
		int size_status = LZ4_compress_fast(src, dst, static_cast<int>(src_size), dst_size, 1);
		auto t_2 = chrono::steady_clock::now();
		auto t_new_fps = t_2;
		auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
		auto delta_fps = chrono::duration_cast<chrono::milliseconds>(t_new_fps - t_old_fps);
		float fps = 1000 / static_cast<float>(delta_fps.count());
		t_old_fps = t_new_fps;
		src = nullptr;

		if (size_status > 0)
		{
			cout << endl;
			cout << "#" << file_number + 1 << endl;
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
}

void compression_lz4_hc(LZ4_streamHC_t* stateHC, list<vector<char>>& Imgs, list<string>& Names, const string& PathOfCompressed, const string& FileExtension)
{
	unsigned int file_number = 0;
	list<string>::iterator name_list_iter = Names.begin();
	auto t_old_fps = chrono::steady_clock::now();

	for (list<vector<char>>::iterator list_iter = Imgs.begin(); list_iter != Imgs.end(); ++list_iter)
	{
		size_t src_size = list_iter->size();
		int dst_size = LZ4_compressBound(static_cast<int>(src_size));

		char* src = new char[src_size];
		src = list_iter->data();
		char* dst = new char[dst_size];

		auto t_1 = chrono::steady_clock::now();
		int size_status = LZ4_compress_HC_extStateHC(stateHC, src, dst, static_cast<int>(src_size), dst_size, 2);
		auto t_2 = chrono::steady_clock::now();
		auto t_new_fps = t_2;
		auto delta_ms = chrono::duration_cast<chrono::milliseconds>(t_2 - t_1);
		auto delta_fps = chrono::duration_cast<chrono::milliseconds>(t_new_fps - t_old_fps);
		float fps = 1000 / static_cast<float>(delta_fps.count());
		t_old_fps = t_new_fps;
		src = nullptr;

		if (size_status > 0)
		{
			cout << endl;
			cout << "#" << file_number + 1 << endl;
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
}
