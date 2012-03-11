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

#ifndef METAINFODIALOG_H
#define METAINFODIALOG_H

#include <QtGui/QDialog>
#include <QPointer>

#include "metainfomodel.h"
#include "imagefile.h"

namespace Ui {
    class MetainfoDialog;
}


namespace imup
{
    class MetainfoDialog : public QDialog
    {
            Q_OBJECT

        public:
            explicit MetainfoDialog(const ImageFile &imf, QWidget *parent = 0);
            ~MetainfoDialog();

        protected:
            void changeEvent(QEvent *e);

        private slots:
            void on_buttonBox_rejected();

            void on_treeView_expanded(const QModelIndex &index);

        private:
            Ui::MetainfoDialog *ui;
            QPointer<MetainfoModel> mdl;
    };
}

#endif // METAINFODIALOG_H
