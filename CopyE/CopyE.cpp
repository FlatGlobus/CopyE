
#include "stdafx.h"
#include "Copy.h"
#include <vector>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace std;

int wmain(int argc, TCHAR* argv[])
{
	fileVector source;
	CCopy copy(source);
	try
	{
		po::options_description desc;
		desc.add_options()
			("help,h", "command line options")
			("source,s", po::wvalue<std::vector<wstring>>()->multitoken(), "source files/masks *.txt c:\\te??t.cpp")
			("destination,d", po::wvalue<wstring>(), "destination path c:\\temp")
			("recursive,r", po::wvalue<wstring>(), "recursive source|destination sd")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);

		if (vm.count("help") || vm.empty())
		{
			cout << desc << "\n";
			return 0;
		}


		if (vm.count("source"))
		{
			for (int i = 0; i < vm["source"].as<std::vector<wstring>>().size(); i++)
			{
				source.push_back(vm["source"].as<std::vector<wstring>>()[i]);
			}
		}

		if (source.empty())
		{
			cout << "source files/masks is not set.\n";
			return 1;
		}

		if (!vm.count("destination"))
		{
			cout << "destination path is not set.\n";
			return 1;
		}

		bool recursiveSource = false;
		bool recursiveDestination = false;

		if (vm.count("recursive"))
		{
			if (vm["recursive"].as<wstring>().find(_T('s')) != wstring::npos)
			{
				recursiveSource = true;
			}
			if (vm["recursive"].as<wstring>().find(_T('d')) != wstring::npos)
			{
				recursiveDestination = true;
			}
		}

		copy.SetDestination(vm["destination"].as<wstring>());
		copy.SetRecursive(recursiveSource, recursiveDestination);

		copy.Process();
	}
	catch (std::exception & e)
	{
		std::cerr << "Error: " << e.what() << "\n";
	}
	return 0;
}
