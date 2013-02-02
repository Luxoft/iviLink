/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#include <string>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

#include "configurator.h"

using namespace iviLink::conf;


std::list<std::string> iviLink::conf::ReadFile(const std::string name)
{
    std::list<std::string> result;
    std::ifstream inifile(name.c_str(), std::ios::in | std::ios::binary);
    if ( !inifile.is_open() )
	return result;

    while (!inifile.eof() && !inifile.bad())
    {
        char readbuf[2048];
        inifile.getline(&readbuf[0], sizeof(readbuf) - 1);
	result.push_back(readbuf);
    };
    return result;
}


std::string iviLink::conf::Split(std::string str, char delimiter, std::string& right)
{
	size_t pos = str.find(delimiter);	
	right = str.substr(pos + 1);	
	return str.substr(0,pos);

}

std::string iviLink::conf::Trim(std::string str)
{
	std::string rstr = str.substr(0, str.find_last_not_of(" ") + 1);
	
 	for (size_t i = 0; i < rstr.size(); i++)
		if (rstr[i] > ' ')
			return rstr.substr(i);
	return " ";		
}


iviLink::conf::kvpair iviLink::conf::ParserLine(std::string str)
{	
	iviLink::conf::kvpair result;
	result.key = Trim(Split (str , '=' , result.value));
	result.value = Trim(result.value);
	return result;
}

std::list<iviLink::conf::kvpair> iviLink::conf::Parse(const std::list<std::string> mnt)
{
	std::list<iviLink::conf::kvpair> result;
    	kvpair ret;
	size_t pos;

    		for (std::list<std::string>::const_iterator i = mnt.begin(); i != mnt.end(); i++)
    		{	
			pos = i->find("=");
			if (pos != std::string::npos)
			{
				ret = ParserLine(*i);		
				result.push_back(ret); 
			}		
		}
	return result;
}

int   iviLink::conf::Configurator::addParam(const std::string key, const std::string value)
{
	kvpair kv(key, value);
	paramlist.push_back(kv);
	return 1;
}

std::string  iviLink::conf::Configurator::getParam(const std::string key)
{
	for (std::list<kvpair>::const_iterator i = paramlist.begin(); i != paramlist.end(); i++)
    	{
       		if (i->key == key)
			return i->value;
    	};
	return "";
}

int iviLink::conf::Configurator:: delParam(const std::string key)
{
	for (std::list<kvpair>::const_iterator i = paramlist.begin(); i != paramlist.end(); i++)
    	{
       		if (i->key == key)
		{
			kvpair kv(i->key, i->value);
			paramlist.remove(kv);	
			break; 	  	
		}
	};	
	return 1;
}

int iviLink::conf::Configurator::setParam(const std::string key, const std::string value)
{	
	
	iviLink::conf::Configurator::delParam(key);
	
	return iviLink::conf::Configurator::addParam(key,value);
}

int iviLink::conf::Configurator::setParam(const std::string key, const int value)
{
	char ch[20];
	sprintf(ch, "%d", value);
	return iviLink::conf::Configurator::setParam(key, (const char*) ch);
}
	
int iviLink::conf::Configurator::setParam( const std::string key, const float value)
{
	char ch[20];
	sprintf(ch, "%f", value);
	return iviLink::conf::Configurator::setParam(key, (const char*) ch);
}

int   iviLink::conf::Configurator::save()
{
	const char *file_name = filepath.c_str();
	std::ofstream out(file_name);

	if ( !out.is_open() )
			return 0;
	for (std::list<kvpair>::const_iterator i = paramlist.begin(); i != paramlist.end(); i++)
    	{
         	out<< i->key + "=" + i->value << std::endl;
    	};
	out.close();
	return 1;	
}

