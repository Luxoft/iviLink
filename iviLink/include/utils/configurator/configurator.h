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


#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <string>
#include <list>

namespace iviLink
{
	namespace conf

	{

	struct kvpair
	{
		std::string key;
		std::string value;
		kvpair() : key(), value() {}
		kvpair( std::string k, std::string v) : key(k), value(v) {}

		bool operator == (const kvpair& p) const 
		{ 
			return (key == p.key && value == p.value);
		}
	};	
	
	std::list <std::string> ReadFile(const std::string name);
	std::string Split(std::string str, char delimiter, std::string& right);
	std::string Trim(std::string str);
	kvpair ParserLine(std::string str);
	std::list <kvpair> Parse(const std::list<std::string> mnt);
	

	class Configurator
	{

	public:
		
		std::string filepath;
		std::list<iviLink::conf::kvpair> paramlist;		

	public:

		Configurator(std::string name)
		{

			//filepath =  ex /home/axis/name/....;
			std::list<iviLink::conf::kvpair> pair = Parse(ReadFile("mconfig.ini")); 
			for (std::list<iviLink::conf::kvpair>::const_iterator i = pair.begin(); i != pair.end(); i++)
		    	{
		       		if (i->key == name)
				{
			    		filepath = i->value;
					break;
				}
		    	};
			
			paramlist = Parse(ReadFile(filepath)); 

		}

		~Configurator()
		{
			save();
		}

		/**
		* Get value of key
		* @param key   - name of parameter
		* @return value 
		*/
		std::string  getParam(const std::string key);

		/**
		* save list of paramerts to the disk
		* @return value 
		*/
		int  save();
		
		/**
		* Set value(int) of key
		* @param name  - name of the requesting module
		* @param key   - name of parameter
		* @param value - new value  
		* @return none
		*/
		int setParam(const std::string key, const int value);

		/**
		* Set value(string) of key
		* @param name  - name of the requesting module
		* @param key   - name of parameter
		* @param value - new value  
		* @return none
		*/
		int setParam(const std::string key, const std::string value);

		/**
		* Set value(float) of key
		* @param name  - name of the requesting module
		* @param key   - name of parameter
		* @param value - new value  
		* @return none
		*/
		int  setParam(const std::string key, const float value);

		/**
		* add string <key=value> to file 
		* @param file  - name of the file
		* @param key   - name of parameter
		* @param value - value  
		* @return none
		*/
		int  addParam(const std::string key, const std::string value);
		
		/**
		* delete value and  key from file
		* @param name  - name of the requesting module
		* @param key   - name of parameter
		* @return value 
		*/
		int  delParam(const std::string key);


	};

	}
}


#endif // CONFIGURATOR_H


