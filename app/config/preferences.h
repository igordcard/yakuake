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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "sessionsettings.h"
#include "settings.h"

class Ignore;
class Highlight;
struct PreferencesSingleton;

class QTreeView;

class Preferences : public Settings
{
    Q_OBJECT

    friend struct PreferencesSingleton;

    protected:
        Preferences();

    public:

        static Preferences *self();
        ~Preferences();
        enum Pages
        {
            NotifyPage,
            ChatWinAppearancePage
        };
        static const Yakuake::SessionHash sessionHash();
        static void setSessionHash(const Yakuake::SessionHash& hash);
        static void addSession(Yakuake::SessionSettingsPtr session);
        static const Yakuake::SessionSettingsPtr sessionById(int id);
        static const QList<Yakuake::SessionSettingsPtr> sessionsByTerminal(const QString& terminal);
        static QList<int> sessionIdsByName(const QString& session);
        static bool isSession(const QString& terminal);
        static void removeSession(int id);

        static void saveColumnState(QTreeView *treeView, QString name);
        static void restoreColumnState(QTreeView *treeView, QString name, int defaultColumn = 0,
                                       Qt::SortOrder defaultSortOrder = Qt::AscendingOrder);

    public slots:

    signals:
        void notifyListStarted(int sessionId);
        void updateTrayIcon();

    protected:
        Yakuake::SessionHash mSessionHash;
        QString mSortingOrder;
};
#endif
