#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QTextStream>
#include <QtGui/QIcon>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <MellowPlayer/UseCases/Logging/LoggingManager.hpp>
#include <MellowPlayer/Entities/StreamingServices/StreamingServicePlugin.hpp>
#include <MellowPlayer/Entities/StreamingServices/StreamingServicePluginMetadata.hpp>
#include <MellowPlayer/Entities/StreamingServices/StreamingServiceStyle.hpp>
#include "StreamingServicePluginLoader.hpp"

USE_MELLOWPLAYER_NAMESPACE(Entities)
USE_MELLOWPLAYER_NAMESPACE(UseCases)
USE_MELLOWPLAYER_NAMESPACE(Infrastructure)
using namespace std;

StreamingServicePluginLoader::StreamingServicePluginLoader() :
        logger(LoggingManager::instance().getLogger("StreamingServicePluginLoader")) {

}

QList<shared_ptr<StreamingServicePlugin>> StreamingServicePluginLoader::load() const {
    QList<shared_ptr<StreamingServicePlugin>> plugins;
    for (const QString& path: getSearchPaths()) {
        if (!QDir(path).exists()) {
            LOG_DEBUG(logger, "skipping plugin path: " << path.toStdString().c_str() << " (directory not found)");
            continue;
        }
        LOG_DEBUG(logger, "looking for plugins in " << path.toStdString().c_str());
        for (const QFileInfo& directory: QDir(path).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (checkPluginDirectory(directory.absoluteFilePath())) {
                shared_ptr<StreamingServicePlugin> plugin = loadPlugin(directory.absoluteFilePath());
                if (plugin->isValid() && !containsPlugin(plugins, plugin)) {
                    LOG_INFO(logger, plugin->getName() + " plugin successfully loaded (from \"" +
                                     directory.absoluteFilePath() + "\")");
                    plugins.append(plugin);
                } else {
                    LOG_DEBUG(logger, "skipping plugin " + plugin->getName() +
                                      ", already loaded from another source or invalid");
                }
            }
        }
    }

    return plugins;
}

QString StreamingServicePluginLoader::findFileByExtension(const QString& directory, const QString& suffix) const {
            foreach (const QFileInfo& fileInfo,
                     QDir(directory).entryInfoList(QDir::Files | QDir::NoDotAndDotDot)) {
            if (fileInfo.isFile() && fileInfo.suffix() == suffix)
                return fileInfo.absoluteFilePath();
        }
    return QString();
}

QString StreamingServicePluginLoader::readFileContent(const QString& filePath) const {
    QString retVal;

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        retVal = in.readAll();
    }

    return retVal;
}

StreamingServicePluginMetadata StreamingServicePluginLoader::readMetadata(const QString& filePath) const {
    QSettings meta(filePath, QSettings::IniFormat);

    StreamingServicePluginMetadata pluginMetadata;
    pluginMetadata.author = meta.value("author").toString();
    pluginMetadata.authorWebsite = meta.value("author_website").toString();
    pluginMetadata.logoPath = QFileInfo(QFileInfo(filePath).dir(), meta.value("icon").toString()).absoluteFilePath();
    pluginMetadata.name = meta.value("name").toString();
    pluginMetadata.url = meta.value("url").toString();
    pluginMetadata.version = meta.value("version").toString();
    pluginMetadata.color = meta.value("color").toString();

    return pluginMetadata;
}

StreamingServiceStyle StreamingServicePluginLoader::readStyle(const QString& filePath) const {
    StreamingServiceStyle style;

    if (QFileInfo(filePath).exists()) {
        QFile file(filePath);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QJsonDocument document = QJsonDocument::fromJson(file.readAll().data());
            QJsonObject object = document.object();
            style.accent = object.value("accent").toString();
            style.background = object.value("background").toString();
            style.foreground = object.value("foreground").toString();
            style.primary = object.value("primary").toString();
            style.primaryForeground = object.value("primaryForeground").toString();
            style.secondary = object.value("secondary").toString();
            style.secondaryForeground = object.value("secondaryForeground").toString();
        }
    }

    return style;
}

unique_ptr<StreamingServicePlugin> StreamingServicePluginLoader::loadPlugin(const QString& directory) const {
    QString metadataPath = findFileByExtension(directory, "ini");
    QString scriptPath = findFileByExtension(directory, "js");
    QString stylePath = findFileByExtension(directory, "json");
    QString locale = QLocale::system().name().split("_")[0];
    StreamingServicePluginMetadata metadata = readMetadata(metadataPath);
    metadata.script = readFileContent(scriptPath);
    metadata.scriptPath = scriptPath;
    StreamingServiceStyle style = readStyle(stylePath);

    return make_unique<StreamingServicePlugin>(metadata, style);
}

bool StreamingServicePluginLoader::checkPluginDirectory(const QString& directory) const {
    QString metadataPath = findFileByExtension(directory, "ini");
    QString scriptPath = findFileByExtension(directory, "js");

    return !scriptPath.isEmpty() && !metadataPath.isEmpty();
}

QString StreamingServicePluginLoader::getUserPluginsDirectory() const {
    return QFileInfo(QStandardPaths::standardLocations(
            QStandardPaths::AppLocalDataLocation)[0], "plugins").absoluteFilePath();
}

QStringList StreamingServicePluginLoader::getSearchPaths() const {
    QStringList paths;
    paths.append(CMAKE_SOURCE_DIR + QString(QDir::separator()) + "plugins");
    paths.append(QFileInfo(QDir::currentPath(), "plugins").absoluteFilePath());
    paths.append(QFileInfo(qApp->applicationDirPath(), "plugins").absoluteFilePath());
    paths.append(getUserPluginsDirectory());

#ifdef Q_OS_MAC
    QDir pluginsDir(qApp->applicationDirPath());
    pluginsDir.cdUp();
    pluginsDir.cd("PlugIns");
    pluginsDir.cd("services");
    paths.append(pluginsDir.path());
#endif

#ifdef Q_OS_LINUX
    paths.append("/usr/share/mellowplayer/plugins");
    paths.append("/usr/local/share/mellowplayer/plugins");
#endif

    paths.removeDuplicates();

    return paths;
}

bool StreamingServicePluginLoader::containsPlugin(const QList<shared_ptr<StreamingServicePlugin>>& plugins,
                                  shared_ptr<StreamingServicePlugin>& toCheck) const {
    for (auto plugin: plugins) {
        if (*toCheck == *plugin)
            return true;
    }
    return false;
}