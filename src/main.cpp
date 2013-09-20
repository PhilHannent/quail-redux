/**
 * @file main.cpp Main file
 *
 * @Copyright (C) 2003-2004 Christian Hammond.
 * @Copyright (C) 2013      Phil Hannent.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA  02111-1307  USA
 */
#include <QApplication>
#include <QDebug>

#if defined(Q_OS_CYGWIN)
#include "QuailWinGlibEventLoop.h"
#endif
#include "QuailEventLoop.h"
#include "QuailMainWindow.h"

int main(int argc, char *argv[])
{
#if defined(Q_OS_CYGWIN)
    qDebug() << "MAIN.1";
    QuailEventDispatcherWinGlib quailEventLoop;
#endif
    //QuailEventDispatcherMarkTwo mainEvent;
    QCoreApplication a(argc, argv);
    qDebug() << "q";
    QQuailMainWindow w;
    qDebug() << "q2";
    w.show();

    return a.exec();
}
