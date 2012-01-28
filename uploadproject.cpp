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

#include <QTextCodec>
#include <QStringList>

#include "uploadproject.h"

namespace imup
{
    UploadProject::UploadProject(QObject *parent) :
        QObject(parent)
    {
    }

    UploadProject::~UploadProject()
    {
        clearObjs();
    }

    const QList<CommonsImgObject*>& UploadProject::objects() const
    {
        return objs;
    }

    void UploadProject::setProjectFilePath(const QString &ppath)
    {
        proj_path = ppath;
    }

    const QString &UploadProject::projectFilePath() const
    {
        return proj_path;
    }

    void UploadProject::addCommonsImgObj(CommonsImgObject *obj, bool write)
    {
        if(known_objs.contains(obj->uuid()))
            return;

        objs.append(obj);

        if(write)
            saveToFile(obj);
    }

    CommonsImgObject *UploadProject::addCommonsImgObj(const QString &path)
    {
        CommonsImgObject *iobj = new CommonsImgObject(path, this);
        iobj->setUuid();

        addCommonsImgObj(iobj, true);
        return iobj;
    }

    void UploadProject::loadFromFile(bool clear, const QString & whe)
    {
        if(whe.isEmpty() == false)
            proj_path = whe;

        QScopedPointer<QSettings> proj_setts(new QSettings(proj_path, QSettings::IniFormat));

        if(proj_setts->status() != QSettings::NoError)
        {
            QString err;
            switch(proj_setts->status())
            {
                case QSettings::AccessError:
                    err = tr("Can't acces project file");
                    break;
                case QSettings::FormatError:
                    err = tr("Project file has an invalid format.");
                    break;
                default:
                    err = tr("Unknown or no error");
                    break;

            }

            throw UploadProjectError(tr("Can't load settings file '%1': %2").arg(proj_path).arg(err));
        }

        if(clear)
            clearObjs();

        const quint64 files = proj_setts->value("meta/file_count", 0).toULongLong();

        for(quint64 i = 0; i < files; ++i)
        {
            const QString &grpn = QString("file_%1").arg(i+1);

            if(proj_setts->contains(grpn + "/file_path") == false)
                continue;


            proj_setts->beginGroup(grpn);
            CommonsImgObject * imob = new CommonsImgObject(proj_setts->value("file_path").toString(), this);

            imob->setCmsFilename(proj_setts->value("cms_filename").toString());
            imob->setCmsAuthor(proj_setts->value("cms_author").toString());
            imob->setFileDateTime(proj_setts->value("file_datetime").toDateTime());
            imob->setCmsDateTime(proj_setts->value("cms_datetime").toString());
            imob->setCmsDescription(proj_setts->value("cms_description").toString());
            imob->setCmsLicense(proj_setts->value("cms_license").toString());
            imob->setCmsLicense(proj_setts->value("cms_geo").toString());

            imob->setUuid(QUuid(proj_setts->value("file_uuid").toString()));
            if(imob->uuid().isNull())
            {
                imob->setUuid();
                proj_setts->setValue("file_uuid", imob->uuid().toString());
            }

            const QVariantList& fg = proj_setts->value("file_geo").toList();
            imob->setFileGeo(fg[0].toDouble(), fg[1].toDouble(), fg.size() > 2 ? fg[2].toDouble() : NAN, fg.size()>3 ?fg[3].toDouble() : NAN);

            const QStringList &cats = proj_setts->value("cms_cats").toStringList();
            foreach(const QString& cc, cats)
                imob->cmsCats() << cc;

            addCommonsImgObj(imob, false);

            proj_setts->endGroup();
        }
    }

    void UploadProject::saveToFile(CommonsImgObject * ob, const QString & whe)
    {
        if(whe.isEmpty() == false)
            proj_path = whe;

        QSharedPointer<QSettings> proj_setts(new QSettings(proj_path, QSettings::IniFormat));

        if(!checkDestFilePath(proj_path))
            throw UploadProjectError(tr("Error writing project to file '%1'").arg(proj_path));

        if(ob == 0)
        {
            proj_setts->clear();
            proj_setts->sync();

            quint64 ctr =  0;
            foreach(CommonsImgObject *imob, objs)
            {
                if(imob->uuid().isNull())
                    imob->setUuid();

                proj_setts->beginGroup(QString("file_%1").arg(++ctr));
                saveObjToFile(proj_setts, imob);
                proj_setts->endGroup();
                proj_setts -> setValue("meta/file_count", QVariant::fromValue<quint64>(objs.size()));
            }
        }
        else
        {
            if(ob->uuid().isNull())
                ob->setUuid();
            saveObjToFile(proj_setts, ob);
        }

        proj_setts->sync();
    }

    void UploadProject::clearObjs()
    {
        foreach(CommonsImgObject *o, objs)
            o->deleteLater();

        objs.clear();
    }

    void UploadProject::saveObjToFile(QSharedPointer<QSettings> proj_setts, const CommonsImgObject *imob)
    {
        bool endgrp = false;
        if((endgrp = proj_setts->group().isEmpty())) //auto append
        {
            proj_setts->beginGroup(QString("file_%1").arg(objs.size()));
        }

        qDebug() << proj_setts->group();

        proj_setts->setValue("file_path", imob->imageFile()->getFileInfo().absoluteFilePath());
        proj_setts->setValue("file_uuid", imob->uuid().toString());

        proj_setts->setValue("cms_filename", imob->cmsFilename());
        proj_setts->setValue("cms_author", imob->cmsAuthor());
        proj_setts->setValue("cms_cats", QStringList(imob->cmsCats().toList()));
        proj_setts->setValue("cms_datetime", imob->cmsDateTime());
        proj_setts->setValue("cms_filedatetime", imob->fileDateTime());
        proj_setts->setValue("cms_description", imob->cmsDescription());
        proj_setts->setValue("cms_geo", imob->cmsGeo());
        proj_setts->setValue("cms_license", imob->cmsLicense());

        QVariantList geos;
        geos << std::get<0>(imob->fileGeo()) << std::get<1>(imob->fileGeo())<< std::get<2>(imob->fileGeo()) << std::get<3>(imob->fileGeo());
        proj_setts->setValue("file_geo", geos);

        if(endgrp)
        {
            proj_setts->endGroup();
            proj_setts -> setValue("meta/file_count", QVariant::fromValue<quint64>(objs.size()));
        }
    }

    bool UploadProject::checkDestFilePath(const QString & fp)
    {
        QFileInfo fi(fp);
        if(fi.exists() == false)
        {
            QFile f(fp);
            bool fok = f.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text);
            if(fok)
                f.close();
            return fok;
        }
        else
        {
            return fi.isWritable();
        }

        return false;
    }
}
