/*
  Copyright (C) 2014 by Igor Duarte Cardoso <igordcard@gmail.com>

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

#include "sessionlistview.h"

SessionListView::SessionListView(QWidget *parent)
: QTreeWidget(parent)
{
}

SessionListView::~SessionListView()
{
}
void SessionListView::dragMoveEvent(QDragMoveEvent *e)
{
    QTreeWidgetItem* item = this->itemAt(e->pos());
    QTreeWidgetItem* sItem = this->currentItem();
    bool bad;
    //  bad selection || dropping on viewport must be a toplevelitem || children of the same parent (or lack thereof)
    if (badDropSelection() || (!item && indexOfTopLevelItem(sItem)<0) || (item && item->parent() != sItem->parent()))
    {
        bad=true;
        setDropIndicatorShown(false);
    }
    else
    {
        bad=false;
        setDropIndicatorShown(true);
    }
    QTreeView::dragMoveEvent(e);

    if(bad)
        e->ignore();

}
void SessionListView::dragLeaveEvent(QDragLeaveEvent *e)
{
    QAbstractItemView::dragLeaveEvent(e);
    emit moved();
}

void SessionListView::dragEnterEvent(QDragEnterEvent *e)
{
    emit aboutToMove();
    QAbstractItemView::dragEnterEvent(e);
}

bool SessionListView::badDropSelection()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    QList<QTreeWidgetItem*> children;
    QTreeWidgetItem* parent;
    int t=0; //top item count
    foreach (QTreeWidgetItem* item, items)
    {
        if (indexOfTopLevelItem(item)<0) // is a child
        {
                children.append(item);
                parent = children.at(0)->parent();
                // make sure all the children have the same parent
                for (int i=0; i < children.count(); i++)
                {
                    if (children.at(i)->parent() != parent)
                        return true;
                }
        }
        else    t++;

        if (t > 0 && children.count() > 0) //make sure we don't have a top and a child selected
            return true;
    }
    return false;
}

void SessionListView::dropEvent(QDropEvent *event)
{
    if(badDropSelection())
    {
        event->ignore();
        return;
    }
    QTreeWidgetItem* sourceItem = currentItem();
    QTreeWidget::dropEvent(event);
    //clean up after dropEvent, flatten the list
    QList<QTreeWidgetItem*> children;
    int childIndex;
    QTreeWidgetItem* sItem;
    QTreeWidgetItem* item;
    //TODO figure out why the iterator fails to do this without crashing
    for(int i=0; i<topLevelItemCount(); i++)
    {
        for(int j=0; j<topLevelItem(i)->childCount(); j++)
        {
            if(topLevelItem(i)->child(j)->childCount() > 0)
            {
                sItem = topLevelItem(i)->child(j);
                if(sItem->text(1).isEmpty()) // if the second column has no text, it's a
                {
                    childIndex = j;
                    children = topLevelItem(i)->child(j)->takeChildren();
                    if (!children.isEmpty())
                        topLevelItem(i)->insertChildren(childIndex++, children);

                }
                else
                {
                    childIndex = i;
                    item = topLevelItem(i)->takeChild(j);
                    insertTopLevelItem(childIndex++, item);
                }
            }
        }
    }
    setCurrentItem(sourceItem);
    emit moved();
}
