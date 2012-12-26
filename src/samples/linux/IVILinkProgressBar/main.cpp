/* 
 * iviLINK SDK, version 1.1.19
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 
#include <QtGui/QApplication>
#include "ivilinkprogressbar.hpp"
#include <QApplication>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Logger.hpp"
#include "Types.hpp"
#include <stdio.h>
#include <pthread.h>

void *threadFunc(void *arg);
UInt8 buf[1]={0};


int main(int argc, char *argv[])
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("IVILinkProgressBar.main.socket"));
    QApplication a(argc, argv);
    IVILinkProgressBar w;
    w.setCursor(QCursor(Qt::BlankCursor));
    w.show();

    pthread_t pth;
    pthread_create(&pth, NULL, threadFunc, (void*)"socket listening");

    return a.exec();
}


void *threadFunc(void *arg)
{
    int sock, listener;
    struct sockaddr_in addr;
    int stopMessage = 0;
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("IVILinkProgressBar.main.socket"));
    listener = socket(AF_INET, SOCK_STREAM, 0);
    
    if (listener < 0)
    {
        LOG4CPLUS_ERROR(logger, "socket create error");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        LOG4CPLUS_ERROR(logger, "socket connect error");
    }

    listen(listener, 1);

    while (buf[0] != 1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            LOG4CPLUS_ERROR(logger, "socket accept error");
        }
        else 
        {
            stopMessage = recv(sock, buf, sizeof(buf), 0);
            printf("recv return %i\n", (int)buf[0]);
            LOG4CPLUS_INFO(logger,"recv_socket");
        }
        usleep(100000);
    }
    close(sock);
    exit(0);
}
