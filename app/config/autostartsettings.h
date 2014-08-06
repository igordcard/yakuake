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

#ifndef AUTOSTARTSETTINGS_H
#define AUTOSTARTSETTINGS_H

#include "ui_autostartsettings.h"
#include "sessionlistview.h"
#include "sessionsettings.h"
#include "common.h"
#include <kdialog.h>

class ConnectionSettings;
class QTreeWidgetItem;
class QCheckBox;

namespace Yakuake
{
    class AutostartSettings : public QWidget, private Ui::AutostartSettings
    {
        Q_OBJECT

        public:
            explicit AutostartSettings(QWidget* parent = 0);
            ~AutostartSettings();
            enum DataRole
            {
                SessionId = Qt::UserRole + 1,
                SortIndex = Qt::UserRole + 2,
                IsTerminal = Qt::UserRole + 3,
                TerminalId = Qt::UserRole + 4
            };

        public slots:
            void updateSessionList();

        signals:
            void connectTo(Yakuake::ConnectionFlag flag, int SessionId);
            void connectTo(Yakuake::ConnectionFlag flag, ConnectionSettings connectionSettings);
            void sessionsChanged(const Yakuake::SessionSettingsPtr Session = Yakuake::SessionSettingsPtr());

        protected slots:
            virtual void slotOk();
            void slotClose();
            void slotAdd();
            void slotEdit();
            void slotDelete();

            void slotSetGroupExpanded(QTreeWidgetItem* item);
            void slotSetGroupCollapsed(QTreeWidgetItem* item);

            void slotAboutToMove();
            void slotMoved();

            void updateButtons();

        protected:
            QTreeWidgetItem* insertSession(SessionSettingsPtr Session);
            void addSession(SessionSettingsPtr Session);

            int m_lastSortColumn;
            Qt::SortOrder m_lastSortOrder;

        private:
            int selectedChildrenCount(QTreeWidgetItem* item);

            bool m_selectedItem;
            int m_selectedSessionId;
            TerminalSettings m_selectedTerminal;
            QTreeWidgetItem* m_selectedItemPtr;

    };

    class SessionListItem : public QTreeWidgetItem
    {
        public:
            SessionListItem(QTreeWidget* tree, QStringList & strings);
            SessionListItem(QTreeWidgetItem* parent, QStringList & strings);
            bool operator<(const QTreeWidgetItem &other) const;
            enum DataRole
            {
                SortIndex = Qt::UserRole + 2
            };
    };
}
#endif
