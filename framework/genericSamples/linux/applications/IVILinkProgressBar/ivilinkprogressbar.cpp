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
#include "ivilinkprogressbar.hpp"
#include <QLabel>
#include <QWidget>
#include <QMovie>
#include <QPalette>
#include <QMouseEvent>

static const char* shutdown = "../applications/shutdown.sh";

IVILinkProgressBar::IVILinkProgressBar(QWidget *parent)
    : QWidget(parent)
{
    movie = NULL;
    const int windowWidth = 800;
    const int windowHeight = 600;

    QImage qimg = QImage(":img/Ai.png").copy(0, 100, windowWidth, windowHeight);
    QPalette palette;
    palette.setBrush(this->backgroundRole(), QBrush(qimg) );
    this->setPalette(palette);
    adjustSize();
    setFixedSize(windowWidth, windowHeight);
    setWindowFlags(Qt::FramelessWindowHint);

    QLabel *label = new QLabel(this);
    movie = new QMovie(":img/progressBar.gif");

    label->setMovie(movie);
    movie->start();
    label->move(width() / 2 - movie->frameRect().width() / 2 - 1
              , height() / 2 - movie->frameRect().height() / 2 - 1 );
    

}

void IVILinkProgressBar::mousePressEvent(QMouseEvent * event)
{
    if (QRect(0, 0, 50, 50).contains(event->pos() ) )
        system(shutdown);
}

IVILinkProgressBar::~IVILinkProgressBar()
{
    delete movie;
}
