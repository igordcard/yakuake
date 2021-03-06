/*
  Copyright (C) 2008 by Eike Hein <hein@kde.org>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor appro-
  ved by the membership of KDE e.V.), which shall act as a proxy 
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see http://www.gnu.org/licenses/.
*/


#ifndef SKINLISTDELEGATE_H
#define SKINLISTDELEGATE_H


#include <QAbstractItemDelegate>


class SkinListDelegate : public QAbstractItemDelegate
{
    public:
        explicit SkinListDelegate(QObject* parent = 0);
         ~SkinListDelegate();

        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index) const;
        virtual QSize sizeHint (const QStyleOptionViewItem& option, const QModelIndex& index) const;


    private:
        void paintBackground(QPainter* painter, const QStyleOptionViewItem& option) const;
        void paintIcon(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index) const;
        void paintText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex &index) const;
};

#endif 
