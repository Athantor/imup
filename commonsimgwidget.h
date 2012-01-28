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

#ifndef COMMONSIMGWIDGET_H
#define COMMONSIMGWIDGET_H

#include <QtGui/QWidget>

#include "commonsimgobject.h"

namespace Ui {
    class CommonsImgWidget;
}

namespace imup
{

    class CommonsImgWidget : public QWidget
    {
            Q_OBJECT

        public:
            explicit CommonsImgWidget(CommonsImgObject *imob, QWidget *parent = 0);
            explicit CommonsImgWidget(const QString& filepath, QWidget *parent = 0);
            ~CommonsImgWidget();

            CommonsImgObject * getImgObj();
        protected:
            CommonsImgObject *img_obj;

            void changeEvent(QEvent *e);

        protected slots:
            virtual void setupFields();
            virtual QString makeThumbTooltipText();

        private:
            Ui::CommonsImgWidget *ui;

            void commonSetup();
    };

}

#endif // COMMONSIMGWIDGET_H
