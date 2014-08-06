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

#include "common.h"
#include "application.h"
#include "preferences.h"

#include <QString>
#include <QRegExp>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <KLocalizedString>

namespace Yakuake
{
    /// Replace invalid codepoints so the string can be converted to Utf8.
    /// @param s a const reference to the QString to copy and change
    /// @retval s new QString
    QString sterilizeUnicode(const QString& s)
    {
        QString copy(s);
        sterilizeUnicode(copy);
        return copy;
    }

    /// Replace invalid codepoints so the string can be converted to Utf8.
    /// @param s a reference to the QString to change, a reference so it works with m_inputbuffer.back() in ?.cpp
    /// @retval s reference to the argument
    QString& sterilizeUnicode(QString& s)
    {
        // HACK work around undocumented requirement to vet Unicode text sent over DBUS.
        // strips noncharacters, the private use characters are presumably safe
        for (int i = 0; i < s.length(); ++i)
        {
            QChar c(s.at(i));
            if (c.category() == QChar::Other_Surrogate)
            {
                if (!c.isHighSurrogate() || (!(i+1 < s.length()) && !s.at(i+1).isLowSurrogate()))
                {
                    //stomp on this bad char now, next trip through the loop (if there is one) eats the other
                    s.replace(i, 1, QChar(0xFFFD));
                    continue;
                }

                QChar next = s.at(i+1);
                if ((next.unicode()&0x3FE) == 0x3FE && (c.unicode()&0x3F) == 0x3F)
                {
                    s.replace(i, 1, QChar(0xFFFD)); //its one of the last two of the plane, replace them
                    s.replace(i+1, 1, QChar(0xFFFD));
                }

                ++i; // skip the high surrogate now, the loop takes care of the low
            }
            else if ((c.category() == QChar::Other_NotAssigned) //perhaps Qt will use QChar::Other_NotAssigned some day
                || (c.unicode() >= 0xFDD0 && c.unicode() <= 0xFDEF) //Unicode class Cn on BMP only
                || (c.unicode() == 0xFFFE || (c.unicode() == 0xFFFF)) //Unicode class Cn on all planes
                )
            {
                s.replace(i, 1, QChar(0xFFFD));
            }
        }
        return s;
    }

    /// Run a QStringList through sterilizeUnicode
    /// @param list a reference to the list
    /// @retval list
    QStringList& sterilizeUnicode(QStringList& list)
    {
        for (int i = 0; i < list.count(); ++i)
            sterilizeUnicode(list[i]);

        return list;
    }

    /// Copy the list argument and return it, filtered
    QStringList sterilizeUnicode(const QStringList& list)
    {
        QStringList out(list);
        return sterilizeUnicode(out);
    }

}
