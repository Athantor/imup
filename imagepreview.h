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

#ifndef IMAGEPREVIEW_H
#define IMAGEPREVIEW_H

#include <QtGui/QDialog>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QTimer>

#include "ui_imagepreview.h"
#include "commonsimgobject.h"

namespace Ui {
    class ImagePreview;
}

namespace imup
{
    class ImagePreview : public QDialog
    {
            Q_OBJECT

        public:
            explicit ImagePreview(const CommonsImgObject *obj, QWidget *parent);
            ~ImagePreview();

        protected:
            void changeEvent(QEvent *e);
            void resizeEvent ( QResizeEvent * e);

        protected slots:
            virtual void toggleScaling(bool );
            virtual void scaleImage();
            virtual void performScaling();

            virtual void rotate();

        private:
            Ui::ImagePreview *ui;
            const CommonsImgObject *the_obj;
            QToolBar *tbar;
            QLabel *ImgView;
            QTimer *tmr;
            QPixmap org_pxm;

            QAction *act_toggle_fit;
            QAction *act_rotate;

            void makeActions();

    };

}

#endif // IMAGEPREVIEW_H
