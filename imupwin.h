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

#ifndef IMUPWIN_H
#define IMUPWIN_H

#include <QtGui/QMainWindow>
#include <QCloseEvent>
#include <QProgressDialog>
#include <QList>
#include <QUrl>

#include "uploadproject.h"
#include "commonsimgwidget.h"
#include "imageloader.h"

namespace Ui {
    class imupWin;
}

namespace imup
{
    class imupWin : public QMainWindow
    {
            Q_OBJECT

        public:
            explicit imupWin(QWidget *parent = 0);
            ~imupWin();

            static imupWin* instance();
            const QSettings& getSetts() const;

            virtual void dragEnterEvent(QDragEnterEvent *evt);
            virtual void dropEvent(QDropEvent *evt);

        public slots:
            bool saveProject();
            bool saveProjectAs(const QString &ppath = QString());
            bool loadProject(const QString &fproj = QString());
            bool newProject();

            void addFileObject(CommonsImgObject * cms_o);
            void addFiles(const QStringList & flist = QStringList());
            void addDirectory(const QString & dir = QString());

        protected:
            void changeEvent(QEvent *e);
            virtual void	closeEvent ( QCloseEvent * event );

            void makeToolbarButtons();

            bool eventFilter ( QObject * watched, QEvent * e );

            virtual void multiLoad(const QList<QUrl> &urllist);

        protected slots:
            void addImageWidgetsFromProject();
            void removeObject(CommonsImgWidget *imwgt);
            void cancelLoad();

        private:
            static const QString unsaved_proj_path;
            Ui::imupWin *ui;
            UploadProject *proj;
            QString project_path;
            QHash<QUuid, CommonsImgWidget*> wgtlist;
            ImageLoader * imloader;
            QSettings global_setts;
            QProgressDialog *pdlg;

            static imupWin* _intance;

            void connects();
            void lockIt(bool unl);
            void validateSetts();
    };
}

#endif // IMUPWIN_H
