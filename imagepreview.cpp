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
#include <QTransform>

#include "imagepreview.h"

namespace imup
{
    ImagePreview::ImagePreview(const CommonsImgObject *obj, QWidget *parent) :
        QDialog(parent), ui(new Ui::ImagePreview), the_obj(obj), tbar(new QToolBar()),
        tmr(new QTimer(this))
    {
        ui->setupUi(this);
        ui->verticalLayout->insertWidget(0, tbar);

        ImgView = new QLabel();
        ui->scrollArea->setWidget(ImgView);

        ImgView->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        setWindowFlags(Qt::Dialog);

        makeActions();

        if(the_obj)
        {
            org_pxm = the_obj->imageFile()->getFileInfo().absoluteFilePath();
            performScaling();

            setWindowTitle(windowTitle() + tr(" — „%1”").arg(the_obj->imageFile()->getFileInfo().absoluteFilePath()) );
        }

        tmr->setInterval(40);
        tmr->setSingleShot(true);
        connect(tmr, SIGNAL(timeout()), this, SLOT(performScaling()));
    }

    ImagePreview::~ImagePreview()
    {
        delete ui;

        tmr->stop();
        tmr->deleteLater();

        ImgView = 0;
    }

    void ImagePreview::makeActions()
    {
        act_toggle_fit = new QAction(QIcon(), tr("Fit to window"), this);
        act_rotate = new QAction(QIcon(), tr("Rotate"), this);

        act_toggle_fit->setCheckable(true);
        act_toggle_fit->setChecked(true);

        connect(act_toggle_fit, SIGNAL(toggled(bool)), this, SLOT(toggleScaling(bool)));
        connect(act_rotate, SIGNAL(triggered()), this, SLOT(rotate()));

        tbar->addAction(act_toggle_fit);
        tbar->addAction(act_rotate);
    }

    void ImagePreview::changeEvent(QEvent *e)
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

    void ImagePreview::resizeEvent(QResizeEvent *e)
    {
        scaleImage();
        QDialog::resizeEvent(e);
    }

    void ImagePreview::toggleScaling(bool)
    {
        scaleImage();
    }

    void ImagePreview::scaleImage()
    {
        if(act_toggle_fit->isChecked())
        {
            tmr->start();
        }
        else
        {
            setCursor(Qt::BusyCursor);

            tmr->stop();
            if(ImgView->pixmap()->size() != org_pxm.size() )
            {
                ImgView->setPixmap(org_pxm);
            }

            unsetCursor();
        }
    }

    void ImagePreview::performScaling()
    {
        setCursor(Qt::BusyCursor);

        QPixmap pxm(org_pxm);
        QSize sz = ui->scrollArea->size() - QSize(8, 8);

        pxm = pxm.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        ImgView->setPixmap(pxm);
        ImgView->resize(sz);

        unsetCursor();
    }

    void ImagePreview::rotate()
    {
        setCursor(Qt::BusyCursor);

        org_pxm = org_pxm.transformed(QTransform().rotate(-90.0), Qt::SmoothTransformation);
        scaleImage();

        unsetCursor();
    }

} //ns
