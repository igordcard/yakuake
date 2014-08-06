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

#ifndef TERMINALSETTINGS_H
#define TERMINALSETTINGS_H

#include <QString>

namespace Yakuake
{

    class TerminalSettings
    {
        public:
            TerminalSettings();
            TerminalSettings(const TerminalSettings& settings);
            explicit TerminalSettings(const QString& name);
            ~TerminalSettings();

            void setName(const QString& name);
            QString name() const { return m_name; }

            void setCommands(const QString& commands);
            QString commands() const { return m_commands; }

            void setNeighbor(int index);
            int neighbor() const { return m_neighbor; }

            void setSplitWay(int split);
            int splitWay() const { return m_split; }

            bool operator== (const TerminalSettings& settings) const;

        private:
            QString m_name;
            QString m_commands;
            int m_neighbor;
            int m_split;

    };

}
#endif
