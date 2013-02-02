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


#include <cstdlib>
#include <vector>
#include <cstring>
//#include <vlc/libvlc.h>
//#include "CServer.h"
#include "iostream"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "string"
#include "stdio.h"
#include "assert.h"

//#include <vlc/libvlc.h>
//#include <vlc/vlc.h>

class CServer {
public:
    CServer();
    virtual ~CServer();

void  onKeepPlay();
void  onPause();
void  onStop();
void  onPlay(std::string const& sout, std::string const& url);

private:
    //libvlc_instance_t *vlc;
    const char*       media_name; 
};


CServer::CServer() {
    //vlc = libvlc_new(0, NULL);
    media_name = "media";
}

void CServer::onPlay(std::string const& sout, std::string const& url) {
    // to char sout url
    const char * mSout = sout.c_str();
    const char * mUrl = url.c_str();
     std::cout << "onPlay" << std::endl;
    //libvlc_vlm_add_broadcast(vlc, media_name, mUrl, mSout, 0, NULL, true, false);
    //libvlc_vlm_play_media(vlc, media_name);
}

void CServer::onStop() {

    //libvlc_vlm_stop_media(vlc, media_name);
    //libvlc_vlm_release(vlc);
     std::cout << "onStop" << std::endl;
}

void CServer::onPause() {

    //libvlc_vlm_pause_media(vlc, media_name);
     std::cout << "onPause" << std::endl;
}

void CServer::onKeepPlay() {

    //libvlc_vlm_play_media(vlc, media_name);
     std::cout << "onKeepPlay" << std::endl;
}

CServer::~CServer() {

    //libvlc_vlm_release(vlc);
     std::cout << "CServer" << std::endl;
}
/*
std::vector<std::string> split( const std::string& raw_command, const char delim= ';' )
{
	
}
*/
using namespace std;


void split(const string& str, const string& delimiters , vector<string>& tokens)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}


int main(int argc, char** argv) {

    int fifo_name;
    CServer * server;
    int fd;
    std::string sout("sout");
    std::string url("url");
    std::string comm("comm");
    const char stop[] = "stop";
    const char keepplay[] = "keepplay";
    const char pause[] = "pause";
    const char audio[] = "audio";
    const char video[] = "video";
    const char play[] = "play";
    char buff[256];


    if (argc != 2)
    {
        //error
        std::cout << "usage: test <pipenumber>" << std::endl;
        return 1;
    }

    if( sscanf(argv[1], "%i", &fifo_name) != 1 )
	{ std::cout<<"invalid argument!\n";
	  return 2; }

    std::cout << fifo_name << std::endl;
    FILE* file = fdopen(fifo_name, "r");
    assert(file);

    while (!feof(file) && !ferror(file) && fgets(buff, sizeof (buff), file) != NULL) {
    
    //parse(buff, comm, sout, url);
    vector<string> parse_result;
    split( string(buff, buff+strlen(buff)-1), ";", parse_result);
    //for( int i= 0; i<parse_result.size(); ++i )
    //	cout<<"pr= \"" << parse_result[i] << "\"\n";
    if (parse_result.size() > 0)
{
    
    if (parse_result[0] == stop) {
        server->onStop();
    } else if (parse_result[0] == pause) {
        server->onPause();
    } else if (parse_result[0] == keepplay) {
        server->onKeepPlay();
    } else if (parse_result[0] == play) {
        server->onPlay(parse_result[1], parse_result[2]);
    }

    }
}
    server->onStop();
    close(fifo_name);

    return 0;
}


