#include "ImgHelper.h"

vector<filesystem::path> ImgHelper::MakePathList(const string& path)
{
	vector<filesystem::path> PathList;
	
	if (filesystem::exists(path) && filesystem::is_directory(path))
	{
		for (auto elem : filesystem::directory_iterator(path))
		{
			PathList.push_back(elem.path());
		}
	}
	return PathList;
}

void ImgHelper::ShowPathList(vector<filesystem::path>& PathList)
{
	for (vector<filesystem::path>::iterator iter = PathList.begin(); iter != PathList.end(); ++iter)
	{
		cout << *iter << endl;
	}
}

void ImgHelper::MakeOrClearDirectory(const string& path)
{
	if (filesystem::exists(path) && filesystem::is_directory(path))
	{
		vector<filesystem::path> PathList = MakePathList(path);
		for (const auto& path : PathList)
		{
			filesystem::remove(path);
		}
		cout << "<Directory> \"" << path << "\" was cleared !!! " << endl;
	}
	else
	{
		filesystem::create_directory(path);
		cout << "<Directory> \"" << path << "\" was created !!! " << endl;
	}
}

bool ImgHelper::ReadImgAsArray(const filesystem::path& path, vector<char>& ImgArray)
{
	ifstream f_in;
	f_in.open(path, ifstream::binary);

	if (!f_in.is_open())
	{
		cout << endl << "<File> " << path << " cannot be opened" << endl;
		return false;
	}

	istreambuf_iterator<char> begin(f_in), end;

	ImgArray.assign(begin, end);
	f_in.close();

	cout << endl << "<File> " << path << " was read and closed" << endl;
	return true;
}

void ImgHelper::SaveData(const string& path, const char* buffer, const size_t buffer_size)
{
	ofstream f_out;
	f_out.open(path, ofstream::binary);
	f_out.write(buffer, buffer_size);
	f_out.close();
	cout << "Write to File \"" << path << "\" " << endl;
}

void ImgHelper::SaveData(const string& path, const string& buffer)
{
	ofstream f_out;
	f_out.open(path, ofstream::binary);
	f_out << buffer;
	f_out.close();
	cout << "Write to File \"" << path << "\" " << endl;
}
