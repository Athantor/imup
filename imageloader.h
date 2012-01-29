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

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QThread>
#include <QDir>
#include <QMap>
#include <QUuid>
#include <QEvent>

#include "commonsimgobject.h"

namespace imup {
    class ImageLoader : public QThread
    {
            Q_OBJECT
        public:
            class ImageLoaderEvent : public QEvent
            {
                public:
                    typedef enum
                    {
                        ObjectCreated,
                        Message,
                        Finished,
                        Invalid

                    } EventType;

                    ImageLoaderEvent(EventType et, CommonsImgObject *obj, const QString &msg = QString()) :
                        QEvent(QEvent::User), the_msg(msg), cms_obj(obj), evt_type(et)
                    {

                    }

                    virtual ~ImageLoaderEvent()
                    {
                        if(isAccepted() == false)
                        {
                            delete cms_obj;
                            cms_obj = 0;
                        }
                    }

                    const QString &the_msg;
                    CommonsImgObject *cms_obj;
                    const EventType evt_type;
            };

            explicit ImageLoader(const QString & filedir, bool recursive = true, QObject *parent = 0);
            explicit ImageLoader(const QStringList & files, QHash<QString, QUuid> uuidlist = QHash<QString, QUuid>(), QObject *parent = 0);
            virtual ~ImageLoader();

            QList<CommonsImgObject *>* objects(bool reparent);

        public slots:

        protected:
            virtual void run();

        private:
            Q_DISABLE_COPY(ImageLoader)

            QStringList file_paths;
            QString path_to_load;
            QHash<QString, QUuid> uuidl;
            bool recurse;
            QList<CommonsImgObject *>* objs;

            bool makeFilePaths(const QString & p = QString());
            quint64 makeCmsObjs();
    };
}

#endif // IMAGELOADER_H
