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
#include <QtGui/QApplication>
#include <QApplication>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>

#include "ivilinkprogressbar.hpp"
#include "Logger.hpp"
#include "Types.hpp"
#include "TcpSocket.hpp"

void *threadFunc(void *arg);
UInt8 buf[1]={0};


int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));
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
    TcpSocket tcpSocket(3425);
    tcpSocket.initServer();
    UInt8 message;
    do
    {
        message = -1;
        tcpSocket.acceptSocket();
        tcpSocket.receiveMessage(message);
    } while (message != 1);
    tcpSocket.closeSocket();
    exit(0);
}
