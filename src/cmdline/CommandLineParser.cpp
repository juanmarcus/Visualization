/*
 * CommandLineParser.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: juanmarcus
 */

#include "CommandLineParser.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace std;
using namespace boost;

CommandLineParser::CommandLineParser()
{
	config = new po::options_description("Allowed options");
}

CommandLineParser::~CommandLineParser()
{

}

void CommandLineParser::addOption(const char* name,
		const po::value_semantic* s, const char* description, bool required)
{
	config->add_options()(name, s, description);

	if (required)
	{
		saveRequired(name);
	}
}

void CommandLineParser::printUsage()
{
	cout << *config << "\n";
}

bool CommandLineParser::parse(int argc, char* argv[])
{
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, *config), vm);
	} catch (po::unknown_option ex)
	{
		cout << ex.what() << "\n\n";
		printUsage();
		return false;
	}

	vector<string>::iterator it = requiredOptions.begin();
	vector<string>::iterator itEnd = requiredOptions.end();

	for (; it != itEnd; ++it)
	{
		if (!vm.count(*it))
		{
			cout << "Missing required option: --" << *it << "\n\n";
			printUsage();
			return false;
		}
	}

	po::notify(vm);
	return true;
}

void CommandLineParser::saveRequired(const char* name)
{
	vector<string> tokens;
	split(tokens, name, is_any_of(","));

	requiredOptions.push_back(tokens[0]);
}
