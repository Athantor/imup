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

#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <stdexcept>

#include <QObject>
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QVariant>

#include <exiv2/image.hpp>

namespace imup {


    class ImageFile : public QObject
    {
            Q_OBJECT
        public:

            class ImageFileError : public std::runtime_error
            {
                public: ImageFileError(const QString& msg) : std::runtime_error(msg.toStdString()) {}
            };

            enum MetadataType
            {
                MDT_EXIF,
                MDT_IPTC,
                MDT_XMP,
                MDT_Invalid
            };

            explicit ImageFile(const QString &, QObject *parent = 0);
            ~ImageFile();

            static const QSize PreviewSize;

            virtual const QFileInfo& getFileInfo() const;

            virtual QString getMimeType() const;
            virtual const QImage& getPreviewIamge() const;

            virtual const Exiv2::Image::AutoPtr& getImageMetaData() const;
            virtual QVariantList& getMetaData(const QString &, QVariantList&, MetadataType = MDT_EXIF);
            virtual QString& getMetaData(const QString&, QString&, MetadataType = MDT_EXIF);

            static bool convertMetaData(const Exiv2::Value&, quint32 , QVariant&);

        protected:
            const QString filePath;
            const QFileInfo fileInfo;
            bool canDisplay;
            Exiv2::Image::AutoPtr imgMetaData;
            QImage previewImg;

            virtual bool loadPreviewImg();
            virtual bool creatPreviewImg();
            virtual const Exiv2::Metadatum* selectFromMetadatum(const QString&, MetadataType) const ;

        public slots:
        signals:

    };

}

#endif // IMAGEFILE_H
