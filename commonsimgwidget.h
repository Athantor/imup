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
#include <QEvent>
#include <QCalendarWidget>

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
            class CommonsImgWidgetEvent : public QEvent
            {
                public:
                    typedef enum
                    {
                        DeleteRequested,
                        MaxEvent
                    } CustomType;

                    CommonsImgWidgetEvent(CustomType c_type, CommonsImgWidget *sdr) : QEvent(QEvent::User),
                        custom_type(c_type), sender_wgt(sdr)
                    {
                    }

                    virtual CustomType customType() const
                    {
                        return custom_type;
                    }

                    virtual const CommonsImgWidget * senderWgt() const
                    {
                        return sender_wgt;
                    }

                protected:
                    CustomType custom_type;
                    CommonsImgWidget *sender_wgt;

            };

            explicit CommonsImgWidget(CommonsImgObject *imob, QWidget *parent = 0);
            explicit CommonsImgWidget(const QString& filepath, QWidget *parent = 0);
            ~CommonsImgWidget();

            CommonsImgObject * getImgObj();
            const CommonsImgObject * getImgObj() const ;
        protected:
            CommonsImgObject *img_obj;

            void changeEvent(QEvent *e);

        protected slots:
            virtual void setupFields();
            virtual QString makeThumbTooltipText();

            virtual void requestDeletion();

            virtual void showCal();
            virtual void setDateFromCal(const QDate &);
            virtual void setDateFromMetadata();

        private slots:
            void on_FileDescTxtEdit_textChanged();


        private:
            Ui::CommonsImgWidget *ui;
            QAction *act_exif_date;
            QAction *act_cal_date;
            QAction *act_exif_geo;
            QAction *act_show_geo_on_map;
            QScopedPointer<QCalendarWidget> calwgt;

            void makeActions();
            void commonSetup();

    };

}

#endif // COMMONSIMGWIDGET_H
