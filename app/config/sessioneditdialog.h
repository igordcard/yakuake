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

#ifndef SESSIONEDITDIALOG_H
#define SESSIONEDITDIALOG_H

#include "sessionsettings.h"

#include <kdialog.h>


namespace Ui
{
    class SessionEditDialog;
    class TerminalEditDialog;
}

namespace Yakuake
{
    class SessionEditDialog : public KDialog
    {
        Q_OBJECT
        public:
            explicit SessionEditDialog(const QString& title, QWidget* parent = 0);
            ~SessionEditDialog();

            void setSessionSettingsPtr(SessionSettingsPtr settings);
            SessionSettingsPtr sessionSettings();

            TerminalSettings editedTerminal();

            int execAndEditTerminal(TerminalSettings terminal);

        public slots:
            virtual void accept();

        protected slots:
            void addTerminal();
            void editTerminal();
            void editTerminal(TerminalSettings terminal);
            void deleteTerminal();
            void updateTerminalArrows();
            void moveTerminalUp();
            void moveTerminalDown();

        private:
            Ui::SessionEditDialog* m_mainWidget;
            bool m_expanded;
            bool m_enableNotifications;
            bool m_configBacked;
            int m_id;
            int m_sortIndex;

            bool m_editedTerminal;
            int m_editedTerminalIndex;
            TerminalList m_terminalList;
    };

    class TerminalEditDialog : public KDialog
    {
        Q_OBJECT
        public:
            explicit TerminalEditDialog(const QString& title, QWidget *parent = 0);
            ~TerminalEditDialog();

            void setTerminalSettings(const TerminalList& terminals, int index);
            TerminalSettings terminalSettings();

        protected slots:
            void slotOk();
            void slotTerminalNameChanged( const QString& );
        private:
            Ui::TerminalEditDialog* m_mainWidget;
    };
}
#endif
