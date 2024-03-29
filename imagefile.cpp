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

#include <iterator>

#include <QDebug>
#include <QImageReader>
#include <QSet>
#include <QDate>
#include <QTime>
#include <QVector2D>

#include <exiv2/preview.hpp>

#include "imagefile.h"


namespace imup {

    const QSize ImageFile::PreviewSize(160, 120);

    ImageFile::ImageFile(const QString & fpath, QObject *parent) :
        QObject(parent), filePath(fpath), fileInfo(QFileInfo(fpath)), canDisplay(QImageReader(filePath).canRead())
    {
        if(fileInfo.exists() == false || fileInfo.isReadable() == false)
            throw ImageFileError(tr("Accessing „%1” failed: %2").arg(filePath).
                                 arg(fileInfo.exists() ? tr("File's not readable") : tr("File doesn't exists")));

        imgMetaData = Exiv2::ImageFactory::open(filePath.toStdString());
        if(imgMetaData.get() == 0 || imgMetaData->good() == false)
            throw ImageFileError(tr("Can't read file metadata: %1").arg(filePath));

        imgMetaData->readMetadata();
        if(canDisplay && !loadPreviewImg())
            creatPreviewImg();
    }

    ImageFile::~ImageFile()
    {
    }

    bool ImageFile::loadPreviewImg()
    {
        if(!canDisplay || imgMetaData.get() == 0)
            return false;

        Exiv2::PreviewManager pvMngr(*imgMetaData);
        Exiv2::PreviewPropertiesList pvpList(pvMngr.getPreviewProperties());

        if(pvpList.size() < 1)
            return false;

        Exiv2::PreviewPropertiesList::iterator pvpItr = pvpList.end();
        std::advance(pvpItr, -1);

        Exiv2::PreviewImage pvImg(pvMngr.getPreviewImage(*pvpItr));
        if(previewImg.loadFromData(reinterpret_cast<const uchar*>(pvImg.pData()), pvImg.size()) != true
                || qMax(previewImg.rect().width(), previewImg.rect().height()) < PreviewSize.width())
        {
            return false;
        }

        return true;
    }

    bool ImageFile::creatPreviewImg()
    {
        QImage tmpi;

        if(!canDisplay || !tmpi.load(filePath))
            return false;

        QSize qs(PreviewSize);
        if(tmpi.width() < tmpi.height())
            qs = QSize(PreviewSize.height(), PreviewSize.height());

        previewImg = tmpi.scaled(qs, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return true;
    }


    const QFileInfo &ImageFile::getFileInfo() const
    {
        return fileInfo;
    }

    QString ImageFile::getMimeType() const
    {
        return QString::fromStdString(imgMetaData->mimeType());
    }

    const QImage& ImageFile::getPreviewIamge() const
    {
        return previewImg;
    }

    const Exiv2::Image::AutoPtr &ImageFile::getImageMetaData() const
    {
        return imgMetaData;
    }

    const Exiv2::Metadatum *ImageFile::selectFromMetadatum(const QString &mdTag, MetadataType mdType) const
    {
        const Exiv2::Metadatum *md = 0;

        switch(mdType)
        {
            case MDT_EXIF:
                md = &imgMetaData->exifData()[mdTag.toStdString()];
                break;
            case MDT_IPTC:
                md = &imgMetaData->iptcData()[mdTag.toStdString()];
                break;
            case MDT_XMP:
                md = &imgMetaData->xmpData()[mdTag.toStdString()];
                break;
            case MDT_Invalid:
            default:
                md = 0;
                break;
        }

        return md;
    }

    QVariantList &ImageFile::getMetaData(const QString & mdTag, QVariantList& mdValue, ImageFile::MetadataType mdType) const
    {
        mdValue.clear();
        if(mdType == MDT_Invalid || mdTag.isEmpty())
            return mdValue;

        const Exiv2::Metadatum *md = selectFromMetadatum(mdTag, mdType);

        if(md && md->count() > 0)
        {
            const Exiv2::Value &val = md->value();
            quint32 cnt = 1;

            if(val.typeId() != Exiv2::asciiString && val.typeId() != Exiv2::string && val.typeId() != Exiv2::xmpText)
                cnt = md->count();

            for(quint32 i = 0; i < cnt; ++i)
            {
                mdValue << QVariant();
                convertMetaData(val, i, mdValue.last());
            }

        }

        return mdValue;
    }

    QString &ImageFile::getMetaData(const QString & mdTag, QString & mdVal, ImageFile::MetadataType mdType) const
    {
        const Exiv2::Metadatum *md = selectFromMetadatum(mdTag, mdType);

        mdVal.clear();
        if(md)
            mdVal = QString::fromStdString(md->toString());

        return mdVal;
    }

    template<typename DatumT>
    void ImageFile::Fill_map(const DatumT& datum, TagList_t & ret_m) const
    {
        ret_m.clear();
        for(typename DatumT::const_iterator it = datum.begin(); it != datum.end(); ++it)
        {
            int count = it->count();
            QString key = QString::fromStdString(it->key());

            if(it->typeId() == Exiv2::asciiString)
                count =1;

            if(Q_LIKELY(count > 0))
            {
                QString mystr = "";
                QVariantList qvl;

                for(int cnt = 0; cnt < count; ++cnt)
                {
                    QVariant md;
                    convertMetaData(it->value(), cnt, md);

                    if(it->typeId() == Exiv2::asciiString || it->typeId() ==Exiv2::string || it->typeId() == Exiv2::xmpText)
                        mystr = md.toString();
                    else if(md.type() == QVariant::Vector2D)
                        qvl << md;
                    else
                        mystr += (mystr.isEmpty() ? "" : " ") + md.toString();
                }

                if(qvl.isEmpty() == false)
                    mystr = QString::number(vector2dbl(qvl), 'g', 8);

                if(mystr.isEmpty())
                    mystr = QString::fromStdString(it->value().toString());

                ret_m.insertMulti(key, mystr);

            }
            else
            {
                //ret_m.insertMulti(key, "-");
            }
        }
    }

    ImageFile::TagList_t ImageFile::getMetaData(ImageFile::MetadataType mdType) const
    {
        ImageFile::TagList_t retmap;

        switch(mdType)
        {
            case MDT_EXIF:
                Fill_map<Exiv2::ExifData>(imgMetaData->exifData(), retmap);
                break;
            case MDT_IPTC:
                Fill_map<Exiv2::IptcData>(imgMetaData->iptcData(), retmap);
                break;
            case MDT_XMP:
                Fill_map<Exiv2::XmpData>(imgMetaData->xmpData(), retmap);
                break;
            case MDT_Invalid:
            default:
                qWarning() << tr("%1: Invalid metadata type!").arg(__PRETTY_FUNCTION__);
                retmap.clear();
        }

        return retmap;
    }

    bool ImageFile::convertMetaData(const Exiv2::Value &val, quint32 n, QVariant & dst)
    {
        static QSet<Exiv2::TypeId> toInt, toRat, toDouble, toString;
        const Exiv2::TypeId tid = val.typeId();

        if(toInt.isEmpty())
        {
            toInt << Exiv2::unsignedByte <<Exiv2::unsignedShort << Exiv2::unsignedLong << Exiv2::signedByte << Exiv2::signedShort << Exiv2::signedLong;
            toRat << Exiv2::signedRational << Exiv2::unsignedRational;
            toDouble << Exiv2::tiffFloat << Exiv2::tiffDouble;
            toString << Exiv2::asciiString << Exiv2::string << Exiv2::comment << Exiv2::xmpText;
        }

        if(toInt.contains(tid))
        {
            dst = QVariant::fromValue<long long>(val.toLong(n));
        }
        else if(toRat.contains(tid))
        {
            Exiv2::Rational r = val.toRational(n);
            dst = QVariant(QVector2D(r.first, r.second));
        }
        else if(toDouble.contains(tid))
        {
            dst = QVariant(val.toFloat(n));
        }
        else if(toString.contains(tid))
        {
            dst = QVariant(QString::fromStdString(val.toString(n)));
        }
        else if(tid == Exiv2::date)
        {
            const Exiv2::DateValue &dval = dynamic_cast<const Exiv2::DateValue&>(val);
            const Exiv2::DateValue::Date &dv = dval.getDate();

            dst = QVariant(QDate(dv.year, dv.month, dv.day));
        }
        else if(tid == Exiv2::time)
        {
            const Exiv2::TimeValue &tval = dynamic_cast<const Exiv2::TimeValue&>(val);
            const Exiv2::TimeValue::Time &tv = tval.getTime();

            dst = QVariant(QTime(tv.hour, tv.minute, tv.second));
        }
        else
        {
            dst = QVariant(QString::fromStdString(val.toString(n)));
        }

        return true;
    }

    double ImageFile::vector2dbl(const QVariantList &qvl)
    {
        double ret = NAN;
        if(qvl.size() == 3)
        {
            const QVector2D& deg = qvl.at(0).value<QVector2D>(), min = qvl.at(1).value<QVector2D>(), sec = qvl.at(2).value<QVector2D>();

            if(sec.x() == 0 && sec.y() == 1)
                ret = deg.x() + ((min.x() / (min.y())) / 60.);
            else
                ret = deg.x() + (min.x() / 60.) + (sec.x() / 3600.);

        }
        else if(qvl.size() == 1)
        {
            const QVector2D& deg = qvl.at(0).value<QVector2D>();
            if(deg.y() != 0)
                ret = deg.x() / deg.y();
        }

        return ret;
    }
}
