#include "settings.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QTextStream>

#include <filesystem>

#include "../utils/own_utils.h"


static constexpr int VERSION = 1;
static constexpr int DEFAULT_1GB_MEMORY_LIMIT = 1 * 1024 * 1024/* * 1024*/;

Settings::Settings() : m_log(Logger::getInstance()) {
    std::filesystem::create_directory(getSettingsPath());
}

Settings::~Settings() {

}

const std::optional<QString> Settings::readFileToString() const {
    QFile file(getFilePath().c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_log->warning("Settings: Can't read JSON file. ", getFilePath().c_str());
        return {};
    }

    const QString content = file.readAll();
    file.close();

    return content;
}

const QByteArray Settings::jsonToByteArray() const {
    const QJsonDocument doc(m_actual_json);
    return doc.toJson();
}

Settings::SettingsStates Settings::writeJsonToFile(const QString& json, const QString& another_path) {
    QFile file(another_path.isEmpty() ? getFilePath().c_str() : another_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_log->error("Settings: Cannot create file ", file.fileName().toUtf8().constData(), ". Check the rights");
        return SettingsStates::CANT_CREATE_FILE;
    }

    QTextStream stream(&file);
    stream << json;
    file.close();

    return SettingsStates::CREATE_SUCCESSFUL;
}

const std::filesystem::path Settings::getSettingsPath() const {
    return m_prefer_path.empty() ? Utils::getPath("settings") : m_prefer_path;
}

const bool Settings::fileExist() const {
    const auto file_path = getSettingsPath() / getFileName();
    return std::filesystem::exists(file_path);
}

const std::filesystem::path Settings::getFilePath() const{
    return getSettingsPath() / getFileName();
}

void Settings::insertByPath(QJsonObject& obj, const QStringList& path, const QJsonValue& value) {
    if (path.isEmpty()) {
        return;
    }

    if (path.size() == 1) {
        obj[path[0]] = value;
    } else {
        const QString current_key = path[0];
        QJsonObject nested = obj[current_key].toObject();

        insertByPath(nested, path.mid(1), value);
        obj[current_key] = nested;
    }
}

void Settings::configureNewSettings(const QMap<QString, QString>& default_settings, const QMap<QString, QString>& old_settings) {
    QJsonObject new_settings;

    for (const QString& key : default_settings.keys()) {
        QString value = old_settings.contains(key) ? old_settings.value(key)
                                                   : default_settings.value(key);
        if (key == "VersionJSON" && default_settings.value(key) > old_settings.value(key)) {
            value = default_settings.value(key);
        }
        QJsonValue json_value;
        bool ok;
        const int int_val = value.toInt(&ok);
        if (ok) {
            json_value = int_val;
        } else {
            json_value = value;
        }

        QStringList path = key.split('.');
        insertByPath(new_settings, path, json_value);
    }

    const QJsonDocument doc(new_settings);
    const QString json_value = doc.toJson(QJsonDocument::Indented);
    if (writeJsonToFile(json_value) != SettingsStates::CANT_CREATE_FILE) {
        // TODO: Create messagebox with warning or something else
    }
}

bool Settings::load() {
    const std::optional<QString> content = readFileToString();
    if (!content.has_value() || content.value().isEmpty()) {
        return false;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(content.value().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        return false;
    }

    m_actual_json = doc.object();
    return true;
}


ContactSettings::ContactSettings() {

}

void ContactSettings::settingsInit() {
    if (!fileExist()) {
        setDefaultSettings();
    }
}

ContactSettings::~ContactSettings() {

}


UserSettings::UserSettings(const std::filesystem::path& prefer_path) {
    m_prefer_path = prefer_path;
}

void UserSettings::settingsInit() {
    if (!fileExist()) {
        setDefaultSettings();
        return;
    } else if (!load()) {
        // load backup file { if don't load just load default }
        return;
    }

    switch (isFieldsUpdated()) {
    case SettingsStates::JSON_FILE_UPDATED:
        updateJson();
        break;
    case SettingsStates::JSON_FILE_NOT_UPDATED:
        // Nothing to do
        break;
    case SettingsStates::PARSE_ERROR:
        // Probably I should write workaround
        break;
    case SettingsStates::CANT_READ_FILE:
        // Probably I should write workaround
        break;
    case SettingsStates::FILE_IS_EMPTY:
        m_log->info("File ", getFilePath().c_str(), " is empty. Recreate file");
        setDefaultSettings();
        break;
    default:
        break;
    };
}

UserSettings::~UserSettings() {

}

const QSize UserSettings::getWindowSize() const {
    const std::optional<QJsonValue> val = findNeededJsonField("Resolution");
    if (!val.has_value() || !val->isObject()) {
        m_log->info("Settings: Can't take resolution values");
        return QSize(1600, 900);
    }

    const QJsonObject obj = val->toObject();
    return QSize(obj["Width"].toInt(), obj["Height"].toInt());
}

QJsonObject UserSettings::getDefaultSettings() {
    QJsonObject collect_objects;

    // WINDOW
    QJsonObject window_object;
    window_object["StartMaximized"] = false;
    window_object["RememberWindowPosition"] = false;
    QJsonObject window_pos;
    window_pos["X"] = 0;
    window_pos["Y"] = 0;
    window_object["WindowPosition"] = window_pos;

    QJsonObject resolution;
    resolution["Width"] = 1600;
    resolution["Height"] = 900;
    window_object["Resolution"] = resolution;

    // SOUND
    QJsonObject sound_object;
    QJsonObject sound_devices;
    sound_devices["InputDevice"] = "default";
    sound_devices["OutputDevice"] = "default";

    QJsonObject sound_volume;
    sound_volume["InputVolume"]             = 100;
    sound_volume["OutputVolume"]            = 100;
    sound_volume["InputHeadphoneVolume"]    = 100;
    sound_volume["OutputHeadphoneVolume"]   = 100;

    sound_object["Devices"] = sound_devices;
    sound_object["Volume"] = sound_volume;

    // VIDEO
    // QJsonObject video;
    // video["Camera"]              = "default";
    // video["DefaultInputQuality"] = "720p";
    // video["OutputQuality"]       = "720p";
    // video["StreamQuality"]       = "720p";
    // video["Bitrate"]             = 2500;

    // COLLECT SETTINGS
    collect_objects["VersionJSON"]  = VERSION;
    collect_objects["Window"]       = window_object;
    collect_objects["Sound"]        = sound_object;
    // collect_objects["Video"]        = video;
    // TODO: User should choose this one
    collect_objects["ChatsHistory"] = "Path to current directory";
    collect_objects["Avatar"]       = "Path to default avatar";
    // TODO: Memory limit for all chats. Exceeding the limit starts to remove old messages
    collect_objects["ChatMemoryLimit"]  = DEFAULT_1GB_MEMORY_LIMIT;
    collect_objects["Theme"]            = "default";
    collect_objects["Language"]         = "en"; //getSystemLanguage();
    // collect_objects["Testing"]     = "Testing";

    return collect_objects;
}

void UserSettings::setDefaultSettings() {
    m_log->info("UserSettings: ", getFilePath());
    const QJsonObject defaults = getDefaultSettings();
    m_actual_json = defaults;

    const QJsonDocument doc(defaults);
    const QString json_string = doc.toJson(QJsonDocument::Indented);
    if (writeJsonToFile(json_string) == SettingsStates::CANT_CREATE_FILE) {
        // TODO: Create messagebox with warning or something else
    }
}

UserSettings::SettingsStates UserSettings::isFieldsUpdated() {
    QJsonValue version = m_actual_json["VersionJSON"];
    if (version == VERSION) {
        return SettingsStates::JSON_FILE_NOT_UPDATED;
    }

    return SettingsStates::JSON_FILE_UPDATED;
}

QMap<QString, QString> UserSettings::bypassJson(const QJsonValue& value, const QString& path) {
    QMap<QString, QString> paths_list;
    if (value.isObject()) {
        const QJsonObject obj = value.toObject();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            const QString p = path.isEmpty() ? it.key() : path + "." + it.key();
            QMap<QString, QString> nested = bypassJson(it.value(), p);
            paths_list.insert(nested);
        }
    } else if (value.isArray()) {
        const QJsonArray arr = value.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QMap<QString, QString> nested = bypassJson(arr.at(i), path + "[" + QString::number(i) + "]");
            paths_list.insert(nested);
        }
    } else {
        if (value.isString()) {
            paths_list[path] = value.toString();
            // m_log->info((path + " = " + paths_list[path]).toUtf8().constData());
        } else {
            paths_list[path] = QString::number(value.toInt());
            // m_log->info((path + " = " + paths_list[path]).toUtf8().constData());
        }
    }
    return paths_list;
};

void UserSettings::updateJson() {
    m_log->info("Update JSON method...");
    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(jsonToByteArray(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        m_log->warning("Settings: Updating file. JSON parse error. ", err.errorString().toUtf8().constData());
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    }

    const QJsonObject default_settings = getDefaultSettings();
    const auto paths_to_old_settings_values = bypassJson(m_actual_json);
    const auto paths_to_default_settings_values = bypassJson(default_settings);

    configureNewSettings(paths_to_default_settings_values, paths_to_old_settings_values);
}

const std::optional<QJsonValue> UserSettings::findNeededJsonField(const QString& key, QJsonObject obj) const {
    if (obj.empty()) {
        if (m_actual_json.empty()) {
            return { };
        }

        obj = m_actual_json;
    }

    for (auto it = obj.begin(); it != obj.end(); ++it) {
        if (it.key() == key) {
            return it.value();
        }

        if (it->isObject()) {
            const auto result = findNeededJsonField(key, it->toObject());
            if (result.has_value()) {
                return result;
            }
        }

        if (it->isArray()) {
            const auto arr = it->toArray();
            for (const auto& el : arr) {
                if (el.isObject()) {
                    const auto result = findNeededJsonField(key, el.toObject());
                    if (result.has_value()) {
                        return result;
                    }
                }
            }
        }
    }

    return {};
}
