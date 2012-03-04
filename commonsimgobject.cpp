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

    void CommonsImgObject::setUuid(const QUuid &uuid)
    {
        objUuid = uuid;
    }

    void CommonsImgObject::fillCmsFromMetadata()
    {
        fillCmsGeoFromMetadata();
        fillCmsDtFromMetadata();
    }

    void CommonsImgObject::fillCmsGeoFromMetadata()
    {
        QString geostr;

        qDebug() << __PRETTY_FUNCTION__ << std::get<0>(cms_file_geo);

        if(!std::isnan(std::get<0>(cms_file_geo)))
            geostr = QString("{{Location dec|%1|%2}}").arg(std::get<0>(cms_file_geo) , 0, 'g', 8).arg(std::get<1>(cms_file_geo), 0, 'g', 8);
        else
            return;

        setCmsGeo(geostr);
    }

    void CommonsImgObject::fillCmsDtFromMetadata()
    {
        setCmsDateTime(fileDateTime().toString("yyyy-MM-dd hh:mm:ss"));
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
        return cms_author;
    }

    const QString &CommonsImgObject::cmsFileSource() const
    {
        return cms_filesrc;
    }

    void CommonsImgObject::setCmsAuthor(const QString &cfa)
    {
        cms_author = cfa;
    }

    void CommonsImgObject::setCmsFileSource(const QString & fsrc)
    {
        cms_filesrc = fsrc;
    }

    const QString &CommonsImgObject::cmsDateTime() const
    {
        return cms_dt;
    }

    const QDateTime &CommonsImgObject::fileDateTime() const
    {
        return cms_file_dt;
    }

    void CommonsImgObject::setFileDateTime(const QDateTime &cfdt)
    {
        cms_file_dt = cfdt;
    }

    const QString &CommonsImgObject::cmsGeo() const
    {
        return cms_geo;
    }

    QString &CommonsImgObject::cmsGeo()
    {
        return cms_geo;
    }

    const CommonsImgObject::Geo_t &CommonsImgObject::fileGeo() const
    {
        return cms_file_geo;
    }

    void CommonsImgObject::setFileGeo(double lat, double lon, double alt, double dir)
    {
        cms_file_geo = Geo_t(lat, lon, alt, dir);

        if(qAbs(lat) > 90)
            std::get<0>(cms_file_geo) = NAN;
        if(qAbs(lon) > 180)
            std::get<1>(cms_file_geo) = NAN;
        if(dir < 0 || dir > 360)
            std::get<3>(cms_file_geo) = NAN;
    }

    void CommonsImgObject::setCmsDateTime(const QString & cdt)
    {
        cms_dt = cdt;
    }

    const CommonsImgObject::CCats_t &CommonsImgObject::cmsCats() const
    {
        return cms_cats;
    }

    CommonsImgObject::CCats_t &CommonsImgObject::cmsCats()
    {
        return cms_cats;
    }

    const QUuid &CommonsImgObject::uuid() const
    {
        return objUuid;
    }

    bool CommonsImgObject::isValid()
    {
        return imageFile() != 0 && imageFile()->getFileInfo().exists() && imageFile()->getFileInfo().isReadable();
    }

    void CommonsImgObject::setCmsGeo(const QString &cgeo)
    {
        cms_geo = cgeo;
    }

    void CommonsImgObject::fillFromMetaData()
    {
        QVariantList qvl;
        cms_filename = img_file->getFileInfo().fileName();

        //----
        cms_file_dt = img_file->getFileInfo().created();
        img_file->getMetaData("Exif.Photo.DateTimeOriginal", qvl);
        if(qvl.size() > 0)
            setFileDateTime(cms_file_dt = qvl.at(0).toDateTime());
        else //no exif, use file ctime
            setFileDateTime(cms_file_dt = img_file->getFileInfo().created());

        qvl.clear();

        //----

        img_file->getMetaData("Exif.Image.Artist", qvl);
        if(qvl.size() > 0)
            setCmsAuthor(qvl.at(0).toString());

        qvl.clear();

        //----

        setFileGeo(calcLonLat(0), calcLonLat(1), calcLonLat(2), calcLonLat(3));

        //----

        qDebug() << cms_file_dt <<cms_author << std::get<0>(fileGeo()) << std::get<1>(fileGeo()) << std::get<2>(fileGeo()) << std::get<3>(fileGeo());
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
