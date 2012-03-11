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
#include <QMessageBox>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

#include "imupwin.h"
#include "imagepreview.h"
#include "metainfodialog.h"

#include "commonsimgwidget.h"
#include "ui_commonsimgwidget.h"

namespace imup
{

    CommonsImgWidget::CommonsImgWidget(CommonsImgObject *imob, QWidget *parent)
        : QWidget(parent), img_obj(imob), ui(new Ui::CommonsImgWidget)
    {
        makeActions();
        commonSetup();
    }

    CommonsImgWidget::CommonsImgWidget(const QString & filepath, QWidget *parent) :
        QWidget(parent), img_obj(new CommonsImgObject(filepath, this)), ui(new Ui::CommonsImgWidget)
    {
        makeActions();
        commonSetup();
    }


    void CommonsImgWidget::makeActions()
    {
        act_exif_date = new QAction(QIcon(), tr("Get date from &metadata"), this);
        act_cal_date = new QAction(QIcon(), tr("Get date from &calendar"), this);
        act_exif_geo = new QAction(QIcon(), tr("&Get geo from metadata"), this);
        act_show_geo_on_map = new QAction(QIcon(), tr("Show geo on &map"), this);
    }

    void CommonsImgWidget::commonSetup()
    {
        ui->setupUi(this);
        setupFields();

        calwgt.reset(new QCalendarWidget(0));
        calwgt->setMaximumDate(QDate::currentDate());

        connect(calwgt.data(), SIGNAL(activated(QDate)), this, SLOT(setDateFromCal(QDate)));

        connect(ui->closeBtn, SIGNAL(clicked()), this, SLOT(requestDeletion()));

        ui->DtBtn->setMenu(new QMenu());
        ui->DtBtn->menu()->addAction(act_exif_date);
        ui->DtBtn->menu()->addAction(act_cal_date);

        connect(act_cal_date, SIGNAL(triggered()), this, SLOT(showCal()));
        connect(act_exif_date, SIGNAL(triggered()), this, SLOT(setDateFromMetadata()));

        ui->GeoBtn->setMenu(new QMenu(tr("G")));
        ui->GeoBtn->menu()->addAction(act_show_geo_on_map);
        ui->GeoBtn->menu()->addAction(act_exif_geo);

        if(std::isnan(std::get<0>( img_obj->fileGeo())))
            act_exif_geo->setEnabled(false);

        connect(act_exif_geo, SIGNAL(triggered()), this, SLOT(setGeoFromMetadata()));
        connect(act_show_geo_on_map, SIGNAL(triggered()), this, SLOT(showGeoOnMap()));
    }

    CommonsImgWidget::~CommonsImgWidget()
    {
        delete ui;
    }

    CommonsImgObject *CommonsImgWidget::getImgObj()
    {
        return img_obj;
    }

    const CommonsImgObject *CommonsImgWidget::getImgObj() const
    {
        return img_obj;
    }

    void CommonsImgWidget::changeEvent(QEvent *e)
    {
        QWidget::changeEvent(e);
        switch (e->type()) {
            case QEvent::LanguageChange:
                ui->retranslateUi(this);
                break;
            default:
                break;
        }
    }

    void CommonsImgWidget::setupFields()
    {
        if(img_obj->isValid() == false)
            return;

        const ImageFile * imf = img_obj->imageFile();

        connect(ui->UploadFilenameEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsFilename(QString)));
        connect(ui->FileAuthorEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsAuthor(QString)));
        connect(ui->FileSourceEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsFileSource(QString)));
        connect(ui->FileDtEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsDateTime(QString)));
        connect(ui->GeoEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsGeo(QString)));
        connect(ui->FileLicenseCbx, SIGNAL(editTextChanged(QString)), img_obj, SLOT(setCmsLicense(QString)));
        connect(ui->CatsEdit, SIGNAL(textChanged(QString)), img_obj, SLOT(setCmsCats(QString)));

        connect(ui->infoBtn, SIGNAL(clicked()), this, SLOT(showMetaInfo()));
        connect(ui->previewBtn, SIGNAL(clicked()), this, SLOT(showImagePreview()));

        //----

        ui->MainGrpBox->setTitle(imf->getFileInfo().canonicalFilePath());

        //----

        ui->UploadFilenameEdit->setText(imf->getFileInfo().fileName());

        //----

        ui->ThumbLbl->setPixmap(QPixmap::fromImage(imf->getPreviewIamge()));
        ui->ThumbLbl->setToolTip(makeThumbTooltipText());

        //----

        ui->FileSourceEdit->setText(tr("{{own}}"));

        //----

        ui->FileAuthorEdit->setText(img_obj->cmsAuthor());

        //----

        ui->FileDtEdit->setText(img_obj->cmsDateTime());

        //----

        ui->FileDescTxtEdit->setPlainText(img_obj->cmsDescription());

        //----

        ui->GeoEdit->setText( img_obj-> cmsGeo() );

        //----

        int lic_idx = -1;
        if(img_obj->cmsLicense().isEmpty() == false)
        {
            if((lic_idx = ui->FileLicenseCbx->findText(img_obj->cmsLicense())) > -1 )
            {
                ui->FileLicenseCbx->setCurrentIndex(lic_idx);
            }
            else
            {
                //ui->FileLicenseCbx->setCurrentIndex(-1);
                ui->FileLicenseCbx->setEditText(img_obj->cmsLicense());
            }
        }
        else
        {
            ui->FileLicenseCbx->setCurrentIndex(0);
        }

        //----

        ui->CatsEdit->setText(img_obj->cmsCats());
    }

    QString CommonsImgWidget::makeThumbTooltipText()
    {
        const ImageFile * imf = img_obj->imageFile();

        return tr("<ul><li>File name: <b>%1</b></li>"
                  "<li>File size: <b>%2MiB</b></li>"
                  "<li>File time: <b>%3</b></li>"
                  "<li>Image dimensions: <b>%4px×%5px</b> (%6Mpx)</li>"
                  "</ul>").arg(imf->getFileInfo().fileName())
                .arg(imf->getFileInfo().size() / 1048576., 0, 'g', 3)
                .arg(imf->getFileInfo().lastModified().toString())
                .arg(imf->getImageMetaData()->pixelWidth()).arg(imf->getImageMetaData()->pixelHeight())
                .arg(imf->getImageMetaData()->pixelWidth() * imf->getImageMetaData()->pixelHeight() / 1e6, 0, 'g', 3);
    }

    void CommonsImgWidget::requestDeletion()
    {
        if(QMessageBox::question(this, tr("Remove?"), tr("Really remove „<strong>%1</strong>”?<br><br>You won't be able to undo this!")
                                 .arg(img_obj->imageFile()->getFileInfo().canonicalFilePath()), QMessageBox::No| QMessageBox::Yes) == QMessageBox::No)
        {
            return;
        }

        CommonsImgWidgetEvent *evt =new CommonsImgWidgetEvent(CommonsImgWidgetEvent::DeleteRequested, this);
        QApplication::postEvent(imupWin::instance(), evt);
    }

    void CommonsImgWidget::showCal()
    {
        calwgt->setWindowTitle(tr("Select date…"));
        calwgt->setWindowModality(Qt::WindowModal);
        calwgt->move(mapToGlobal(ui->FileDtEdit->geometry().center()));

        QString hlp = ui->FileDtEdit->text().trimmed();
        QDate dtxt;
        if((dtxt = QDate::fromString(hlp, Qt::TextDate)).isValid() || (dtxt = QDate::fromString(hlp, Qt::SystemLocaleDate)).isValid() || (dtxt = QDate::fromString(hlp, Qt::ISODate)).isValid())
            calwgt->setSelectedDate(dtxt);

        calwgt->showSelectedDate();
        calwgt->show();
    }

    void CommonsImgWidget::setDateFromCal(const QDate &d)
    {
        calwgt->hide();
        ui->FileDtEdit->setText(d.toString("yyyy-MM-dd"));
    }

    void CommonsImgWidget::setDateFromMetadata()
    {
        img_obj->fillCmsDtFromMetadata();
        ui->FileDtEdit->setText(img_obj->cmsDateTime());
    }

    void CommonsImgWidget::setGeoFromMetadata()
    {
        img_obj->fillCmsGeoFromMetadata();
        ui->GeoEdit->setText(img_obj->cmsGeo());
    }

    void CommonsImgWidget::showGeoOnMap()
    {
        QString geos, qgeo;

        if(ui->GeoEdit->text().isEmpty())
        {
            auto geo = img_obj->fileGeo();

            if(img_obj->cmsGeo().isEmpty() == false)
                geos = img_obj->cmsGeo();
            else if(!std::isnan(std::get<0>(geo)))
                qgeo = QString("?q=%1,%2").arg(std::get<0>(geo) , 0, 'g', 8).arg(std::get<1>(geo), 0, 'g', 8);
        }
        else
            geos = ui->GeoEdit->text();

        if(qgeo.isEmpty() && geos.isEmpty() == false)
        {
            QRegExp rx("[+-]?((180|1[0-7][0-9])|[0-9]{1,2})\\.[0-9]+");

            if(geos.count(rx)>=2)
            {
                int pos = rx.indexIn(geos);
                qgeo = rx.cap();
                rx.indexIn(geos, pos+qgeo.size());
                QString ll = qgeo + "," + rx.cap(0);

                qgeo = "?q=" + ll + "&ll=" + ll + "&t=h&mrt=loc&z=15";
            }
        }

        QDesktopServices::openUrl(QUrl("https://maps.google.com/"+qgeo));
    }

    void CommonsImgWidget::showMetaInfo()
    {
        QScopedPointer<MetainfoDialog> dlg(new MetainfoDialog(*getImgObj()->imageFile()));
        dlg->exec();
    }

    void CommonsImgWidget::showImagePreview()
    {
        ImagePreview* prv = new ImagePreview(img_obj, this);
        prv->exec();
        prv->deleteLater();
    }

    void CommonsImgWidget::on_FileDescTxtEdit_textChanged()
    {
        img_obj->setCmsDescription(ui->FileDescTxtEdit->toPlainText());
    }

    void CommonsImgWidget::on_CatsEdit_textEdited(const QString &arg1)
    {
        qDebug() << __PRETTY_FUNCTION__ << arg1 ;
    }

}




