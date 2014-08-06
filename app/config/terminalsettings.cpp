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

#include "terminalsettings.h"

namespace Yakuake
{

    TerminalSettings::TerminalSettings()
    {
    }

    TerminalSettings::TerminalSettings(const TerminalSettings& settings)
    {
        setName(settings.name());
        setCommands(settings.commands());
        setNeighbor(settings.neighbor());
        setSplitWay(settings.splitWay());
    }

    TerminalSettings::TerminalSettings(const QString& name)
    {
        setName(name);
    }

    TerminalSettings::~TerminalSettings()
    {
    }

    bool TerminalSettings::operator==(const TerminalSettings& settings) const
    {
        if (m_name.toLower() == settings.name().toLower() &&
            m_commands == settings.commands() &&
            m_neighbor == settings.neighbor() &&
            m_split == settings.splitWay())
        {
            return true;
        }
        else
            return false;
    }

    void TerminalSettings::setName(const QString& name)
    {
        m_name = name.trimmed();
    }

    void TerminalSettings::setCommands(const QString& commands)
    {
        m_commands = commands;
    }
    void TerminalSettings::setNeighbor(int index)
    {
        m_neighbor = index;
    }
    void TerminalSettings::setSplitWay(int split)
    {
        m_split = split;
    }
}
