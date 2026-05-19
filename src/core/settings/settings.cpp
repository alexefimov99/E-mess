#include "settings.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QTextStream>

#include <filesystem>

#include "../../utils/own_utils.h"


Settings::Settings(std::filesystem::path prefer_path)
    : m_log(Logger::getInstance())
    , m_prefer_path(std::move(prefer_path)) {
    std::filesystem::create_directory(getSettingsPath());
}

const std::filesystem::path Settings::getSettingsPath() const {
    return m_prefer_path.empty() ? Utils::getPath("settings") : m_prefer_path;
}

const std::filesystem::path Settings::getFilePath() const {
    return getSettingsPath() / getFileName();
}

const bool Settings::fileExist() const {
    return std::filesystem::exists(getFilePath());
}

bool Settings::writeFile(const QByteArray& data) const {
    QFile file(getFilePath().c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_log->error("Settings: Cannot create file ", file.fileName().toUtf8().constData(), ". Check the rights");
        return false;
    }

    QTextStream stream(&file);
    stream << data;
    file.close();

    return true;
}

std::optional<QByteArray> Settings::readFile() const {
    QFile file(getFilePath().c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_log->warning("Settings: Can't read JSON file. ", getFilePath().c_str());
        return {};
    }

    return file.readAll();
}

QJsonValue Settings::getValue(const QString& path, const QJsonValue& default_value) const {
    if (path.isEmpty()) {
        return default_value;
    }

    const QStringList keys = path.split('.');
    QJsonValue curr = m_json;
    for (const auto& key : keys) {
        if (!curr.isObject()) {
            return default_value;
        }

        QJsonObject obj = curr.toObject();
        if (!obj.contains(key)) {
            return default_value;
        }

        curr = obj[key];
    }

    return curr;
}

void Settings::setValue(const QString& path, const QJsonValue& value) {
    QStringList keys = path.split('.');
    insertByPath(m_json, keys, value);
}

void Settings::insertByPath(QJsonObject& obj, QStringList& path, const QJsonValue& value) {
    if (path.isEmpty()) {
        return;
    }

    const QString key = path.takeFirst();

    if (path.isEmpty()) {
        obj[key] = value;
    } else {
        QJsonObject nested;
        if (obj.contains(key) && obj[key].isObject()) {
            nested = obj[key].toObject();
        }

        insertByPath(nested, path, value);
        obj[key] = std::move(nested);
    }
}

///////////////////////////////////////////////
MainSettings::MainSettings(const std::filesystem::path& prefer_path)
    : Settings(prefer_path) {
}

void MainSettings::init() {
    if (!fileExist()) {
        setDefaultSettings();
        return;
    }

    if (!load()) {
        m_log->warning("MainSettings: Can't load settings file");
        return;
    }

    migrateSettings();
}

const bool MainSettings::load() {
    auto data = readFile();
    if (!data || data->isEmpty()) {
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(*data, &err);
    if (err.error != QJsonParseError::NoError) {
        m_log->error("MainSettings: Parse error: ", err.errorString().toUtf8().constData());
        return false;
    }

    m_json = doc.object();
    return true;
}

const bool MainSettings::save() {
    QJsonDocument doc(m_json);
    return writeFile(doc.toJson(QJsonDocument::Indented));
}

QJsonObject MainSettings::createDefaultSettings() const {
    QJsonObject root;

    QJsonObject window;
    window["StartMaximized"] = false;
    window["RememberWindowPosition"] = false;
    window["WindowPosition"] = QJsonObject{{"X", 0}, {"Y", 0}};;
    window["Resolution"] = QJsonObject{{"Width", 1600}, {"Height", 900}};

    QJsonObject sound;
    sound["Devices"] = QJsonObject{{"InputDevice","default"}, {"OutputDevice","default"}};
    sound["Volume"] =
        QJsonObject{{"InputVolume", 100},
                    {"OutputVolume", 100},
                    {"InputHeadphoneVolume", 100},
                    {"OutputHeadphoneVolume", 100}};

    // VIDEO
    // QJsonObject video;
    // video["Camera"]              = "default";
    // video["DefaultInputQuality"] = "720p";
    // video["OutputQuality"]       = "720p";
    // video["StreamQuality"]       = "720p";
    // video["Bitrate"]             = 2500;

    root["VersionJSON"]  = CURR_VERSION;
    root["Window"]       = window;
    root["Sound"]        = sound;
    // root["Video"]        = video;
    root["Theme"]            = "default";
    root["Language"]         = "en"; // TODO: getSystemLanguage();
    // TODO: Memory limit for all chats. Exceeding the limit starts to remove old messages
    // Also should to transfer it to contact settings
    root["ChatMemoryLimit"]  = 1 * 1024 * 1024/* * 1024*/;  // TODO: uncomment it // 1GB
    // TODO: User should choose this one
    root["ChatsHistory"] = "Path to current directory";
    root["Avatar"]       = "Path to default avatar";

    return root;
}

void MainSettings::setDefaultSettings() {
    m_json = createDefaultSettings();
    if (!save()) {
        m_log->info("MainSettings: Can't save settings file");
    }
}

void MainSettings::migrateSettings() {
    const int version = m_json["VersionJSON"].toInt();
    if (version >= CURR_VERSION) {
        return;
    }

    m_log->info("MainSettings: Migrating from version", version, "to", CURR_VERSION);

    QJsonObject defaults = createDefaultSettings();
    QJsonObject newJson;

    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        const QString& key = it.key();
        newJson[key] = m_json.contains(key) ? m_json[key] : it.value();
    }

    m_json = std::move(newJson);
    m_json["VersionJSON"] = CURR_VERSION;
    save();
}
