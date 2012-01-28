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

            explicit UploadProject(QObject *parent = 0);
            virtual ~UploadProject();

        public:
            const QList<CommonsImgObject*>& objects() const;

            void setProjectFilePath(const QString &ppath);
            const QString& projectFilePath() const;

        public slots:
            void addCommonsImgObj(CommonsImgObject * obj, bool write = true);
            CommonsImgObject * addCommonsImgObj(const QString &path);

            void loadFromFile(bool clear = true, const QString & whe = QString());
            void saveToFile(CommonsImgObject* =0, const QString & whe = QString());

        protected:
            QString proj_path;
            QList<CommonsImgObject *> objs;
            QSet<QUuid> known_objs;
            bool autosave;

            void clearObjs();
            void saveObjToFile(QSharedPointer<QSettings> proj_setts, const CommonsImgObject *imob);

            static bool checkDestFilePath(const QString&);
    };
}

#endif // UPLOADPROJECT_H
