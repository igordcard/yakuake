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

#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include "sessionsettings.h"
#include "terminalsettings.h"

#include <QString>


class ConnectionSettings
{
    public:
        explicit ConnectionSettings();
        ~ConnectionSettings();

        bool isValid();

        QString name() const;

        Yakuake::TerminalSettings terminal() const { return m_terminal; }
        void setTerminal(Yakuake::TerminalSettings terminal) { m_terminal = terminal; }

        Yakuake::SessionSettingsPtr session() const { return m_session; }
        void setSession(Yakuake::SessionSettingsPtr session) { m_session = session; }

    private:
        Yakuake::TerminalSettings m_terminal;
        Yakuake::SessionSettingsPtr m_session;

};


#endif
