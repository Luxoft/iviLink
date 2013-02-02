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
#include "qmlapplicationviewer.h"
#include <QDeclarativeContext>

#include <signal.h>
#include <sys/types.h>
#include <cerrno>
#include "BaseError.hpp"
#include "CButtonProcessor.h"

void shutdownLauncher()
{   
    char *subStr = 0;
    char line[15];   
    pid_t pid = 0;

    FILE *command = popen("pidof launcher", "r");

    fgets(line, 15, command); 
    subStr = strtok(line, " ");   
    while (subStr != NULL) 
    {               
        if ((pid = atoi(subStr)) > 0)
        {
            if(pid != getpid())
            {
                if (-1 == kill(-pid, SIGTERM))
                {
                    std::cout<<"Failed to kill launcher "<<BaseError::FormErrnoDescr(errno)<<std::endl;
                }
                else
                {
                    std::cout<<"Killed process launcher with pid = "<<convertIntegerToString(pid)<<std::endl;
                }
            }
        }
        subStr = strtok(NULL, " ");
    }
    pclose(command);
}


Q_DECL_EXPORT int main(int argc, char *argv[])
{
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("launcherLog.properties"));

    shutdownLauncher();

    QScopedPointer<QApplication> app(createApplication(argc, argv));

    CButtonProcessor* btnProcessor = NULL;
    if (QCoreApplication::arguments().contains("-n") ) 
        btnProcessor = new CButtonProcessor(false);
    else
        btnProcessor = new CButtonProcessor(true);

    QDeclarativeView view;
    view.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    view.rootContext()->setContextProperty("BtnProcessor", btnProcessor);
    view.setSource(QUrl::fromLocalFile("qml/launcher/main.qml"));
    view.setGeometry(0, 0, 800, 600);
    view.setCursor(QCursor(Qt::BlankCursor));
    view.setWindowFlags(Qt::FramelessWindowHint);
    view.show();

    btnProcessor->resetPressed();

    return app->exec();
}
