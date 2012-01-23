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

#include <QDebug>

#include <QLabel>
#include <QApplication>

#include "imupwin.h"
#include "ui_imupwin.h"

#include "commonsimgobject.h"
#include "commonsimgwidget.h"

namespace imup
{
    imupWin::imupWin(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::imupWin)
    {
        ui->setupUi(this);
        makeToolbarButtons();

#ifdef QT_DEBUG
        for(int i = 1; i < QApplication::arguments().size(); ++i)
            ui->ScrollLay->addWidget(new CommonsImgWidget(QApplication::arguments().at(i)));
#endif

    }

    imupWin::~imupWin()
    {
        delete ui;
    }

    void imupWin::changeEvent(QEvent *e)
    {
        QMainWindow::changeEvent(e);
        switch (e->type()) {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }

    void imupWin::makeToolbarButtons()
    {
        ui->mainToolBar->addAction(ui->action_Quit);
        ui->projectToolBar->addAction(ui->actionNew_project);
        ui->projectToolBar->addAction(ui->action_Open_project);
        ui->projectToolBar->addAction(ui->menu_Save_project->menuAction());
    }

}
