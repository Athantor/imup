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
#include <QApplication>
#include <QMessageBox>

#include "uploadproject.h"

namespace imup
{
    UploadProject::UploadProject(QObject *parent) :
        QObject(parent), imldr(0), proj_setts(0), is_modifed(true), pre_load(false)
    {
        installEventFilter(this);
    }

    UploadProject::~UploadProject()
    {
        clearObjs();
        if(imldr)
        {
            if(imldr->isRunning())
                imldr->terminate();

            imldr->deleteLater();
            imldr = 0;
        }
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

    bool UploadProject::isModified() const
    {
        return is_modifed;
    }

    void UploadProject::addCommonsImgObj(CommonsImgObject *obj, bool write)
    {
        objs.append(obj);

        if(write)
        {
            is_modifed = true;
            saveToFile(obj);
        }
    }

    CommonsImgObject *UploadProject::addCommonsImgObj(const QString &path)
    {
        CommonsImgObject *iobj = new CommonsImgObject(path, this);
        iobj->setUuid();

        addCommonsImgObj(iobj, true);
        return iobj;
    }

    void UploadProject::removeCommonsImgObj(CommonsImgObject *obj)
    {
        if(!proj_setts || proj_setts->fileName() != proj_path)
            proj_setts = QSharedPointer<QSettings>(new QSettings(proj_path, QSettings::IniFormat));

        int idx = -1;
        if(known_objs.contains(obj->uuid()) && (idx = objs.indexOf(obj)) > -1)
        {
            proj_setts->remove(known_objs.value(obj->uuid()));
            obj->deleteLater();

            objs.removeAt(idx);
            is_modifed = true;
        }

        proj_setts.clear();
    }

    void UploadProject::loadFromFile(bool clear, const QString & whe)
    {
        if(whe.isEmpty() == false)
            proj_path = whe;

        if(!proj_setts || proj_setts->fileName() != proj_path)
            proj_setts = QSharedPointer<QSettings>(new QSettings(proj_path, QSettings::IniFormat));

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
        {
            clearObjs();
            is_modifed = true;
        }

        const quint64 files = proj_setts->value("meta/file_count", 0).toULongLong();
        QStringList filelist;
        QHash<QString, QUuid> uuids;

        for(quint64 i = 0; i < files; ++i)
        {
            const QString &grpn = QString("file_%1").arg(i+1);
            if(proj_setts->contains(grpn + "/file_path") == false)
                continue;

            proj_setts->beginGroup(grpn);


            known_objs.insert(QUuid(proj_setts->value("file_uuid").toString()), proj_setts->group());
            uuids.insert(proj_setts->value("file_path").toString(), QUuid(proj_setts->value("file_uuid").toString()));
            filelist << proj_setts->value("file_path").toString();

            proj_setts->endGroup();
        }

        if(imldr)
        {
            imldr->terminate();
            imldr->deleteLater();

            imldr = 0;
        }

        pre_load = is_modifed;
        imldr = new ImageLoader(filelist, uuids, false, this);
        imldr->start();
    }

    void UploadProject::saveToFile(CommonsImgObject * ob, const QString & whe)
    {
        if(whe.isEmpty() == false)
            proj_path = whe;

        if(!proj_setts || proj_setts->fileName() != proj_path)
            proj_setts = QSharedPointer<QSettings>(new QSettings(proj_path, QSettings::IniFormat));

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
                saveObjToFile(imob);
                proj_setts->endGroup();
                proj_setts -> setValue("meta/file_count", QVariant::fromValue<quint64>(objs.size()));
            }
        }
        else
        {
            if(ob->uuid().isNull())
                ob->setUuid();
            saveObjToFile(ob);
        }

        is_modifed = false;
        proj_setts->sync();
    }

    void UploadProject::cancelLoad()
    {
        if(imldr && imldr->isRunning())
        {
            imldr->quit();
        }
    }

    void UploadProject::clearObjs()
    {
        foreach(CommonsImgObject *o, objs)
            o->deleteLater();

        is_modifed = false;
        objs.clear();
    }

    void UploadProject::saveObjToFile(const CommonsImgObject *imob)
    {
        bool endgrp = false;
        if((endgrp = proj_setts->group().isEmpty())) //auto append
        {
            proj_setts->beginGroup(QString("file_%1").arg(objs.size()));
        }

        proj_setts->setValue("file_path", imob->imageFile()->getFileInfo().absoluteFilePath());
        proj_setts->setValue("file_uuid", imob->uuid().toString());

        proj_setts->setValue("cms_filename", imob->cmsFilename());
        proj_setts->setValue("cms_author", imob->cmsAuthor());
        proj_setts->setValue("cms_cats", imob->cmsCats());
        proj_setts->setValue("cms_datetime", imob->cmsDateTime());
        proj_setts->setValue("cms_description", imob->cmsDescription());
        proj_setts->setValue("cms_geo", imob->cmsGeo());
        proj_setts->setValue("cms_license", imob->cmsLicense());

        QVariantList geos;
        geos << std::get<0>(imob->fileGeo()) << std::get<1>(imob->fileGeo())<< std::get<2>(imob->fileGeo()) << std::get<3>(imob->fileGeo());
        proj_setts->setValue("file_geo", geos);

        known_objs.insert(imob->uuid(), proj_setts->group());

        if(endgrp)
        {
            proj_setts->endGroup();
            proj_setts -> setValue("meta/file_count", QVariant::fromValue<quint64>(objs.size()));
        }
    }

    void UploadProject::loadObjFromFile(const QUuid &uuid, CommonsImgObject *imob)
    {
        proj_setts->beginGroup(known_objs.value(uuid));

        imob->setCmsFilename(proj_setts->value("cms_filename").toString());
        imob->setCmsAuthor(proj_setts->value("cms_author").toString());
//        imob->setFileDateTime(proj_setts->value("file_datetime").toDateTime());
        imob->setCmsDateTime(proj_setts->value("cms_datetime").toString());
        imob->setCmsDescription(proj_setts->value("cms_description").toString());
        imob->setCmsLicense(proj_setts->value("cms_license").toString());
        imob->setCmsGeo(proj_setts->value("cms_geo").toString());
        imob->setCmsCats(proj_setts->value("cms_cats").toString());

        proj_setts->endGroup();
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

    bool UploadProject::eventFilter(QObject *, QEvent *e)
    {
        if(e->type() == QEvent::User)
        {
            ImageLoader::ImageLoaderEvent *ilevt = dynamic_cast<ImageLoader::ImageLoaderEvent*>(e);
            if(ilevt)
            {
                if(ilevt->evt_type == ImageLoader::ImageLoaderEvent::Starting)
                {
                    if(parent())
                    {
                        UploadProjectEvent *evt = new UploadProjectEvent(UploadProjectEvent::LoadingStarted, 0);
                        evt->the_msg["files"] = ilevt->the_msg.value("files");

                        QApplication::postEvent(parent(), evt);
                    }

                    ilevt->accept();
                    return true;
                }
                else if(ilevt->evt_type == ImageLoader::ImageLoaderEvent::ObjectCreated)
                {
                    loadObjFromFile(ilevt->cms_obj->uuid(), ilevt->cms_obj);
                    addCommonsImgObj(ilevt->cms_obj, false);
                    ilevt->cms_obj->setParent(this);

                    if(parent())
                    {
                        UploadProjectEvent *evt = new UploadProjectEvent(UploadProjectEvent::FileLoaded, ilevt->cms_obj);
                        evt->the_msg["file"] = ilevt->cms_obj->imageFile()->getFileInfo().canonicalFilePath();

                        QApplication::postEvent(parent(), evt);
                    }

                    ilevt->accept();

                    return true;
                }
                else if(ilevt->evt_type == ImageLoader::ImageLoaderEvent::Finished)
                {
                    quint64 lc = imldr->loadedFilesCount();

                    imldr->terminate();
                    imldr->deleteLater();
                    imldr = 0;

                    if(parent())
                    {
                        UploadProjectEvent *evt = new UploadProjectEvent(UploadProjectEvent::LoadingFinished, 0);
                        evt->the_msg["loaded_files"] = lc;

                        QApplication::postEvent(parent(), evt);
                    }

                    is_modifed = pre_load;

                    emit projectLoaded();

                    ilevt->accept();
                    return true;
                }
            }
        }

        return false;
    }

}
