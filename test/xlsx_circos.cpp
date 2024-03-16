#include <circos/read_excel.h>
#include <iostream>

#include <filesystem>
#include <ctime>

using namespace spiritsaway::circos;

using namespace std;

int main(int argc, char** argv)
{
	if(argc < 2 || argc > 3)
	{
		cout<< "this program need: \n1. the xlsx file path \n2.(optional) specify which file would be generated -png or -svg\nif optional argument not provided then png and svg whould be generated\n";
		return 0;
	}
	std::filesystem::path cur_file_path = std::string(argv[1]);
	auto cur_file_name = cur_file_path.filename();
	if(cur_file_name.empty())
	{
		cout<<"invalid file_path "<<argv[1];
	}
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%m-%Y-%H-%M-%S", timeinfo);
	std::string file_name = cur_file_path.stem().string() +"-" + std::string(buffer);
	std::string png_file_name = file_name + ".png";
	std::string svg_file_name = file_name + ".svg";
	if(argc == 3)
	{
		auto file_opt = std::string(argv[2]);
		if(file_opt == "-png")
		{
			svg_file_name = "";
		}
		else if(file_opt == "-svg")
		{
			png_file_name = "";
		}
		else
		{
			cout<<"the second argument should be -png or -svg"<<endl;
			return 0;
		}
	}
	cout << "png file " << png_file_name << " svg file " << svg_file_name << endl;
	read_xlsx_and_draw(std::string(argv[1]), png_file_name, svg_file_name);
	return 0;
}