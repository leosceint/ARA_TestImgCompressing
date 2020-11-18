#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <chrono>

#include "ImgHelper.h"
#include "zstd.h"

using namespace std;

int main()
{
	string path_InputImages = ".\\InputImages";
	string path_CompressedImages = ".\\CompressedImages";
	vector<filesystem::path> ImgsPathList;

	ImgsPathList = ImgHelper::MakePathList(path_InputImages);
	
	ImgHelper::MakeOrClearDirectory(path_CompressedImages);

	vector<char> img;

	for (vector<filesystem::path>::iterator iter = ImgsPathList.begin(); iter != ImgsPathList.end(); ++iter)
	{
		if (ImgHelper::ReadImgAsArray(*iter, img))
		{
			size_t src_size = img.size();
			size_t dst_size = ZSTD_compressBound(src_size);
			cout << "<Size> original File --> " << src_size << endl;
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
				cout << "<File> " << *iter << " be compressed" << endl;
				cout << "<Size> compressed File --> " << size_status << endl;
				cout << "<Ratio> --> " << size_status / static_cast<float>(src_size) << endl;
				cout << "<Delta> of compression (ms) --> " << delta_ms.count() << endl;

				char* output_buffer = new char[size_status];
				memcpy(output_buffer, dst, size_status);
				string compressed_FilePath = path_CompressedImages + "\\" + (iter->filename()).string() + ".zst";
				ImgHelper::SaveData(compressed_FilePath, output_buffer, size_status);
				
				delete[] output_buffer;
				//break;
			}

			delete[] dst;
			delete[] src;
		}
	}

	cout << endl << "PRESS ANY KEY TO END ... " << endl;
	getchar();
	return 0;
}
