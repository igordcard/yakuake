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

#include "sessioneditdialog.h"
#include "application.h"
#include "ui_sessioneditdialog.h"
#include "ui_terminaleditdialog.h"

#include <QPushButton>
#include <QCheckBox>

#include <KMessageBox>
#include <KPushButton>


namespace Yakuake
{

    SessionEditDialog::SessionEditDialog(const QString& title, QWidget *parent)
        : KDialog(parent)
    {
        setCaption(title);
        setButtons(Ok|Cancel);

        m_id = -1;
        m_editedTerminal = false;

        m_mainWidget = new Ui::SessionEditDialog();
        m_mainWidget->setupUi(mainWidget());
        mainWidget()->layout()->setMargin(0);
        m_mainWidget->terminalWidget->layout()->setMargin(0);

        m_mainWidget->m_removeTerminalButton->setIcon(KIcon("list-remove"));
        m_mainWidget->m_upTerminalButton->setIcon(KIcon("arrow-up"));
        m_mainWidget->m_downTerminalButton->setIcon(KIcon("arrow-down"));

        connect(m_mainWidget->m_addTerminalButton, SIGNAL(clicked()), this, SLOT(addTerminal()));
        connect(m_mainWidget->m_changeTerminalButton, SIGNAL(clicked()), this, SLOT(editTerminal()));
        connect(m_mainWidget->m_removeTerminalButton, SIGNAL(clicked()), this, SLOT(deleteTerminal()));
        connect(m_mainWidget->m_terminalLBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateTerminalArrows()));
        connect(m_mainWidget->m_upTerminalButton, SIGNAL(clicked()), this, SLOT(moveTerminalUp()));
        connect(m_mainWidget->m_downTerminalButton, SIGNAL(clicked()), this, SLOT(moveTerminalDown()));

        setButtonGuiItem(Ok, KGuiItem(i18n("&OK"), "dialog-ok", i18n("Change session configuration")));
        setButtonGuiItem(Cancel, KGuiItem(i18n("&Cancel"), "dialog-cancel", i18n("Discards all changes made")));

        m_mainWidget->m_sessionName->setFocus();

        setInitialSize(QSize(320, 400));
    }

    SessionEditDialog::~SessionEditDialog()
    {
        delete m_mainWidget;
    }

    void SessionEditDialog::setSessionSettingsPtr(SessionSettingsPtr settings)
    {
        m_id = settings->id();
        m_sortIndex = settings->sortIndex();
        m_expanded = settings->expanded();
        m_mainWidget->m_sessionName->setText(settings->name());

        m_terminalList = settings->terminalList();
        TerminalList::iterator it;
        m_mainWidget->m_terminalLBox->clear();

        for(it = m_terminalList.begin(); it != m_terminalList.end(); ++it)
        {
            m_mainWidget->m_terminalLBox->addItem((*it).name());
        }
    }

    SessionSettingsPtr SessionEditDialog::sessionSettings()
    {
        SessionSettingsPtr settings(new SessionSettings(m_id));
        settings->setSortIndex(m_sortIndex);
        settings->setName(m_mainWidget->m_sessionName->text());
        settings->setTerminalList(m_terminalList);
        settings->setExpanded(m_expanded);

        return settings;
    }

    TerminalSettings SessionEditDialog::editedTerminal()
    {
        if (m_editedTerminal && m_editedTerminalIndex < m_terminalList.count())
        {
            return m_terminalList[m_editedTerminalIndex];
        }

        return TerminalSettings("");
    }

    int SessionEditDialog::execAndEditTerminal(TerminalSettings terminal)
    {
        show();
        editTerminal(terminal);
        return exec();
    }

    void SessionEditDialog::addTerminal()
    {
        QPointer<TerminalEditDialog> dlg = new TerminalEditDialog(i18n("Add Terminal"), this);
        dlg->setTerminalNeighbors(m_terminalList);

        if(dlg->exec() == KDialog::Accepted)
        {
            TerminalSettings terminal = dlg->terminalSettings();
            m_mainWidget->m_terminalLBox->addItem(terminal.name());
            m_mainWidget->m_terminalLBox->setCurrentItem(m_mainWidget->m_terminalLBox->item(m_mainWidget->m_terminalLBox->count() - 1));
            m_terminalList.append(terminal);
        }
        delete dlg;
    }

    void SessionEditDialog::editTerminal()
    {
        int current = m_mainWidget->m_terminalLBox->currentRow();

        if(current < m_terminalList.count())
        {
            QPointer <TerminalEditDialog> dlg = new TerminalEditDialog(i18n("Edit Terminal"), this);
            dlg->setTerminalSettings(m_terminalList, current);

            if(dlg->exec() == KDialog::Accepted)
            {
                TerminalSettings terminal = dlg->terminalSettings();
                m_mainWidget->m_terminalLBox->item(current)->setText(terminal.name());
                m_terminalList[current] = terminal;
            }
            delete dlg;
        }
    }

    void SessionEditDialog::editTerminal(TerminalSettings terminal)
    {
        // Track the terminal the Terminal List dialog told us to edit
        // and find out which terminal to select in the listbox
        m_editedTerminal = true;
        m_editedTerminalIndex = m_terminalList.indexOf(terminal);
        m_mainWidget->m_terminalLBox->setCurrentRow(m_editedTerminalIndex);

        editTerminal();
    }

    void SessionEditDialog::deleteTerminal()
    {
        int current = m_mainWidget->m_terminalLBox->currentRow();

        if (current < m_terminalList.count())
        {
            m_terminalList.removeAt(current);
            delete m_mainWidget->m_terminalLBox->takeItem(current);

            // Track the terminal the Terminal List dialog told us to edit
            if (m_editedTerminal && m_editedTerminalIndex==current)
                m_editedTerminal = false;
        }

        updateTerminalArrows();
    }

    void SessionEditDialog::updateTerminalArrows()
    {
        QList<QListWidgetItem*> terminalBoxSelection = m_mainWidget->m_terminalLBox->selectedItems();

        if (m_mainWidget->m_terminalLBox->count() && terminalBoxSelection.count())
        {
            QListWidgetItem* selectedTerminal = terminalBoxSelection.first();
            int selectedTerminalRow = m_mainWidget->m_terminalLBox->row(selectedTerminal);

            m_mainWidget->m_upTerminalButton->setEnabled(selectedTerminalRow > 0);
            m_mainWidget->m_downTerminalButton->setEnabled(selectedTerminalRow < m_mainWidget->m_terminalLBox->count() - 1);
        }
        else
        {
            m_mainWidget->m_upTerminalButton->setEnabled(false);
            m_mainWidget->m_downTerminalButton->setEnabled(false);
        }

        m_mainWidget->m_removeTerminalButton->setEnabled(terminalBoxSelection.count());
        m_mainWidget->m_changeTerminalButton->setEnabled(terminalBoxSelection.count());
    }

    void SessionEditDialog::moveTerminalUp()
    {
        int current = m_mainWidget->m_terminalLBox->currentRow();

        if (current > 0)
        {
            TerminalSettings terminal = m_terminalList[current];
            delete m_mainWidget->m_terminalLBox->takeItem(current);
            m_mainWidget->m_terminalLBox->insertItem(current - 1, terminal.name());
            m_mainWidget->m_terminalLBox->setCurrentRow(current - 1);
            m_terminalList.move(current, current - 1);

            // Track the terminal the Terminal List dialog told us to edit
            if (m_editedTerminal && m_editedTerminalIndex==current)
                m_editedTerminalIndex = current - 1;
        }

        updateTerminalArrows();
    }

    void SessionEditDialog::moveTerminalDown()
    {
        int current = m_mainWidget->m_terminalLBox->currentRow();

        if (current < (m_terminalList.count() - 1))
        {
            TerminalSettings terminal = m_terminalList[current];
            delete m_mainWidget->m_terminalLBox->takeItem(current);
            m_mainWidget->m_terminalLBox->insertItem(current + 1, terminal.name());
            m_mainWidget->m_terminalLBox->setCurrentRow(current + 1);
            m_terminalList.move(current, current + 1);

            // Track the terminal the Terminal List dialog told us to edit
            if (m_editedTerminal && m_editedTerminalIndex==current)
                m_editedTerminalIndex = current + 1;
        }

        updateTerminalArrows();
    }

    void SessionEditDialog::accept()
    {
        if (m_mainWidget->m_sessionName->text().isEmpty())
        {
            KMessageBox::error(this, i18n("The session name is required."));
        }
        else if (m_terminalList.count() == 0)
        {
            KMessageBox::error(this, i18n("You need to add at least one terminal to the session."));
        }
        else
        {
            KDialog::accept();
        }
    }

    // TODO: put this class on its own file.
    TerminalEditDialog::TerminalEditDialog(const QString& title, QWidget *parent)
    : KDialog(parent)
    {
        setCaption(title);
        setButtons(Ok|Cancel);

        m_mainWidget = new Ui::TerminalEditDialog();
        m_mainWidget->setupUi(mainWidget());
        m_mainWidget->m_terminalName->setFocus();

        connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
        connect( m_mainWidget->m_terminalName, SIGNAL(textChanged(QString)),this,SLOT(slotTerminalNameChanged(QString)) );
        slotTerminalNameChanged( m_mainWidget->m_terminalName->text() );
    }

    TerminalEditDialog::~TerminalEditDialog()
    {
    }

    void TerminalEditDialog::slotTerminalNameChanged( const QString &text )
    {
        enableButtonOk( !text.isEmpty() );
    }

    void TerminalEditDialog::setTerminalNeighbors(const TerminalList& terminals, int until)
    {
        int i;
        until = (until < 0) ? terminals.size() : until;
        for(i = 0; i < until; i++)
        {
            m_mainWidget->m_neighbor->addItem(terminals[i].name());
        }
        m_mainWidget->m_split->addItem(i18n("Left->Right"));
        m_mainWidget->m_split->addItem(i18n("Top->Bottom"));
    }

    // From TerminalSettings to UI
    //TODO: Protect against out of range manually hacked indices.
    void TerminalEditDialog::setTerminalSettings(const TerminalList& terminals, int index)
    {
        m_mainWidget->m_terminalName->setText(terminals[index].name());
        m_mainWidget->m_commands->setPlainText(terminals[index].commands());

        setTerminalNeighbors(terminals, index);
        m_mainWidget->m_neighbor->setCurrentIndex(terminals[index].neighbor());

        m_mainWidget->m_split->setCurrentIndex(terminals[index].splitWay());
    }

    // From UI to TerminalSettings
    TerminalSettings TerminalEditDialog::terminalSettings()
    {
        TerminalSettings terminal;
        terminal.setName(m_mainWidget->m_terminalName->text());
        terminal.setCommands(m_mainWidget->m_commands->toPlainText());
        terminal.setNeighbor(m_mainWidget->m_neighbor->currentIndex());
        terminal.setSplitWay(m_mainWidget->m_split->currentIndex());
        return terminal;
    }

    void TerminalEditDialog::slotOk()
    {
        if (m_mainWidget->m_terminalName->text().isEmpty())
        {
            KMessageBox::error(this, i18n("The terminal name is required."));
        }
        else
        {
            accept();
        }
    }
}
