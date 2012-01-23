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

#ifndef IMUPWIN_H
#define IMUPWIN_H

#include <QtGui/QMainWindow>

namespace Ui {
    class imupWin;
}

namespace imup
{
    class imupWin : public QMainWindow
    {
            Q_OBJECT

        public:
            explicit imupWin(QWidget *parent = 0);
            ~imupWin();

        protected:
            void changeEvent(QEvent *e);

        private:
            Ui::imupWin *ui;
    };
}

#endif // IMUPWIN_H
