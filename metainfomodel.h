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

#ifndef METAINFOMODEL_H
#define METAINFOMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QList>

#include "imagefile.h"

namespace imup
{

    class MetainfoModel : public QAbstractItemModel
    {
            Q_OBJECT
        public:
            explicit MetainfoModel(const ImageFile& imf, QObject *parent = 0);
            virtual ~MetainfoModel();

            virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
            virtual QModelIndex parent ( const QModelIndex & index ) const;
            virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
            virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
            virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
            QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

        signals:

        public slots:
        private:
            const ImageFile&  imfile;
            QVector<QList<QPair<QString, QString> > > metakval;


            void Fill_meta();
            static ImageFile::MetadataType row2metatype(int);

    };
}

#endif // METAINFOMODEL_H
