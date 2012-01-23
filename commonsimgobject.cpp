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

#include <QVector2D>

#include "commonsimgobject.h"

namespace imup
{
    CommonsImgObject::CommonsImgObject(const QString & fp, QObject *parent) :
        QObject(parent), img_file(new ImageFile(fp, this))
    {
        fillFromMetaData();
    }

    CommonsImgObject::~CommonsImgObject()
    {
    }

    const ImageFile *CommonsImgObject::imageFile() const
    {
        return img_file;
    }

    ImageFile *CommonsImgObject::imageFile()
    {
        return img_file;
    }

    const QString &CommonsImgObject::cmsFilename() const
    {
        return cms_filename;
    }

    void CommonsImgObject::setCmsFilename(const QString &cfn)
    {
        cms_filename = cfn;
    }

    const QString &CommonsImgObject::cmsDescription() const
    {
        return cms_desc;
    }

    void CommonsImgObject::setCmsDescription(const QString &cfd)
    {
        cms_desc = cfd;
    }

    const QString &CommonsImgObject::cmsLicense() const
    {
        return cms_license;
    }

    void CommonsImgObject::setCmsLicense(const QString & cfl)
    {
        cms_license = cfl;
    }

    const QString &CommonsImgObject::cmsAuthor() const
    {
        return cmsAuthor();
    }

    void CommonsImgObject::setCmsAuthor(const QString &cfa)
    {
        cms_author = cfa;
    }

    const QDateTime &CommonsImgObject::cmsDateTime() const
    {
        return cms_file_dt;
    }

    void CommonsImgObject::setCmsDateTime(const QDateTime &cfdt)
    {
        cms_file_dt = cfdt;
    }

    const CommonsImgObject::Geo_t &CommonsImgObject::cmsGeo() const
    {
        return cms_geo;
    }

    CommonsImgObject::Geo_t &CommonsImgObject::cmsGeo()
    {
        return cms_geo;
    }

    void CommonsImgObject::setCmsGeo(double lat, double lon, double alt, double dir)
    {
        cms_geo = Geo_t(lat, lon, alt, dir);

        if(qAbs(lat) > 90)
            std::get<0>(cms_geo) = NAN;
        if(qAbs(lon) > 180)
            std::get<1>(cms_geo) = NAN;
        if(dir < 0 || dir > 360)
            std::get<3>(cms_geo) = NAN;
    }

    const CommonsImgObject::CCats_t &CommonsImgObject::cmsCats() const
    {
        return cms_cats;
    }

    CommonsImgObject::CCats_t &CommonsImgObject::cmsCats()
    {
        return cms_cats;
    }

    bool CommonsImgObject::isValid()
    {
        return imageFile() != 0 && imageFile()->getFileInfo().exists() && imageFile()->getFileInfo().isReadable();
    }

    void CommonsImgObject::fillFromMetaData()
    {
        QVariantList qvl;
        cms_filename = img_file->getFileInfo().fileName();

        //----
        cms_file_dt = img_file->getFileInfo().created();
        img_file->getMetaData("Exif.Photo.DateTimeOriginal", qvl);
        if(qvl.size() > 0)
            setCmsDateTime(cms_file_dt = qvl.at(0).toDateTime());

        qvl.clear();

        //----

        img_file->getMetaData("Exif.Image.Artist", qvl);
        if(qvl.size() > 0)
            setCmsAuthor(qvl.at(0).toString());

        qvl.clear();

        //----

        setCmsGeo(calcLonLat(0), calcLonLat(1), calcLonLat(2), calcLonLat(3));

        //----

        qDebug() << cms_file_dt <<cms_author << std::get<0>(cmsGeo()) << std::get<1>(cmsGeo()) << std::get<2>(cmsGeo()) << std::get<3>(cmsGeo());
    }

    //mode: 0 lat; 1: lon; 2: alt; 3: dor
    double CommonsImgObject::calcLonLat(quint8 mode)
    {
        QString tag, tref, tref_r;

        switch(mode)
        {
            case 0:
                tag = "Exif.GPSInfo.GPSLatitude";
                tref = "Exif.GPSInfo.GPSLatitudeRef";
                tref_r = "S";
                break;
            case 1:
                tag = "Exif.GPSInfo.GPSLongitude";
                tref = "Exif.GPSInfo.GPSLongitudeRef";
                tref_r = "W";
                break;
            case 2:
                tag = "Exif.GPSInfo.GPSAltitude";
                tref = "Exif.GPSInfo.GPSAltitudeRef";
                tref_r = "1";
                break;
            case 3:
                tag = "Exif.GPSInfo.GPSImgDirection";
                break;
                //
            default:
                return NAN;
        }

        QVariantList qvl;
        double ret = NAN;

        img_file->getMetaData(tag, qvl);
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

        QString ref;
        if(isnan(ret) == 0 && tref_r.isEmpty() == false && tref.isEmpty() == false)
        {
            img_file->getMetaData(tref, ref);
            if(ref.toUpper().trimmed() == tref_r)
                ret = -ret;
        }

        return ret;
    }
}
