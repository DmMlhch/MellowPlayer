#pragma once

#include <QFileInfo>
#include <QObject>
#include <QString>
#include <MellowPlayer/Macros.hpp>

BEGIN_MELLOWPLAYER_NAMESPACE(Application)

class IAlbumArtDownloader: public QObject
{
    Q_OBJECT
public:
    virtual ~IAlbumArtDownloader() = default;

    virtual bool download(const QString& artUrl, const QString& songId) = 0;
    virtual QFileInfo getLocalArtUrl(const QString &songId) = 0;

signals:
    void downloadFinished(const QString& localUrl);
};

END_MELLOWPLAYER_NAMESPACE