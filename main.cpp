/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
    
   Krzysztof Kundzicz <athantor+cpp@athi.pl>
*/

#include <QtGui/QApplication>
#include <QTextCodec>

#include "imupwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("imup");
    QApplication::setApplicationVersion("0.1");
    QApplication::setOrganizationName("Athantor");
    QApplication::setOrganizationDomain("athi.pl");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("utf8"));

    imup::imupWin w;
    w.show();
    
    return a.exec();
}
