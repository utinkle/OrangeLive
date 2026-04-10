/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#include "playlistservice.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QSet>
#include <QSettings>
#include <QUrl>

PlaylistService::PlaylistService(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(new QNetworkAccessManager(this))
    , m_settings(new QSettings(QStringLiteral("MpvQt"), QStringLiteral("multiple-video-players"), this))
{
    m_favoriteUrls = m_settings->value(QStringLiteral("favorites")).toStringList();
    m_recentUrls = m_settings->value(QStringLiteral("recent")).toStringList();
}

void PlaylistService::load(const QString &source)
{
    const QUrl url = QUrl::fromUserInput(source);
    setErrorString(QString());

    if (url.isValid() && (url.scheme().startsWith(QStringLiteral("http")))) {
        setLoading(true);
        QNetworkRequest request(url);
        auto *reply = m_networkAccessManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            reply->deleteLater();
            setLoading(false);

            if (reply->error() != QNetworkReply::NoError) {
                setErrorString(reply->errorString());
                return;
            }

            parsePlaylistText(QString::fromUtf8(reply->readAll()));
        });
        return;
    }

    QString path = source;
    if (url.isLocalFile()) {
        path = url.toLocalFile();
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setErrorString(tr("无法打开播放列表文件: %1").arg(path));
        return;
    }

    parsePlaylistText(QString::fromUtf8(file.readAll()));
}

QVariantList PlaylistService::channelsByGroup(const QString &group) const
{
    if (group.isEmpty()) {
        return m_channels;
    }

    QVariantList filtered;
    for (const auto &channelVariant : m_channels) {
        const QVariantMap channel = channelVariant.toMap();
        if (channel.value(QStringLiteral("group")).toString() == group) {
            filtered << channel;
        }
    }
    return filtered;
}

void PlaylistService::toggleFavorite(const QString &url)
{
    if (url.isEmpty()) {
        return;
    }

    if (m_favoriteUrls.contains(url)) {
        m_favoriteUrls.removeAll(url);
    } else {
        m_favoriteUrls.prepend(url);
    }

    persistPreferences();
    Q_EMIT favoriteUrlsChanged();
    updateRecentChannels();
}

bool PlaylistService::isFavorite(const QString &url) const
{
    return m_favoriteUrls.contains(url);
}

void PlaylistService::addRecent(const QString &url)
{
    if (url.isEmpty()) {
        return;
    }

    m_recentUrls.removeAll(url);
    m_recentUrls.prepend(url);
    constexpr int maxRecent = 30;
    while (m_recentUrls.size() > maxRecent) {
        m_recentUrls.removeLast();
    }

    persistPreferences();
    updateRecentChannels();
}

QVariantList PlaylistService::channels() const
{
    return m_channels;
}

QVariantList PlaylistService::filteredChannels() const
{
    return m_filteredChannels;
}

QStringList PlaylistService::groups() const
{
    return m_groups;
}

QString PlaylistService::epgUrl() const
{
    return m_epgUrl;
}

bool PlaylistService::loading() const
{
    return m_loading;
}

QString PlaylistService::errorString() const
{
    return m_errorString;
}

QString PlaylistService::currentGroup() const
{
    return m_currentGroup;
}

void PlaylistService::setCurrentGroup(const QString &group)
{
    if (m_currentGroup == group) {
        return;
    }
    m_currentGroup = group;
    updateFilteredChannels();
    Q_EMIT currentGroupChanged();
}

QString PlaylistService::searchKeyword() const
{
    return m_searchKeyword;
}

void PlaylistService::setSearchKeyword(const QString &keyword)
{
    if (m_searchKeyword == keyword) {
        return;
    }
    m_searchKeyword = keyword;
    updateFilteredChannels();
    Q_EMIT searchKeywordChanged();
}

QStringList PlaylistService::favoriteUrls() const
{
    return m_favoriteUrls;
}

QVariantList PlaylistService::recentChannels() const
{
    return m_recentChannels;
}

void PlaylistService::setLoading(bool loading)
{
    if (m_loading == loading) {
        return;
    }
    m_loading = loading;
    Q_EMIT loadingChanged();
}

void PlaylistService::setErrorString(const QString &error)
{
    if (m_errorString == error) {
        return;
    }
    m_errorString = error;
    Q_EMIT errorStringChanged();
}

void PlaylistService::parsePlaylistText(const QString &playlistText)
{
    QVariantList parsedChannels;
    QString pendingExtinf;
    QString parsedEpgUrl;

    const QStringList lines = playlistText.split(QRegularExpression(QStringLiteral("\\r?\\n")));
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line.startsWith(QStringLiteral("#EXTM3U"), Qt::CaseInsensitive)) {
            static const QRegularExpression epgRegex(QStringLiteral("x-tvg-url\\s*=\\s*\"([^\"]+)\""), QRegularExpression::CaseInsensitiveOption);
            const auto match = epgRegex.match(line);
            if (match.hasMatch()) {
                parsedEpgUrl = match.captured(1);
            }
            continue;
        }

        if (line.startsWith(QStringLiteral("#EXTINF"), Qt::CaseInsensitive)) {
            pendingExtinf = line;
            continue;
        }

        if (line.startsWith(QLatin1Char('#'))) {
            continue;
        }

        QVariantMap channel;
        if (!pendingExtinf.isEmpty()) {
            channel = parseExtinf(pendingExtinf);
        }

        channel.insert(QStringLiteral("url"), line);
        if (!channel.contains(QStringLiteral("title")) || channel.value(QStringLiteral("title")).toString().isEmpty()) {
            channel.insert(QStringLiteral("title"), QUrl(line).fileName());
        }
        channel.insert(QStringLiteral("favorite"), m_favoriteUrls.contains(line));

        const QString stream = line.toLower();
        const bool isLive = stream.startsWith(QStringLiteral("rtsp://"))
            || stream.contains(QStringLiteral(".flv"))
            || stream.contains(QStringLiteral("live"));
        channel.insert(QStringLiteral("isLive"), isLive);

        parsedChannels << channel;
        pendingExtinf.clear();
    }

    m_channels = parsedChannels;
    m_groups = collectGroups(parsedChannels);
    if (m_epgUrl != parsedEpgUrl) {
        m_epgUrl = parsedEpgUrl;
        Q_EMIT epgUrlChanged();
    }

    updateFilteredChannels();
    updateRecentChannels();
    Q_EMIT channelsChanged();
    Q_EMIT groupsChanged();
}

void PlaylistService::updateFilteredChannels()
{
    QVariantList filtered;
    const QString keyword = m_searchKeyword.trimmed();

    for (const auto &channelVariant : m_channels) {
        QVariantMap channel = channelVariant.toMap();

        if (!m_currentGroup.isEmpty() && channel.value(QStringLiteral("group")).toString() != m_currentGroup) {
            continue;
        }

        if (!keyword.isEmpty()) {
            const QString title = channel.value(QStringLiteral("title")).toString();
            const QString name = channel.value(QStringLiteral("name")).toString();
            const QString id = channel.value(QStringLiteral("id")).toString();
            if (!title.contains(keyword, Qt::CaseInsensitive)
                && !name.contains(keyword, Qt::CaseInsensitive)
                && !id.contains(keyword, Qt::CaseInsensitive)) {
                continue;
            }
        }

        channel.insert(QStringLiteral("favorite"), m_favoriteUrls.contains(channel.value(QStringLiteral("url")).toString()));
        filtered << channel;
    }

    m_filteredChannels = filtered;
    Q_EMIT filteredChannelsChanged();
}

void PlaylistService::updateRecentChannels()
{
    QVariantList recent;
    for (const QString &recentUrl : m_recentUrls) {
        QVariantMap channel = findChannelByUrl(recentUrl);
        if (!channel.isEmpty()) {
            channel.insert(QStringLiteral("favorite"), m_favoriteUrls.contains(recentUrl));
            recent << channel;
        }
    }

    m_recentChannels = recent;
    Q_EMIT recentChannelsChanged();
}

void PlaylistService::persistPreferences()
{
    m_settings->setValue(QStringLiteral("favorites"), m_favoriteUrls);
    m_settings->setValue(QStringLiteral("recent"), m_recentUrls);
}

QVariantMap PlaylistService::findChannelByUrl(const QString &url) const
{
    for (const auto &channelVariant : m_channels) {
        const QVariantMap channel = channelVariant.toMap();
        if (channel.value(QStringLiteral("url")).toString() == url) {
            return channel;
        }
    }

    QVariantMap fallback;
    fallback.insert(QStringLiteral("url"), url);
    fallback.insert(QStringLiteral("title"), url);
    fallback.insert(QStringLiteral("group"), QStringLiteral("最近播放"));
    fallback.insert(QStringLiteral("isLive"), true);
    return fallback;
}

QVariantMap PlaylistService::parseExtinf(const QString &line)
{
    QVariantMap channel;

    const int commaIndex = line.lastIndexOf(QLatin1Char(','));
    if (commaIndex > -1 && commaIndex + 1 < line.size()) {
        channel.insert(QStringLiteral("title"), line.mid(commaIndex + 1).trimmed());
    }

    static const QRegularExpression attrRegex(QStringLiteral("([a-zA-Z0-9\\-]+)=\"([^\"]*)\""));
    auto it = attrRegex.globalMatch(line);
    while (it.hasNext()) {
        const auto match = it.next();
        const QString key = match.captured(1).trimmed();
        const QString value = match.captured(2).trimmed();

        if (key == QStringLiteral("tvg-id")) {
            channel.insert(QStringLiteral("id"), value);
        } else if (key == QStringLiteral("tvg-name")) {
            channel.insert(QStringLiteral("name"), value);
        } else if (key == QStringLiteral("tvg-logo")) {
            channel.insert(QStringLiteral("logo"), value);
        } else if (key == QStringLiteral("group-title")) {
            channel.insert(QStringLiteral("group"), value);
        } else {
            channel.insert(key, value);
        }
    }

    if (!channel.contains(QStringLiteral("name")) && channel.contains(QStringLiteral("title"))) {
        channel.insert(QStringLiteral("name"), channel.value(QStringLiteral("title")));
    }

    return channel;
}

QStringList PlaylistService::collectGroups(const QVariantList &channels)
{
    QSet<QString> uniqueGroups;
    for (const auto &channelVariant : channels) {
        const QString group = channelVariant.toMap().value(QStringLiteral("group")).toString();
        if (!group.isEmpty()) {
            uniqueGroups.insert(group);
        }
    }

    QStringList groups = uniqueGroups.values();
    groups.sort(Qt::CaseInsensitive);
    return groups;
}

#include "moc_playlistservice.cpp"
