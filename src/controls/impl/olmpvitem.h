/*
 * SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef MPVOBJECT_H
#define MPVOBJECT_H

#include "olmpvabstractitem.h"
#include <qqmlintegration.h>

class MpvRenderer;

class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
    QML_NAMED_ELEMENT(MpvItem)

public:
    explicit MpvItem(QQuickItem *parent = nullptr);
    ~MpvItem();

    enum class AsyncIds {
        None,
        SetVolume,
        GetVolume,
        ExpandText,
    };
    Q_ENUM(AsyncIds)

    Q_PROPERTY(QString mediaTitle READ mediaTitle NOTIFY mediaTitleChanged)
    QString mediaTitle();

    Q_PROPERTY(double position READ position WRITE setPosition NOTIFY positionChanged)
    double position();
    void setPosition(double value);

    Q_PROPERTY(double duration READ duration NOTIFY durationChanged)
    double duration();

    Q_PROPERTY(QString formattedPosition READ formattedPosition NOTIFY positionChanged)
    QString formattedPosition() const;

    Q_PROPERTY(QString formattedDuration READ formattedDuration NOTIFY durationChanged)
    QString formattedDuration() const;

    Q_PROPERTY(bool pause READ pause WRITE setPause NOTIFY pauseChanged)
    bool pause();
    void setPause(bool value);

    Q_INVOKABLE void stop();

    Q_PROPERTY(double playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    double playbackRate() const;
    void setPlaybackRate(double value);

    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    int volume();
    void setVolume(int value);

    Q_PROPERTY(QUrl currentUrl READ currentUrl NOTIFY currentUrlChanged)
    QUrl currentUrl() const;

    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    bool seekable() const;

    Q_PROPERTY(bool liveMode READ liveMode NOTIFY liveModeChanged)
    bool liveMode() const;

    Q_PROPERTY(QString streamType READ streamType NOTIFY streamTypeChanged)
    QString streamType() const;

    Q_PROPERTY(double cacheDuration READ cacheDuration NOTIFY cacheDurationChanged)
    double cacheDuration() const;

    Q_PROPERTY(double demuxerCacheTime READ demuxerCacheTime NOTIFY demuxerCacheTimeChanged)
    double demuxerCacheTime() const;

    Q_PROPERTY(double networkSpeedKbps READ networkSpeedKbps NOTIFY networkSpeedKbpsChanged)
    double networkSpeedKbps() const;

    Q_PROPERTY(double estimatedBitrate READ estimatedBitrate NOTIFY estimatedBitrateChanged)
    double estimatedBitrate() const;

    Q_PROPERTY(double avSync READ avSync NOTIFY avSyncChanged)
    double avSync() const;

    Q_PROPERTY(int droppedFrames READ droppedFrames NOTIFY droppedFramesChanged)
    int droppedFrames() const;

    Q_PROPERTY(QString playbackHealth READ playbackHealth NOTIFY playbackHealthChanged)
    QString playbackHealth() const;

    Q_PROPERTY(bool buffering READ buffering NOTIFY bufferingChanged)
    bool buffering() const;

    Q_PROPERTY(QString playbackState READ playbackState NOTIFY playbackStateChanged)
    QString playbackState() const;

    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
    QString errorMessage() const;

    Q_PROPERTY(double videoZoom READ videoZoom WRITE setVideoZoom NOTIFY videoZoomChanged)
    double videoZoom() const;
    void setVideoZoom(double value);

    Q_PROPERTY(QString videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)
    QString videoFillMode() const;
    void setVideoFillMode(const QString &mode);

    Q_PROPERTY(QString hwdecMode READ hwdecMode WRITE setHwdecMode NOTIFY hwdecModeChanged)
    QString hwdecMode() const;
    void setHwdecMode(const QString &mode);

    Q_PROPERTY(QString hwdecCurrent READ hwdecCurrent NOTIFY hwdecCurrentChanged)
    QString hwdecCurrent() const;

    Q_INVOKABLE void loadFile(const QString &file);
    Q_INVOKABLE void retry();

Q_SIGNALS:
    void mediaTitleChanged();
    void currentUrlChanged();
    void positionChanged();
    void durationChanged();
    void pauseChanged();
    void playbackRateChanged();
    void volumeChanged();

    void seekableChanged();
    void liveModeChanged();
    void streamTypeChanged();
    void cacheDurationChanged();
    void demuxerCacheTimeChanged();
    void networkSpeedKbpsChanged();
    void estimatedBitrateChanged();
    void avSyncChanged();
    void droppedFramesChanged();
    void playbackHealthChanged();

    void bufferingChanged();
    void playbackStateChanged();
    void errorMessageChanged();
    void videoZoomChanged();
    void videoFillModeChanged();
    void hwdecModeChanged();
    void hwdecCurrentChanged();

    void fileStarted();
    void fileLoaded();
    void endFile(QString reason);
    void videoReconfig();

private:
    void setupConnections();
    void onPropertyChanged(const QString &property, const QVariant &value);
    void onAsyncReply(const QVariant &data, mpv_event event);
    QString formatTime(const double time);
    void updateStreamType();
    void updateLiveMode();
    void updatePlaybackHealth();

    double m_position{0.0};
    QString m_formattedPosition;
    double m_duration{0.0};
    QString m_formattedDuration;
    QUrl m_currentUrl;
    double m_playbackRate{1.0};

    bool m_seekable{false};
    bool m_liveMode{false};
    QString m_streamType{QStringLiteral("unknown")};
    double m_cacheDuration{0.0};
    double m_demuxerCacheTime{0.0};
    double m_networkSpeedKbps{0.0};
    double m_estimatedBitrate{0.0};
    double m_avSync{0.0};
    int m_droppedFrames{0};
    QString m_playbackHealth{QStringLiteral("good")};

    bool m_buffering{false};
    QString m_playbackState{QStringLiteral("idle")};
    QString m_errorMessage;
    double m_videoZoom{0.0};
    QString m_videoFillMode{QStringLiteral("fit")};
    QString m_hwdecMode{QStringLiteral("auto")};
    QString m_hwdecCurrent;
};

#endif // MPVOBJECT_H
