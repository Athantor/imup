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

#ifndef COMMONSIMGOBJECT_H
#define COMMONSIMGOBJECT_H

#include <tuple>

#include <QObject>
#include <QDateTime>
#include <QSet>

#include "imagefile.h"

namespace imup
{
    class CommonsImgObject : public QObject
    {
            Q_OBJECT
        public:
            typedef std::tuple<double, double, double, double> Geo_t;
            typedef QSet<QString> CCats_t;

            explicit CommonsImgObject(const QString &, QObject *parent = 0);
            ~CommonsImgObject();

            virtual const ImageFile* imageFile() const;
            virtual ImageFile* imageFile();

            virtual const QString& cmsFilename() const;
            virtual const QString& cmsDescription() const;
            virtual const QString& cmsLicense() const;
            virtual const QString& cmsAuthor() const;
            virtual const QDateTime& cmsDateTime() const;
            virtual const Geo_t& cmsGeo() const;
            virtual Geo_t& cmsGeo();

            virtual const CCats_t& cmsCats() const;
            virtual CCats_t& cmsCats();

            virtual bool isValid();

        public slots:
            virtual void setCmsGeo(double, double, double = NAN, double = NAN);
            virtual void setCmsDateTime(const QDateTime&);
            virtual void setCmsAuthor(const QString&);
            virtual void setCmsLicense(const QString&);
            virtual void setCmsDescription(const QString&);
            virtual void setCmsFilename(const QString&);

        protected:
          ImageFile* img_file;
          QString cms_filename;
          QString cms_desc;
          QString cms_license;
          QString cms_author;
          QDateTime cms_file_dt;
          Geo_t cms_geo;
          CCats_t cms_cats;

          virtual void fillFromMetaData();
          virtual double calcLonLat(quint8 mode);

        public slots:
        signals:

    };
}

#endif // COMMONSIMGOBJECT_H
