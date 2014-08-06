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

#include "config/preferences.h"

#include <QFileInfo>
#include <QHashIterator>
#include <QHeaderView>
#include <QTreeView>

#include <KLocale>
#include <KUser>
#include <KStandardDirs>

struct PreferencesSingleton
{
    Preferences prefs;
};

K_GLOBAL_STATIC(PreferencesSingleton, s_prefs)

Preferences *Preferences::self()
{
  if ( !s_prefs.exists() ) {
    s_prefs->prefs.readConfig();
  }

  return &s_prefs->prefs;
}

Preferences::Preferences()
{
    Yakuake::SessionSettingsPtr session(new Yakuake::SessionSettings);
    session->setName("My Session");
    Yakuake::TerminalSettings terminal;
    terminal.setName("My Shell");
    session->addTerminal(terminal);
    mSessionHash.insert(0, session);
}

Preferences::~Preferences()
{
}

const Yakuake::SessionHash Preferences::sessionHash()
{
    return self()->mSessionHash;
}

void Preferences::setSessionHash(const Yakuake::SessionHash& hash)
{
    self()->mSessionHash.clear();
    self()->mSessionHash = hash;
}

void Preferences::addSession(Yakuake::SessionSettingsPtr session)
{
    Yakuake::SessionHash hash = self()->mSessionHash;
    hash.insert(session->id(), session);
    self()->mSessionHash = hash;
}

const Yakuake::SessionSettingsPtr Preferences::sessionById(int id)
{
    return  self()->mSessionHash.value(id);
}

const QList<Yakuake::SessionSettingsPtr> Preferences::sessionsByTerminal(const QString& terminal)
{
    QList<Yakuake::SessionSettingsPtr> sessions;
    QHashIterator<int, Yakuake::SessionSettingsPtr> it(self()->mSessionHash);
    while(it.hasNext())
    {
        it.next();
        for (int i=0; i != it.value()->terminalList().count(); i++)
        {
            if(it.value()->terminalByIndex(i).name().toLower() == terminal)
                sessions.append(it.value());
        }
    }
    return sessions;
}

QList<int> Preferences::sessionIdsByName(const QString& session)
{
    QList<int> terminalIds;
    QHashIterator<int, Yakuake::SessionSettingsPtr> it(self()->mSessionHash);
    while(it.hasNext())
    {
        if(it.next().value()->name().toLower() == session.toLower())
            terminalIds.append(it.key());
    }
    if (terminalIds.isEmpty())
        terminalIds.append(-1);

    return terminalIds;
}

bool Preferences::isSession(const QString& terminal)
{
    QHashIterator<int, Yakuake::SessionSettingsPtr> it(self()->mSessionHash);
    while(it.hasNext())
    {
        if(it.next().value()->name().toLower() == terminal.toLower())
            return true;
    }
    return false;
}

void Preferences::removeSession(int id)
{
    self()->mSessionHash.remove(id);
}
