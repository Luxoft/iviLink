/* 
 * 
 * iviLINK SDK, version 1.0
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
 * 
 */



#include "ui_auth.h"
#include "AuthenticationDialog.hpp"
#include "utils/misc/Logger.hpp"

using namespace authentication;

int main(int argc, char *argv[])
{
    Logger logger = Logger::getInstance(LOG4CPLUS_TEXT("applications.AuthenticationApplication"));

    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

    LOG4CPLUS_INFO(logger, "Authentication main()");

    QApplication app(argc, argv);
    QWidget *bottomWidget = new QWidget;
    bottomWidget->setAttribute(Qt::WA_TranslucentBackground);
    bottomWidget->setAttribute(Qt::WA_NoSystemBackground);
    bottomWidget->setWindowFlags(Qt::FramelessWindowHint);

    QWidget *topWidget = new QWidget(bottomWidget);
    topWidget->setStyleSheet("image: url(:images/pin/pin-b-01.png); border-width : 0px");
    topWidget->setFixedSize(QSize(328, 354));

    AuthenticationDialog ui(topWidget);

    return app.exec();

}
