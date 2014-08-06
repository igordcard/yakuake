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

#include "sessionsettings.h"
#include "application.h"


namespace Yakuake
{

    int SessionSettings::s_availableId = 0;

    SessionSettings::SessionSettings()
        : KShared()
    {
        m_id = s_availableId;
        s_availableId++;
        m_sortIndex = m_id;
        m_expanded = false;
    }

    SessionSettings::SessionSettings(int id)
        : KShared()
    {
        if(id < 0)
        {
            m_id = s_availableId;
            s_availableId++;
        }
        else
        {
            m_id = id;
        }

        m_sortIndex = m_id;
        m_expanded = false;
    }

    SessionSettings::SessionSettings(const SessionSettings& settings)
        : KShared()
    {
        (*this) = settings;
    }


    SessionSettings& Yakuake::SessionSettings::operator=(const SessionSettings& settings)
    {
        setName(settings.name());
        setTerminalList(settings.terminalList());
        setExpanded(settings.expanded());
        m_id = settings.id();
        m_sortIndex = settings.sortIndex();
        return *this;
    }

    SessionSettings::SessionSettings(const QString& name)
        : KShared()
    {
        setName(name);
        m_id = s_availableId;
        s_availableId++;
        m_sortIndex = m_id;
        m_expanded = false;
    }

    SessionSettings::~SessionSettings()
    {
    }

    void SessionSettings::setTerminalList(const TerminalList& list)
    {
        m_terminalList.clear();
        m_terminalList = list;
    }

    void SessionSettings::removeTerminal(const TerminalSettings& settings)
    {
        m_terminalList.removeOne(settings);
    }

    TerminalSettings SessionSettings::terminalByIndex(int index) const
    {
        TerminalList terminals = terminalList();

        if(index < terminals.count())
        {
            return terminals[index];
        }

        return TerminalSettings();
    }
}
