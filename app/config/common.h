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

#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>
#include <QHash>
#include <QStringList>

class QString;
class QPixmap;

namespace Yakuake
{
    static QRegExp ircMarkupsRegExp("[\\0000-\\0037]");
    static QRegExp colorRegExp("((\003([0-9]|0[0-9]|1[0-5])(,([0-9]|0[0-9]|1[0-5])|)|\017)|\x02|\x03|\x09|\x13|\x15|\x16|\x1d|\x1f)");
    static QRegExp colorOnlyRegExp("(\003([0-9]|0[0-9]|1[0-5]|)(,([0-9]|0[0-9]|1[0-5])|,|)|\017)");
    static QRegExp urlPattern(QString("\\b((?:(?:([a-z][\\w\\.-]+:/{1,3})|www\\d{0,3}[.]|[a-z0-9.\\-]+[.][a-z]{2,4}/)(?:[^\\s()<>]+|\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\))+(?:\\(([^\\s()<>]+|(\\([^\\s()<>]+\\)))*\\)|\\}\\]|[^\\s`!()\\[\\]{};:'\".,<>?%1%2%3%4%5%6])|[a-z0-9.\\-+_]+@[a-z0-9.\\-]+[.][a-z]{1,5}[^\\s/`!()\\[\\]{};:'\".,<>?%1%2%3%4%5%6]))").arg(QChar(0x00AB)).arg(QChar(0x00BB)).arg(QChar(0x201C)).arg(QChar(0x201D)).arg(QChar(0x2018)).arg(QChar(0x2019)));
    static QRegExp chanExp(QString("(^|\\s|^\"|\\s\"|,|'|\\(|\\:|!|@|%|\\+)(#[^,\\s;\\)\\:\\/\\(\\<\\>]*[^.,\\s;\\)\\:\\/\\(\"\''\\<\\>?%1%2%3%4%5%6])").arg(QChar(0x00AB)).arg(QChar(0x00BB)).arg(QChar(0x201C)).arg(QChar(0x201D)).arg(QChar(0x2018)).arg(QChar(0x2019)));

    enum TabNotifyType
    {
        tnfNick,
        tnfHighlight,
        tnfPrivate,
        tnfNormal,
        tnfSystem,
        tnfControl,
        tnfNone
    };

    enum ConnectionState
    {
        SSNeverConnected,
        SSDeliberatelyDisconnected,
        SSInvoluntarilyDisconnected,
        SSScheduledToConnect,
        SSConnecting,
        SSConnected
    };

    enum ConnectionFlag
    {
        SilentlyReuseConnection,
        PromptToReuseConnection,
        CreateNewConnection
    };

    struct TextUrlData
    {
        QList<QPair<int, int> > urlRanges;
        QStringList fixedUrls;
    };

    struct TextChannelData
    {
        QList<QPair<int, int> > channelRanges;
        QStringList fixedChannels;
    };

    QString removeIrcMarkup(const QString& text);
    QString doVarExpansion(const QString& text);
    QString replaceFormattingCodes(const QString& text);
    QString replaceIRCMarkups(const QString& text);
    inline bool hasIRCMarkups(const QString& text)
    {
        return text.contains(ircMarkupsRegExp);
    }

    QList<QPair<int, int> > getUrlRanges(const QString& text);
    QList<QPair<int, int> > getChannelRanges(const QString& text);
    TextUrlData extractUrlData(const QString& string, bool doUrlFixup = true);
    TextChannelData extractChannelData(const QString& text, bool doChannelFixup = true);
    bool isUrl(const QString& text);
    QString extractColorCodes(const QString& text);

    QPixmap overlayPixmaps(const QPixmap &under, const QPixmap &over);
    bool isUtf8(const QByteArray& text);
    uint colorForNick(const QString& nickname);

    static QHash<QChar,QString> m_modesHash;
    QHash<QChar,QString> getChannelModesHash();

    QString sterilizeUnicode(const QString& s);
    QString& sterilizeUnicode(QString& s);
    QStringList& sterilizeUnicode(QStringList& list);
    QStringList sterilizeUnicode(const QStringList& inVal);
}
#endif
