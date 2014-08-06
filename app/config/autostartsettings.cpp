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

#include "autostartsettings.h"
#include "application.h"
#include "sessioneditdialog.h"
#include "sessionsettings.h"
#include "connectionsettings.h"
#include "preferences.h"

#include <QCheckBox>
#include <QHeaderView>

#include <KGuiItem>
#include <KMessageBox>

#include <QPointer>

namespace Yakuake
{
    SessionListItem::SessionListItem( QTreeWidget * tree, QStringList & strings) : QTreeWidgetItem (tree,strings)
    {
    }

    SessionListItem::SessionListItem( QTreeWidgetItem * parent, QStringList & strings) : QTreeWidgetItem (parent,strings)
    {
    }

    bool SessionListItem::operator<(const QTreeWidgetItem &other) const
    {
        int column = treeWidget()->sortColumn();
        if (column==0)
        {
            if (data(0,SortIndex).toInt() >= other.data(0,SortIndex).toInt())
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        return text(column) < other.text(column);
    }

    AutostartSettings::AutostartSettings(QWidget* parent) : QWidget(parent)
    {
        setupUi(this);

        m_sessionList->setFocus();

        m_selectedItem = false;
        m_selectedItemPtr = 0;
        m_selectedTerminal = TerminalSettings("");

        // Load session list
        updateSessionList();

        connect(m_sessionList, SIGNAL(aboutToMove()), this, SLOT(slotAboutToMove()));
        connect(m_sessionList, SIGNAL(moved()), this, SLOT(slotMoved()));
        connect(m_sessionList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotOk()));
        connect(m_sessionList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
        connect(m_sessionList, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotSetGroupExpanded(QTreeWidgetItem*)));
        connect(m_sessionList, SIGNAL(itemCollapsed(QTreeWidgetItem*)), this, SLOT(slotSetGroupCollapsed(QTreeWidgetItem*)));
        connect(m_addButton, SIGNAL(clicked()), this, SLOT(slotAdd()));
        connect(m_editButton, SIGNAL(clicked()), this, SLOT(slotEdit()));
        connect(m_delButton, SIGNAL(clicked()), this, SLOT(slotDelete()));
        connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
        connect(this, SIGNAL(cancelClicked()), this, SLOT(slotClose()));
        connect(this, SIGNAL(sessionsChanged(Yakuake::SessionSettingsPtr)), this, SLOT(updateSessionList()));

        updateButtons();

        m_sessionList->header()->setMovable(false); // don't let the user reorder the header
        m_sessionList->sortItems(0, Qt::AscendingOrder);
        //because it sorts the first column in ascending order by default
        //causing problems and such.
        m_sessionList->topLevelItem(0)->setSelected(true);
    }

    AutostartSettings::~AutostartSettings()
    {
    }

    void AutostartSettings::slotClose()
    {
        //accept();
        // TODO
    }

    void AutostartSettings::slotOk()
    {
        QList<QTreeWidgetItem*> selected = m_sessionList->selectedItems();
        foreach (QTreeWidgetItem* item, selected)
        {
            if (item->data(0,IsTerminal).toBool())
            {
                ConnectionSettings settings;
                SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());
                settings.setSession(session);

                settings.setTerminal(session->terminalByIndex(item->data(0,TerminalId).toInt()));

                emit connectTo(Yakuake::PromptToReuseConnection, settings);
            }
            else
                emit connectTo(Yakuake::PromptToReuseConnection, item->data(0,SessionId).toInt());
        }
    }

    void AutostartSettings::slotAdd()
    {
        QPointer<SessionEditDialog> dlg = new SessionEditDialog(i18n("New Session"), this);

        if(dlg->exec() == KDialog::Accepted)
        {
            addSession(dlg->sessionSettings());

            emit sessionsChanged(dlg->sessionSettings());
        }
        delete dlg;
    }

    void AutostartSettings::slotEdit()
    {
        QTreeWidgetItem* item = m_sessionList->selectedItems().first();

        if (item)
        {
            Yakuake::SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());

            if (session)
            {
                QPointer<SessionEditDialog> dlg = new SessionEditDialog(i18n("Edit Session"), this);

                dlg->setSessionSettingsPtr(session);

                if (item->data(0,IsTerminal).toBool())
                {
                    if(dlg->execAndEditTerminal(session->terminalByIndex(item->data(0,TerminalId).toInt())) == KDialog::Accepted)
                    {
                        delete item;

                        m_selectedItem = true;
                        m_selectedSessionId = session->id();
                        m_selectedTerminal = dlg->editedTerminal();

                        *session = *dlg->sessionSettings();

                        emit sessionsChanged(session); // will call updateSessionList
                    }
                }
                else
                {
                    if(dlg->exec() == KDialog::Accepted)
                    {
                        delete item;

                        m_selectedItem = true;
                        m_selectedSessionId = session->id();
                        m_selectedTerminal = TerminalSettings("");

                        *session = *dlg->sessionSettings();

                        emit sessionsChanged(session); // will call updateSessionList
                    }
                }
                delete dlg;
            }
        }
    }

    void AutostartSettings::slotDelete()
    {
        QList<QTreeWidgetItem*> selectedItems;
        // Make sure we're not deleting a session's only terminals
        QTreeWidgetItem* parent = 0;
        QTreeWidgetItemIterator it(m_sessionList, QTreeWidgetItemIterator::Selected);
        while (*it)
        {
            //if it has a parent that's also selected it'll be deleted anyway so no need to worry
            if (!(*it)->parent() || !(*it)->parent()->isSelected())
            {
                if ((*it)->data(0,IsTerminal).toBool())
                {
                    parent = (*it)->parent();

                    if (parent && parent->childCount() == 1)
                    {
                        KMessageBox::error(this, i18n("You cannot delete %1.\n\nThe session %2 needs to have at least one terminal.",(*it)->text(0),parent->text(0)));
                        return;
                    }
                    else if (parent && parent->childCount() == selectedChildrenCount(parent))
                    {
                        KMessageBox::error(this, i18np("You cannot delete the selected terminal.\n\nThe session %2 needs to have at least one terminal.",
                                            "You cannot delete the selected terminals.\n\nThe session %2 needs to have at least one terminal.",
                                            selectedChildrenCount(parent),
                                            parent->text(0)));
                                            return;
                    }
                }
                selectedItems.append(*it); // if it hasn't returned by now it's good to go
            }
            ++it;
        }
        if (selectedItems.isEmpty())
            return;
        // Ask the user if he really wants to delete what he selected
        QString question;
        if (selectedItems.count()>1)
            question = i18n("Do you really want to delete the selected entries?");
        else
            question = i18n("Do you really want to delete %1?",selectedItems.first()->text(0));

        if (KMessageBox::warningContinueCancel(this,question) == KMessageBox::Cancel)
        {
            return;
        }

        // Have fun deleting
        QTreeWidgetItem* rootItem = m_sessionList->invisibleRootItem();
        foreach (QTreeWidgetItem* item, selectedItems)
        {
            if (item == m_selectedItemPtr)
                m_selectedItemPtr = 0;

            rootItem->removeChild(item);
            if (item->data(0,IsTerminal).toBool())
            {
                Yakuake::SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());
                session->removeTerminal(session->terminalByIndex(item->data(0,TerminalId).toInt()));
            }
            else
            {
                Preferences::removeSession(item->data(0,SessionId).toInt());
            }
            delete item;
        }

        emit sessionsChanged();
   }

    void AutostartSettings::slotSetGroupExpanded(QTreeWidgetItem* item)
    {
        Yakuake::SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());
        session->setExpanded(true);
    }

    void AutostartSettings::slotSetGroupCollapsed(QTreeWidgetItem* item)
    {
        Yakuake::SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());
        session->setExpanded(false);

        for(int i = 0; i < item->childCount(); i++)
        {
          if(item->child(i)->isSelected())
          {
            item->child(i)->setSelected(false);
            item->setSelected(true);
          }
        }
    }

    void AutostartSettings::slotAboutToMove()
    {
        m_lastSortColumn = m_sessionList->sortColumn();
        m_lastSortOrder = m_sessionList->header()->sortIndicatorOrder();
        m_sessionList->setSortingEnabled(false);
        m_sessionList->header()->setSortIndicatorShown(true);
    }

    void AutostartSettings::slotMoved()
    {
        Yakuake::SessionHash newSessionHash;

        QTreeWidgetItem* item;
        int sort=0;
        for (int i=0; i < m_sessionList->topLevelItemCount(); i++ )
        {
            item = m_sessionList->topLevelItem(i);
            Yakuake::SessionSettingsPtr session = Preferences::sessionById(item->data(0,SessionId).toInt());

            item->setExpanded(session->expanded());

            session->setSortIndex(sort);

            newSessionHash.insert(item->data(0,SessionId).toInt(),session);

            item->setData(0,SortIndex,sort++);
            for(int j=0; j < item->childCount(); j++)
                item->child(j)->setData(0,SortIndex,++sort);

        }
        if(Preferences::sessionHash() != newSessionHash)
        {
            Preferences::setSessionHash(newSessionHash);

            emit sessionsChanged();
        }
        m_sessionList->setSortingEnabled(true);
        m_sessionList->sortItems(m_lastSortColumn, m_lastSortOrder);
    }

    void AutostartSettings::updateButtons()
    {
        int count = m_sessionList->selectedItems().count();
        bool enable = (count > 0);

        //enableButtonOk(enable);
        m_delButton->setEnabled(enable);

        enable = (count == 1);
        m_editButton->setEnabled(enable);
    }

    void AutostartSettings::addSession(SessionSettingsPtr session)
    {
        int sortTotal = m_sessionList->topLevelItemCount();
        session->setSortIndex(sortTotal + 1);

        Preferences::addSession(session);
        QTreeWidgetItem* item = insertSession(session);
        m_sessionList->clearSelection();
        item->setSelected(true);
        m_sessionList->setCurrentItem(item);
    }

    void AutostartSettings::updateSessionList()
    {
        if (!m_selectedItem && m_sessionList->currentItem())
        {
            QTreeWidgetItem* item = m_sessionList->currentItem();

            m_selectedItem = true;
            m_selectedSessionId = item->data(0,SessionId).toInt();

            if (item->data(0,IsTerminal).toBool())
                m_selectedTerminal = Preferences::sessionById(m_selectedSessionId)->terminalByIndex(item->data(0,TerminalId).toInt());
            else
                m_selectedTerminal = TerminalSettings("");
        }

        m_sessionList->setUpdatesEnabled(false);
        m_sessionList->clear();

        Yakuake::SessionHash sessions = Preferences::sessionHash();
        QHashIterator<int, SessionSettingsPtr> it(sessions);

        QTreeWidgetItem* sessionItem = 0;
        while(it.hasNext())
        {
            it.next();
            sessionItem = insertSession((it.value()));
            if(m_selectedItem && m_selectedTerminal.name().isEmpty() && it.key()==m_selectedSessionId)
                m_selectedItemPtr = sessionItem;
        }

        // Highlight the last edited item
        if (m_selectedItem)
        {
            m_selectedItemPtr->setSelected(true);
            m_sessionList->setCurrentItem(m_selectedItemPtr);
            m_selectedItem = false;
        }

        m_sessionList->setUpdatesEnabled(true);
        m_sessionList->repaint();
    }

    QTreeWidgetItem* AutostartSettings::insertSession(SessionSettingsPtr session)
    {

        // Insert the session into the list
        QTreeWidgetItem* sessionItem = new SessionListItem(m_sessionList, QStringList() << session->name());
        sessionItem->setData(0,SessionId,session->id());
        sessionItem->setData(0,SortIndex,session->sortIndex());
        sessionItem->setData(0,IsTerminal,false);

        // Recreate expanded/collapsed state
        if (session->expanded())
            sessionItem->setExpanded(true);

        // Produce a list of this session's terminals and iterate over it
        Yakuake::TerminalList terminalList = session->terminalList();
        Yakuake::TerminalList::const_iterator terminalIt;

        int i = 0;
        for (terminalIt = terminalList.constBegin(); terminalIt != terminalList.constEnd(); ++terminalIt)
        {
            // Produce a string representation of the terminal object
            QString name = (*terminalIt).name();

            // Insert the terminal into the list, as child of the session list item
            QTreeWidgetItem* terminalItem = new SessionListItem(sessionItem, QStringList() << name);
            terminalItem->setData(0,SessionId,session->id());
            terminalItem->setData(0,SortIndex,i);
            terminalItem->setData(0,IsTerminal,true);
            terminalItem->setData(0,TerminalId,i);
            terminalItem->setFirstColumnSpanned(true);
            // Initialize a pointer to the new location of the last edited terminal
            if (m_selectedItem && m_selectedTerminal==(*terminalIt))
                m_selectedItemPtr = terminalItem;

            ++i;
        }

        return sessionItem;
    }

    int AutostartSettings::selectedChildrenCount(QTreeWidgetItem* item)
    {
        int count = 0;

        for (int i=0; i< item->childCount(); i++)
        {
            if (item->child(i)->isSelected()) count++;
        }

        return count;
    }
}

#include "autostartsettings.moc"
