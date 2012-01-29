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
#include <QApplication>
#include <QSettings>

#include "imageloader.h"

namespace imup {

    ImageLoader::ImageLoader(const QString & filedir, bool recursive, QObject *parent) :
        QThread(parent), path_to_load(filedir), uuidl(QHash<QString, QUuid>()), recurse(recursive), objs(0)
    {
    }

    ImageLoader::ImageLoader(const QStringList &files, QHash<QString, QUuid> uuidlist, QObject *parent) :
        QThread(parent), file_paths(files), uuidl(uuidlist), objs(new QList<CommonsImgObject *>())
    {
    }

    ImageLoader::~ImageLoader()
    {
        if(objs)
        {
            for(int i = 0; i < objs->size(); ++i)
            {
                delete objs->at(i);
            }

            delete objs;
            objs = 0;
        }
    }

    QList<CommonsImgObject *> *ImageLoader::objects(bool reparent)
    {

        QList<CommonsImgObject *>* ret = objs;
        if(reparent)
            objs = 0;

        return ret;
    }

    void ImageLoader::run()
    {
        if(path_to_load.isEmpty() == false)
        {
            file_paths.clear();
            makeFilePaths(path_to_load);
        }

        makeCmsObjs();

        if(parent())
        {
            ImageLoaderEvent *evt = new ImageLoaderEvent(ImageLoaderEvent::Finished, 0);
            QApplication::postEvent(parent(), evt);
        }

        emit finished();

    }

    bool ImageLoader::makeFilePaths(const QString & p)
    {
        QFileInfo qfi(p);
        QSettings ss;

        if(qfi.exists() == false || qfi.isReadable() == false)
            return false;

        if(qfi.isDir() && (p == path_to_load || recurse))
        {
            QDir the_dir(p);
            QStringList files, filters;

            filters << "*";
            the_dir.setNameFilters(ss.value("files/types_filter", filters).toStringList());

            files = the_dir.entryList(QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name|QDir::IgnoreCase|QDir::LocaleAware);
            foreach(const QString &path, files)
                makeFilePaths(the_dir.absoluteFilePath(path));

            return true;
        }
        else if(!qfi.isDir())
        {
            file_paths << p;
            return true;
        }

        return false;
    }

    quint64 ImageLoader::makeCmsObjs()
    {
        quint64 ctr = 0;
        foreach(const QString &file, file_paths)
        {
            CommonsImgObject *the_obj = new CommonsImgObject(file);
            the_obj->setUuid(uuidl.value(file, QUuid::createUuid()));;

            ctr++;

            if(parent())
            {
                the_obj->moveToThread(parent()->thread());
                ImageLoaderEvent *evt = new ImageLoaderEvent(ImageLoaderEvent::ObjectCreated, the_obj);
                QApplication::postEvent(parent(), evt);
            }
            else
            {
                objs->push_back(the_obj);
            }
        }

        return ctr;
    }
}
