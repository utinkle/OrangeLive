/*
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PLAYLISTSERVICE_H
#define PLAYLISTSERVICE_H

#include <QObject>
#include <QVariantMap>
#include <qqmlintegration.h>

class QNetworkAccessManager;
class QSettings;

class PlaylistService : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantList channels READ channels NOTIFY channelsChanged)
    Q_PROPERTY(QVariantList filteredChannels READ filteredChannels NOTIFY filteredChannelsChanged)
    Q_PROPERTY(QStringList groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(QString epgUrl READ epgUrl NOTIFY epgUrlChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)
    Q_PROPERTY(QString currentGroup READ currentGroup WRITE setCurrentGroup NOTIFY currentGroupChanged)
    Q_PROPERTY(QString searchKeyword READ searchKeyword WRITE setSearchKeyword NOTIFY searchKeywordChanged)
    Q_PROPERTY(QStringList favoriteUrls READ favoriteUrls NOTIFY favoriteUrlsChanged)
    Q_PROPERTY(QVariantList recentChannels READ recentChannels NOTIFY recentChannelsChanged)

public:
    explicit PlaylistService(QObject *parent = nullptr);

    Q_INVOKABLE void load(const QString &source);
    Q_INVOKABLE QVariantList channelsByGroup(const QString &group) const;
    Q_INVOKABLE void toggleFavorite(const QString &url);
    Q_INVOKABLE bool isFavorite(const QString &url) const;
    Q_INVOKABLE void addRecent(const QString &url);

    QVariantList channels() const;
    QVariantList filteredChannels() const;
    QStringList groups() const;
    QString epgUrl() const;
    bool loading() const;
    QString errorString() const;
    QString currentGroup() const;
    void setCurrentGroup(const QString &group);
    QString searchKeyword() const;
    void setSearchKeyword(const QString &keyword);
    QStringList favoriteUrls() const;
    QVariantList recentChannels() const;

Q_SIGNALS:
    void channelsChanged();
    void filteredChannelsChanged();
    void groupsChanged();
    void epgUrlChanged();
    void loadingChanged();
    void errorStringChanged();
    void currentGroupChanged();
    void searchKeywordChanged();
    void favoriteUrlsChanged();
    void recentChannelsChanged();

private:
    void setLoading(bool loading);
    void setErrorString(const QString &error);
    void parsePlaylistText(const QString &playlistText);
    void updateFilteredChannels();
    void updateRecentChannels();
    void persistPreferences();
    QVariantMap findChannelByUrl(const QString &url) const;
    static QVariantMap parseExtinf(const QString &line);
    static QStringList collectGroups(const QVariantList &channels);

    QNetworkAccessManager *m_networkAccessManager{nullptr};
    QSettings *m_settings{nullptr};
    QVariantList m_channels;
    QVariantList m_filteredChannels;
    QStringList m_groups;
    QString m_epgUrl;
    bool m_loading{false};
    QString m_errorString;
    QString m_currentGroup;
    QString m_searchKeyword;
    QStringList m_favoriteUrls;
    QStringList m_recentUrls;
    QVariantList m_recentChannels;
};

#endif // PLAYLISTSERVICE_H
