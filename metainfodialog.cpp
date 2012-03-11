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

#include "metainfodialog.h"
#include "ui_metainfodialog.h"

namespace imup {

    MetainfoDialog::MetainfoDialog(const ImageFile &imf, QWidget *parent) :
        QDialog(parent), ui(new Ui::MetainfoDialog), mdl(new MetainfoModel(imf))
    {
        ui->setupUi(this);
        ui->treeView->setModel(mdl);
    }

    MetainfoDialog::~MetainfoDialog()
    {
        delete ui;
    }

    void MetainfoDialog::changeEvent(QEvent *e)
    {
        QDialog::changeEvent(e);
        switch (e->type()) {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }
}

void imup::MetainfoDialog::on_buttonBox_rejected()
{
    close();
}

void imup::MetainfoDialog::on_treeView_expanded(const QModelIndex &index)
{
    ui->treeView->resizeColumnToContents(index.column());
}
