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
#include <QPalette>

#include "metainfomodel.h"

namespace imup
{

    MetainfoModel::MetainfoModel(const ImageFile& imf, QObject *parent) :
        QAbstractItemModel(parent), MD_SETS(3), COLS(2), imfile(imf), metakval(3),
        ttip_cache(new QCache<QPair<int, int>, QString>)
    {
        Fill_meta();
    }

    MetainfoModel::~MetainfoModel()
    {
    }

    QModelIndex MetainfoModel::index(int row, int column, const QModelIndex &parent) const
    {
        if(!hasIndex(row, column, parent))
            return QModelIndex();

        if(parent.isValid() == false)
            return createIndex(row, column, static_cast<quint32>(0));
        else
            return createIndex(row, column, static_cast<quint32>(parent.row()+1));
    }

    QModelIndex MetainfoModel::parent(const QModelIndex &index) const
    {
        if(index.internalId() < 1)
            return QModelIndex();

        return createIndex(index.internalId()-1, index.column(), static_cast<quint32>(0));

    }

    int MetainfoModel::rowCount(const QModelIndex &parent) const
    {
        if(parent.isValid() == false) //top root: we have 3 metadata sets
            return MD_SETS;
        else if(parent.internalId() == 0) //metadata set
            return metakval.at(parent.row()).size();
        else
            return 0; //leaf → no children
    }

    int MetainfoModel::columnCount(const QModelIndex &parent) const
    {
        Q_UNUSED(parent);
        return COLS;
    }

    QVariant MetainfoModel::data(const QModelIndex &index, int role) const
    {
        if(index.isValid() == false)
            return QVariant();

        if(role == Qt::DisplayRole)
        {
            if(index.internalId() < 1)
            {
                ImageFile::MetadataType mdt = row2metatype(index.row());

                if(index.column() == 0)
                {
                    switch(mdt)
                    {
                        case ImageFile::MDT_EXIF:
                            return tr("EXIF");
                        case ImageFile::MDT_IPTC:
                            return tr("IPTC");
                        case ImageFile::MDT_XMP:
                            return tr("XMP");
                        case ImageFile::MDT_Invalid:
                        default:
                            return tr("<imup:error:MDS>");
                    }
                }
                else if(index.column() == 1)
                {
                    return tr("[Elements: %1]").arg(metakval[index.row()].size());
                }
            }
            else
            {
                const int idx = index.internalId()-1; //leaf parent → a metadata set

               // qDebug() << __PRETTY_FUNCTION__ << index;

                if(row2metatype(idx) == ImageFile::MDT_Invalid)
                    return tr("<imup:err:MDT>");

                auto kval = metakval[idx];

                if(index.row() > kval.size())
                    return tr("<imup:err:row>");

                if(index.column() == 0)
                {
                    return kval.at(index.row()).first;
                }
                else if(index.column() == 1)
                {
                    return kval.at(index.row()).second;
                }
                else
                {
                    return tr("<imup:err:col>");
                }
            }
        }
        else if(role == Qt::ToolTipRole)
        {
            const int idx = index.internalId()-1;
            if(idx >= 0 && index.column() == 1)
            {
                QString md;
                auto cache_key = qMakePair(idx, index.row());

                if(ttip_cache->contains(cache_key))
                {
                    md = *ttip_cache->object(cache_key);
                }
                else
                {
                    auto mdtype = row2metatype(idx);
                    if(mdtype == ImageFile::MDT_Invalid)
                        return QVariant();

                    auto kval = metakval[idx];
                    if(index.row() > kval.size())
                        return QVariant();

                    imfile.getMetaData(kval.at(index.row()).first, md, mdtype);
                    ttip_cache->insert(cache_key, new QString(md));
                }

                //if it's the same as display value, don't show tooltip
                if(md.isEmpty() == false &&  metakval[idx].at(index.row()).second == md)
                    return QVariant();

                return  "<tt>" + md + "</tt>";
            }
        }
        else if(role == Qt::BackgroundRole)
        {
            if(index.internalId() < 1)
            {
                return QApplication::palette().light();
            }
        }

        return QVariant();
    }

    QVariant MetainfoModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if(role == Qt::DisplayRole)
        {
            if(orientation == Qt::Horizontal)
            {
                switch(section)
                {
                    case 0:
                        return tr("Property");
                        break;
                    case 1:
                        return tr("Value");
                        break;
                    default:
                        return tr("WAT‽ %1").arg(section);
                        break;
                }
            }
        }

        return QVariant();
    }

    void MetainfoModel::Fill_meta()
    {
        Q_ASSERT(static_cast<int>(ImageFile::MDT_EXIF) == 0);

        for(int i = ImageFile::MDT_EXIF; i < ImageFile::MDT_Invalid; ++i)
        {
            ImageFile::TagList_t mapit = imfile.getMetaData(row2metatype(i));
            for(auto it = mapit.begin(); it != mapit.end(); ++it)                
                metakval[i] << qMakePair(it.key(), it.value().toString());
        }

    }

    ImageFile::MetadataType MetainfoModel::row2metatype(int row)
    {
        switch(row)
        {
            case 0:
                return ImageFile::MDT_EXIF;
            case 1:
                return ImageFile::MDT_IPTC;
            case 2:
                return ImageFile::MDT_XMP;
            default:
                return ImageFile::MDT_Invalid;
        }
    }


}
