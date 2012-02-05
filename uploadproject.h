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

#ifndef UPLOADPROJECT_H
#define UPLOADPROJECT_H

#include <stdexcept>

#include <QObject>
#include <QSettings>
#include <QSet>
#include <QUuid>
#include <QEvent>

#include "imageloader.h"
#include "commonsimgobject.h"

namespace imup {

    class UploadProject : public QObject
    {
            Q_OBJECT
        public:

            class UploadProjectError : public std::runtime_error
            {
                public:
                    UploadProjectError(const QString& msg) : std::runtime_error(msg.toStdString()) {}
            };

            class UploadProjectEvent : public QEvent
            {
                public:
                    typedef enum
                    {
                        LoadingStarted,
                        FileLoaded,
                        LoadingFinished,
                        Invalid
                    } EvtType;

                    UploadProjectEvent(EvtType et, CommonsImgObject *ob =0) : QEvent(QEvent::User), evt_type(et),
                        cms_obj(ob)
                    {}

                    const EvtType evt_type;
                    CommonsImgObject *cms_obj;
                    QVariantHash the_msg;
            };

            explicit UploadProject(QObject *parent = 0);
            virtual ~UploadProject();

        public:
            const QList<CommonsImgObject*>& objects() const;

            void setProjectFilePath(const QString &ppath);
            const QString& projectFilePath() const;

            bool isModified() const;

        public slots:
            void addCommonsImgObj(CommonsImgObject * obj, bool write = true);
            CommonsImgObject * addCommonsImgObj(const QString &path);

            void removeCommonsImgObj(CommonsImgObject* obj);

            void loadFromFile(bool clear = true, const QString & whe = QString());
            void saveToFile(CommonsImgObject* =0, const QString & whe = QString());

            void cancelLoad();

        protected:
            QString proj_path;
            QList<CommonsImgObject *> objs;
            QMap<QUuid, QString> known_objs;
            QHash<QString, QString> known_objs_p;
            ImageLoader *imldr;
            bool autosave;
            QSharedPointer<QSettings> proj_setts;
            bool is_modifed, pre_load;

            void clearObjs();
            void saveObjToFile(const CommonsImgObject *imob);
            void loadObjFromFile(const QUuid & uuid, CommonsImgObject *imob);

            static bool checkDestFilePath(const QString&);

            virtual bool eventFilter(QObject *, QEvent *);


        private:
            Q_DISABLE_COPY(UploadProject)


        signals:
            void projectLoaded();
    };
}

#endif // UPLOADPROJECT_H
