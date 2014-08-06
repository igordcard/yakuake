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


#ifndef SESSIONSETTINGS_H
#define SESSIONSETTINGS_H

#include "terminalsettings.h"
#include <ksharedptr.h>
#include <qhash.h>

namespace Yakuake
{

    class SessionSettings;
    typedef KSharedPtr<SessionSettings> SessionSettingsPtr;
    typedef QHash<int,SessionSettingsPtr> SessionHash;
    typedef QList<TerminalSettings> TerminalList;

    class SessionSettings : public KShared
    {
        public:
            explicit SessionSettings();
            explicit SessionSettings(int id);
            explicit SessionSettings(const SessionSettings& settings);
            explicit SessionSettings(const QString& name);
            ~SessionSettings();

            SessionSettings& operator=(const SessionSettings& settings);

            void setName(const QString& name) { m_name = name; }
            QString name() const { return m_name; }

            void setTerminalList(const TerminalList& list);
            void addTerminal(const TerminalSettings& settings) { m_terminalList.append(settings); }
            void removeTerminal(const TerminalSettings& settings);
            TerminalList terminalList() const { return m_terminalList; }
            TerminalSettings terminalByIndex(int index) const;

            int id() const { return m_id; }

            void setSortIndex(int sortIndex) { m_sortIndex = sortIndex; }
            int sortIndex() const { return m_sortIndex; }

            void setExpanded(bool enable) { m_expanded = enable; }
            bool expanded() const { return m_expanded; }

        private:
            static int s_availableId;
            int m_sortIndex;
            QString m_name;
            TerminalList m_terminalList;
            QString m_group;
            int m_id;
            bool m_expanded;
    };

}
#endif
