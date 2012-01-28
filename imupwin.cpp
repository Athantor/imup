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

#include <signal.h>

#include <QDebug>

#include <QApplication>
#include <QLabel>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "imupwin.h"
#include "ui_imupwin.h"

#include "commonsimgobject.h"
#include "uploadproject.h"

namespace imup
{
    const QString imupWin::unsaved_proj_path = QDir(QDesktopServices::storageLocation(QDesktopServices::DataLocation)).absoluteFilePath("unsaved_project.ini");
    imupWin* imupWin::_intance = 0;

    imupWin::imupWin(QWidget *parent) :
        QMainWindow(parent),  ui(new Ui::imupWin), proj(new UploadProject(this))
    {
        imupWin::_intance = this;

        ui->setupUi(this);
        connects();
        makeToolbarButtons();

        lockIt(false);

        //-----

        proj->setProjectFilePath(unsaved_proj_path);
        QFileInfo qfi(unsaved_proj_path);
                                  /* [meta]*/
        if(qfi.exists() && qfi.size() > 6  &&
                QMessageBox::question(this, tr("Rescue unsaved?"), tr("There seems to be an unsaved upload project "
                                                                      "from prevoius session. Load it?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            proj->loadFromFile(true);
        }
        else
        {
            proj->saveToFile();
        }
    }

    imupWin::~imupWin()
    {
        delete ui;
        lockIt(true);
    }

    imupWin *imupWin::instance()
    {
        return imupWin::_intance;
    }

    bool imupWin::saveProject()
    {
        QString sp = QFileDialog::getSaveFileName(this, tr("Save project…"), QDir::homePath(), tr("Project files (*.ini) (*.ini)"));
        if(sp.isEmpty() == false)
        {
            proj->saveToFile(0, sp);
            QFile::remove(unsaved_proj_path);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool imupWin::loadProject()
    {
        if(proj->objects().size() > 0 && proj->projectFilePath() == unsaved_proj_path)
        {
            if(QMessageBox::question(this, tr("Save unsaved?"), tr("There are unsaved changes. Do you wish to save them before opening other project?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                if(saveProject() == false)
                    return false;
            }
        }

        QString qs = QFileDialog::getOpenFileName(this, tr("Open project…"), QDir::homePath(), tr("Project files (*.ini) (*.ini);;All files (*) (*)"));
        if(qs.isEmpty())
            return false;

        wgtlist.clear();
        proj->loadFromFile(true, qs);

        addImageWidgetsFromProject();

        return true;
    }

    bool imupWin::newProject()
    {
        if(proj->objects().size() > 0 && proj->projectFilePath() == unsaved_proj_path)
        {
            if(QMessageBox::question(this, tr("Save unsaved?"), tr("There are unsaved changes. Do you wish to save them before clearing project?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                if(saveProject() == false)
                    return false;
            }
        }

        foreach(QWidget *qw, wgtlist.values())
        {
            qw->hide();
            qw->deleteLater();
        }

        wgtlist.clear();
        proj->deleteLater();

        proj = new UploadProject(this);
        proj->setProjectFilePath(unsaved_proj_path);

        return true;
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

    void imupWin::closeEvent(QCloseEvent *event)
    {
        if(proj->objects().size() > 0 && proj->projectFilePath() == unsaved_proj_path)
        {
            if(QMessageBox::question(this, tr("Save project"), tr("Do you wish to save this project under some fancy name, "
                                                               "so you'll be able to return to it later?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                if(saveProject() == false)
                    event->ignore();
            }
            else
            {
                if(QMessageBox::warning(this, tr("Save project"), tr("Really quit?<br><br>All your work will be <strong>L O S T</strong>!"), QMessageBox::No|QMessageBox::Yes) == QMessageBox::No)
                {
                    event->ignore();
                    return;
                }
                else
                {
                    proj->deleteLater();
                    QFile::remove(unsaved_proj_path);
                }
            }
        }

        event->accept();
    }

    void imupWin::makeToolbarButtons()
    {
        ui->mainToolBar->addAction(ui->action_Quit);
        ui->projectToolBar->addAction(ui->actionNew_project);
        ui->projectToolBar->addAction(ui->action_Open_project);
        ui->projectToolBar->addAction(ui->menu_Save_project->menuAction());
    }

    bool imupWin::event(QEvent *e)
    {
        if(e->type() == QEvent::User)
        {
            CommonsImgWidget::CommonsImgWidgetEvent * evt= dynamic_cast<CommonsImgWidget::CommonsImgWidgetEvent*>(e);
            if(evt)
            {
                if(evt->customType() == CommonsImgWidget::CommonsImgWidgetEvent::DeleteRequested && evt->senderWgt() != 0)
                {
                    CommonsImgWidget *rmwgt = 0;
                    if(evt->senderWgt()->getImgObj() != 0 && evt->senderWgt()->getImgObj()->uuid().isNull() == false
                            && (rmwgt = wgtlist.value(evt->senderWgt()->getImgObj()->uuid(),0) ) != 0)
                    {
                        removeObject(rmwgt);
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void imupWin::addImageWidgetsFromProject()
    {
        const QList<CommonsImgObject*> &objs = proj->objects();

        foreach(CommonsImgObject *obj, objs)
        {
            if(wgtlist.contains(obj->uuid()) == false)
            {
                CommonsImgWidget *wgt = new CommonsImgWidget(obj, this);
                ui->ScrollLay->addWidget(wgt);
                wgtlist.insert(obj->uuid(), wgt);
            }
        }
    }

    void imupWin::removeObject(CommonsImgWidget *imwgt)
    {
        if(imwgt == 0)
            return;

        proj->removeCommonsImgObj(imwgt->getImgObj());

        imwgt->hide();

        wgtlist.remove(imwgt->getImgObj()->uuid());
        imwgt->deleteLater();

    }

    void imupWin::connects()
    {
        connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(saveProject()));
        connect(ui->action_Open_project, SIGNAL(triggered()), this, SLOT(loadProject()));
        connect(ui->actionNew_project, SIGNAL(triggered()), this, SLOT(newProject()));

        connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    }

    void imupWin::lockIt(bool unl)
    {
        QDir dloc(QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        QScopedPointer<QSettings> lf(new QSettings(dloc.absoluteFilePath("LOCK.IT"), QSettings::IniFormat));

        if(lf->allKeys().size() > 0)
        {
            int pid = lf->value("lock/pid", 0).toInt();

            errno = 0;
            if(pid > 0 && kill(pid, 0) == 0 && pid != QApplication::applicationPid())
            {
                if(!unl)
                {
                    if(QMessageBox::warning(this, tr("Warning"), tr("<strong>%1</strong> seems to be already running!<br><br>"
                                                                    "Ignore and run anyway (can cause problems)?").arg(QApplication::applicationName()), QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
                    {
                        close();
                    }
                }
                else
                {
                    lf.reset(0);
                    QFile::remove(lf->fileName());
                }
            }
        }

        if(!unl)
            lf->setValue("lock/pid", QApplication::applicationPid());

    }
}
