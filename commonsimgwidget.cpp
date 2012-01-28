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

#include "commonsimgwidget.h"
#include "ui_commonsimgwidget.h"

namespace imup
{

    CommonsImgWidget::CommonsImgWidget(CommonsImgObject *imob, QWidget *parent)
        : QWidget(parent), img_obj(imob), ui(new Ui::CommonsImgWidget)
    {
        commonSetup();
    }

    CommonsImgWidget::CommonsImgWidget(const QString & filepath, QWidget *parent) :
        QWidget(parent), img_obj(new CommonsImgObject(filepath, this)), ui(new Ui::CommonsImgWidget)
    {
        commonSetup();
    }

    void CommonsImgWidget::commonSetup()
    {
        ui->setupUi(this);
        setupFields();
    }

    CommonsImgWidget::~CommonsImgWidget()
    {
        delete ui;
    }

    CommonsImgObject *CommonsImgWidget::getImgObj()
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

        ui->MainGrpBox->setTitle(imf->getFileInfo().canonicalFilePath());

        ui->UploadFilenameEdit->setText(imf->getFileInfo().fileName());

        ui->ThumbLbl->setPixmap(QPixmap::fromImage(imf->getPreviewIamge()));
        ui->ThumbLbl->setToolTip(makeThumbTooltipText());

        ui->FileSourceEdit->setText(tr("{{own}}"));
        ui->FileAuthorEdit->setText(img_obj->cmsAuthor());
        ui->FileDtEdit->setText(img_obj->cmsDateTime());

        //auto geo = img_obj->fileGeo();
        // QString("{{Location dec|%1|%2}}").arg(std::get<0>(geo) , 0, 'g', 8).arg(std::get<0>(geo), 0, 'g', 8)
        ui->GeoEdit->setText( img_obj-> cmsGeo() );
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
}
