/*
  Copyright (C) 2008-2009 by Eike Hein <hein@kde.org>
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


#include "application.h"
#include "mainwindow.h"


Application::Application() : KUniqueApplication()
{
    m_mainWindow = 0;
}

Application::~Application()
{
    saveOptions(false);
}

int Application::newInstance()
{
    if (!m_mainWindow)
    {
        Preferences::self();
        readOptions();

        m_mainWindow = new MainWindow();

        connect(this, SIGNAL(sessionsChanged(Yakuake::SessionSettingsPtr)), this, SLOT(saveOptions()));

        m_mainWindow->hide();
    }
    else
        m_mainWindow->toggleWindowState();

    return 0;
}

void Application::readOptions()
{
    QStringList groups = KGlobal::config()->groupList().filter(QRegExp("Session [0-9]+"));
    QList<int> sgKeys;

    if(!groups.isEmpty())
    {
        Yakuake::SessionHash sessions;
        QStringList::iterator it;
        QStringList tmp1;
        QStringList::iterator it2;
        int index = 0;
        Yakuake::TerminalSettings terminal;

        for (it = groups.begin(); it != groups.end(); ++it)
        {
            KConfigGroup cgSession(KGlobal::config()->group(*it));
            Yakuake::SessionSettingsPtr session(new Yakuake::SessionSettings);
            session->setName(cgSession.readEntry("Name"));
            session->setSortIndex(index);
            session->setExpanded(cgSession.readEntry("Expanded", false));

            tmp1 = cgSession.readEntry("Terminals", QStringList());
            for (it2 = tmp1.begin(); it2 != tmp1.end(); ++it2)
            {
                KConfigGroup cgTerminal(KGlobal::config()->group(*it2));
                terminal.setName(cgTerminal.readEntry("Name"));
                terminal.setCommands(cgTerminal.readEntry("Commands"));
                terminal.setNeighbor(cgTerminal.readEntry("Neighbor", 0));
                terminal.setSplitWay(cgTerminal.readEntry("Split", 0));
//TODO                terminal.setProfile(cgTerminal.readEntry("Profile"));
                session->addTerminal(terminal);
            }

            sessions.insert(session->id(), session);
            sgKeys.append(session->id());

            index++;
        }

        Preferences::setSessionHash(sessions);
    }
}

void Application::saveOptions(bool updateGUI)
{
    // Remove the old sessions from the config
    QStringList groups = KGlobal::config()->groupList().filter(QRegExp("Session [0-9]+"));
    if (groups.count())
    {
        QStringList::iterator it;
        for(it = groups.begin(); it != groups.end(); ++it)
        {
            KGlobal::config()->deleteGroup((*it));
        }
    }

    // Remove the old terminals from the config
    groups = KGlobal::config()->groupList().filter(QRegExp("Terminal [0-9]+"));
    if (groups.count())
    {
        QStringList::iterator it;
        for(it = groups.begin(); it != groups.end(); ++it)
        {
            KGlobal::config()->deleteGroup((*it));
        }
    }

    // Add the new sessions to the config
    Yakuake::SessionHash sessionHash = Preferences::sessionHash();
    QHashIterator<int, Yakuake::SessionSettingsPtr> hashIt(sessionHash);

    QMap<int, Yakuake::SessionSettingsPtr> sortedSessionMap;

    // Make the indices in the group headers reflect the terminal list dialog sorting.
    while (hashIt.hasNext())
    {
        hashIt.next();

        sortedSessionMap.insert(hashIt.value()->sortIndex(), hashIt.value());
    }

    QMapIterator<int, Yakuake::SessionSettingsPtr> it(sortedSessionMap);

    int index = 0;
    int index2 = 0;
    int width = 0;
    QList<int> keys = sessionHash.keys();
    for(int i=0; i<keys.count(); i++)
        if(width < keys.at(i)) width = keys.at(i);
    width = QString(width).length();
    QString groupName;
    QStringList terminals;
    Yakuake::TerminalList::iterator it2;
    Yakuake::TerminalList terminalList;
    QList<int> sgKeys;

    while(it.hasNext())
    {
        it.next();
        terminalList = (it.value())->terminalList();
        terminals.clear();

        sgKeys.append(it.value()->id());

        for(it2 = terminalList.begin(); it2 != terminalList.end(); ++it2)
        {
            groupName = QString("Terminal %1").arg(index2);
            terminals.append(groupName);
            KConfigGroup cgTerminal(KGlobal::config()->group(groupName));
            cgTerminal.writeEntry("Name", (*it2).name());
            cgTerminal.writeEntry("Commands", (*it2).commands());
            cgTerminal.writeEntry("Neighbor", (*it2).neighbor());
            cgTerminal.writeEntry("Split", (*it2).splitWay());
//TODO            cgTerminal.writeEntry("Profile", (*it2).profile());
            index2++;
        }

        QString sgn = QString("Session %1").arg(QString::number(index).rightJustified(width,'0'));
        KConfigGroup cgSession(KGlobal::config()->group(sgn));
        cgSession.writeEntry("Name", it.value()->name());
        cgSession.writeEntry("Terminals", terminals);
        cgSession.writeEntry("Expanded", it.value()->expanded());
        index++;
    }

    KGlobal::config()->sync();

    if(updateGUI)
        emit appearanceChanged();
}
