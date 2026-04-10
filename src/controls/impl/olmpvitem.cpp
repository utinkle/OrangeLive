#include "olmpvitem.h"

#include "olmpvcontroller.h"
#include "olmpvproperties.h"

MpvItem::MpvItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    observeProperty(MpvProperties::self()->MediaTitle, MPV_FORMAT_STRING);
    observeProperty(MpvProperties::self()->Position, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Duration, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Pause, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->Speed, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->Volume, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->Seekable, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->DemuxerCacheDuration, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->CacheSpeed, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->AVSync, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->DroppedFrames, MPV_FORMAT_INT64);
    observeProperty(MpvProperties::self()->VideoBitrate, MPV_FORMAT_DOUBLE);
    observeProperty(MpvProperties::self()->PausedForCache, MPV_FORMAT_FLAG);
    observeProperty(MpvProperties::self()->HwdecCurrent, MPV_FORMAT_STRING);

    setupConnections();
    setProperty(QStringLiteral("mute"), false);
    setProperty(QStringLiteral("volume"), 80);
}

void MpvItem::setupConnections()
{
    // clang-format off
    connect(mpvController(), &MpvController::propertyChanged,
            this, &MpvItem::onPropertyChanged, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::fileStarted,
            this, [this]() {
                m_playbackState = QStringLiteral("opening");
                Q_EMIT playbackStateChanged();
                Q_EMIT fileStarted();
            }, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::fileLoaded,
            this, [this]() {
                m_playbackState = QStringLiteral("playing");
                Q_EMIT playbackStateChanged();
                Q_EMIT fileLoaded();
            }, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::endFile,
            this, [this](const QString &reason) {
                if (reason == QStringLiteral("eof")) {
                    m_playbackState = QStringLiteral("ended");
                    m_errorMessage.clear();
                } else {
                    m_playbackState = QStringLiteral("error");
                    m_errorMessage = reason;
                }
                Q_EMIT playbackStateChanged();
                Q_EMIT errorMessageChanged();
                Q_EMIT endFile(reason);
            }, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::videoReconfig,
            this, &MpvItem::videoReconfig, Qt::QueuedConnection);

    connect(mpvController(), &MpvController::asyncReply,
            this, &MpvItem::onAsyncReply, Qt::QueuedConnection);
    // clang-format on
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == MpvProperties::self()->MediaTitle) {
        Q_EMIT mediaTitleChanged();

    } else if (property == MpvProperties::self()->Position) {
        m_position = value.toDouble();
        m_formattedPosition = formatTime(m_position);
        updateLiveMode();
        Q_EMIT positionChanged();

    } else if (property == MpvProperties::self()->Duration) {
        m_duration = value.toDouble();
        m_formattedDuration = formatTime(m_duration);
        updateLiveMode();
        Q_EMIT durationChanged();

    } else if (property == MpvProperties::self()->Pause) {
        Q_EMIT pauseChanged();

    } else if (property == MpvProperties::self()->Volume) {
        Q_EMIT volumeChanged();

    } else if (property == MpvProperties::self()->Speed) {
        const double speed = value.toDouble();
        if (!qFuzzyCompare(m_playbackRate + 1.0, speed + 1.0)) {
            m_playbackRate = speed;
            Q_EMIT playbackRateChanged();
        }

    } else if (property == MpvProperties::self()->Seekable) {
        const bool seekable = value.toBool();
        if (m_seekable != seekable) {
            m_seekable = seekable;
            updateLiveMode();
            Q_EMIT seekableChanged();
        }

    } else if (property == MpvProperties::self()->DemuxerCacheDuration) {
        const double cacheTime = qMax(0.0, value.toDouble());
        if (!qFuzzyCompare(m_demuxerCacheTime + 1.0, cacheTime + 1.0)) {
            m_demuxerCacheTime = cacheTime;
            m_cacheDuration = m_demuxerCacheTime;
            Q_EMIT demuxerCacheTimeChanged();
            Q_EMIT cacheDurationChanged();
        }

    } else if (property == MpvProperties::self()->CacheSpeed) {
        const double speedKbps = qMax(0.0, value.toDouble() / 1024.0);
        if (!qFuzzyCompare(m_networkSpeedKbps + 1.0, speedKbps + 1.0)) {
            m_networkSpeedKbps = speedKbps;
            updatePlaybackHealth();
            Q_EMIT networkSpeedKbpsChanged();
        }

    } else if (property == MpvProperties::self()->AVSync) {
        const double avSync = value.toDouble();
        if (!qFuzzyCompare(m_avSync + 1.0, avSync + 1.0)) {
            m_avSync = avSync;
            updatePlaybackHealth();
            Q_EMIT avSyncChanged();
        }

    } else if (property == MpvProperties::self()->DroppedFrames) {
        const int droppedFrames = value.toInt();
        if (m_droppedFrames != droppedFrames) {
            m_droppedFrames = droppedFrames;
            updatePlaybackHealth();
            Q_EMIT droppedFramesChanged();
        }

    } else if (property == MpvProperties::self()->VideoBitrate) {
        const double bitrate = qMax(0.0, value.toDouble());
        if (!qFuzzyCompare(m_estimatedBitrate + 1.0, bitrate + 1.0)) {
            m_estimatedBitrate = bitrate;
            updatePlaybackHealth();
            Q_EMIT estimatedBitrateChanged();
        }

    } else if (property == MpvProperties::self()->HwdecCurrent) {
        const QString hwdecCurrent = value.toString();
        if (m_hwdecCurrent != hwdecCurrent) {
            m_hwdecCurrent = hwdecCurrent;
            Q_EMIT hwdecCurrentChanged();
        }

    } else if (property == MpvProperties::self()->PausedForCache) {
        const bool buffering = value.toBool();
        if (m_buffering != buffering) {
            m_buffering = buffering;
            m_playbackState = m_buffering ? QStringLiteral("buffering") : QStringLiteral("playing");
            Q_EMIT bufferingChanged();
            Q_EMIT playbackStateChanged();
        }
    }
}

void MpvItem::onAsyncReply(const QVariant &data, mpv_event event)
{
    switch (static_cast<AsyncIds>(event.reply_userdata)) {
    case AsyncIds::None: {
        break;
    }
    case AsyncIds::SetVolume: {
        qDebug() << "onSetPropertyReply" << event.reply_userdata;
        break;
    }
    case AsyncIds::GetVolume: {
        qDebug() << "onGetPropertyReply" << event.reply_userdata << data;
        break;
    }
    case AsyncIds::ExpandText: {
        qDebug() << "onGetPropertyReply" << event.reply_userdata << data;
        break;
    }
    }
}

QString MpvItem::formatTime(const double time)
{
    int totalNumberOfSeconds = static_cast<int>(time);
    int seconds = totalNumberOfSeconds % 60;
    int minutes = (totalNumberOfSeconds / 60) % 60;
    int hours = (totalNumberOfSeconds / 60 / 60);

    QString timeString =
        QStringLiteral("%1:%2:%3").arg(hours, 2, 10, QLatin1Char('0')).arg(minutes, 2, 10, QLatin1Char('0')).arg(seconds, 2, 10, QLatin1Char('0'));

    return timeString;
}

void MpvItem::loadFile(const QString &file)
{
    auto url = QUrl::fromUserInput(file);
    if (m_currentUrl != url) {
        m_currentUrl = url;
        updateStreamType();
        updateLiveMode();
        m_playbackState = QStringLiteral("opening");
        m_errorMessage.clear();
        Q_EMIT playbackStateChanged();
        Q_EMIT errorMessageChanged();
        Q_EMIT currentUrlChanged();
    }

    command(QStringList() << QStringLiteral("loadfile") << m_currentUrl.toString(QUrl::PreferLocalFile));
}


void MpvItem::retry()
{
    if (!m_currentUrl.isValid() || m_currentUrl.isEmpty()) {
        return;
    }

    m_playbackState = QStringLiteral("opening");
    m_errorMessage.clear();
    Q_EMIT playbackStateChanged();
    Q_EMIT errorMessageChanged();

    command(QStringList() << QStringLiteral("loadfile") << m_currentUrl.toString(QUrl::PreferLocalFile));
}

QString MpvItem::mediaTitle()
{
    return getProperty(MpvProperties::self()->MediaTitle).toString();
}

double MpvItem::position()
{
    return m_position;
}

void MpvItem::setPosition(double value)
{
    if (qFuzzyCompare(value, position())) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Position, value);
}

double MpvItem::duration()
{
    return m_duration;
}

bool MpvItem::pause()
{
    return getProperty(MpvProperties::self()->Pause).toBool();
}

void MpvItem::setPause(bool value)
{
    if (value == pause()) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Pause, value);
}

void MpvItem::stop()
{
    command(QStringList() << QStringLiteral("stop"));
}

double MpvItem::playbackRate() const
{
    return m_playbackRate;
}

void MpvItem::setPlaybackRate(double value)
{
    const double speed = qBound(0.25, value, 4.0);
    if (qFuzzyCompare(speed, m_playbackRate)) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Speed, speed);
}

int MpvItem::volume()
{
    return getProperty(MpvProperties::self()->Volume).toInt();
}

void MpvItem::setVolume(int value)
{
    if (value == volume()) {
        return;
    }
    setPropertyAsync(MpvProperties::self()->Volume, value);
}

QString MpvItem::formattedDuration() const
{
    return m_formattedDuration;
}

QString MpvItem::formattedPosition() const
{
    return m_formattedPosition;
}

QUrl MpvItem::currentUrl() const
{
    return m_currentUrl;
}

bool MpvItem::seekable() const
{
    return m_seekable;
}

bool MpvItem::liveMode() const
{
    return m_liveMode;
}

QString MpvItem::streamType() const
{
    return m_streamType;
}

double MpvItem::cacheDuration() const
{
    return m_cacheDuration;
}

double MpvItem::demuxerCacheTime() const
{
    return m_demuxerCacheTime;
}

double MpvItem::networkSpeedKbps() const
{
    return m_networkSpeedKbps;
}

double MpvItem::estimatedBitrate() const
{
    return m_estimatedBitrate;
}

double MpvItem::avSync() const
{
    return m_avSync;
}

int MpvItem::droppedFrames() const
{
    return m_droppedFrames;
}

QString MpvItem::playbackHealth() const
{
    return m_playbackHealth;
}

bool MpvItem::buffering() const
{
    return m_buffering;
}

QString MpvItem::playbackState() const
{
    return m_playbackState;
}

QString MpvItem::errorMessage() const
{
    return m_errorMessage;
}

double MpvItem::videoZoom() const
{
    return m_videoZoom;
}

void MpvItem::setVideoZoom(double value)
{
    const double zoom = qBound(-1.0, value, 3.0);
    if (qFuzzyCompare(m_videoZoom + 1.0, zoom + 1.0)) {
        return;
    }

    m_videoZoom = zoom;
    setPropertyAsync(QStringLiteral("video-zoom"), m_videoZoom);
    Q_EMIT videoZoomChanged();
}

QString MpvItem::videoFillMode() const
{
    return m_videoFillMode;
}

void MpvItem::setVideoFillMode(const QString &mode)
{
    QString normalizedMode = mode.toLower();
    if (normalizedMode != QStringLiteral("fit") && normalizedMode != QStringLiteral("fill") && normalizedMode != QStringLiteral("crop")) {
        normalizedMode = QStringLiteral("fit");
    }

    if (m_videoFillMode == normalizedMode) {
        return;
    }

    m_videoFillMode = normalizedMode;
    if (m_videoFillMode == QStringLiteral("fit")) {
        setPropertyAsync(QStringLiteral("panscan"), 0.0);
    } else if (m_videoFillMode == QStringLiteral("fill")) {
        setPropertyAsync(QStringLiteral("panscan"), 0.5);
    } else {
        setPropertyAsync(QStringLiteral("panscan"), 1.0);
    }
    Q_EMIT videoFillModeChanged();
}

QString MpvItem::hwdecMode() const
{
    return m_hwdecMode;
}

void MpvItem::setHwdecMode(const QString &mode)
{
    QString normalizedMode = mode.toLower();
    if (normalizedMode != QStringLiteral("auto") && normalizedMode != QStringLiteral("no")) {
        normalizedMode = QStringLiteral("auto");
    }

    if (m_hwdecMode == normalizedMode) {
        return;
    }

    m_hwdecMode = normalizedMode;
    setPropertyAsync(QStringLiteral("hwdec"), m_hwdecMode);
    Q_EMIT hwdecModeChanged();
}

QString MpvItem::hwdecCurrent() const
{
    return m_hwdecCurrent;
}

void MpvItem::updateStreamType()
{
    QString type = QStringLiteral("unknown");
    const QString scheme = m_currentUrl.scheme().toLower();
    const QString path = m_currentUrl.path().toLower();

    if (scheme == QStringLiteral("rtsp")) {
        type = QStringLiteral("rtsp");
    } else if (path.endsWith(QStringLiteral(".flv")) || m_currentUrl.toString().contains(QStringLiteral(".flv"), Qt::CaseInsensitive)) {
        type = QStringLiteral("flv");
    } else if (path.endsWith(QStringLiteral(".m3u8")) || m_currentUrl.toString().contains(QStringLiteral("m3u8"), Qt::CaseInsensitive)) {
        type = QStringLiteral("hls");
    } else if (!scheme.isEmpty()) {
        type = QStringLiteral("vod");
    }

    if (m_streamType != type) {
        m_streamType = type;
        Q_EMIT streamTypeChanged();
    }
}

void MpvItem::updateLiveMode()
{
    bool live = false;
    const QString lowerStreamType = m_streamType.toLower();
    if (lowerStreamType == QStringLiteral("rtsp") || lowerStreamType == QStringLiteral("flv")) {
        live = true;
    }

    if (!m_seekable) {
        live = true;
    }

    if (m_duration <= 0.0 && m_position > 0.0) {
        live = true;
    }

    if (m_liveMode != live) {
        m_liveMode = live;
        Q_EMIT liveModeChanged();
    }
}

void MpvItem::updatePlaybackHealth()
{
    QString health = QStringLiteral("good");

    if (m_networkSpeedKbps < 30.0 || m_droppedFrames > 30 || qAbs(m_avSync) > 0.3) {
        health = QStringLiteral("poor");
    } else if (m_networkSpeedKbps < 80.0 || m_droppedFrames > 10 || qAbs(m_avSync) > 0.1) {
        health = QStringLiteral("warning");
    }

    if (m_playbackHealth != health) {
        m_playbackHealth = health;
        Q_EMIT playbackHealthChanged();
    }
}


MpvItem::~MpvItem()
{
}

