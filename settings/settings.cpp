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
static constexpr int DEFAULT_1GB_MEMORY_LIMIT = 1 * 1024 * 1024;

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
        const QString value = old_settings.contains(key) ? old_settings.value(key)
                                                         : default_settings.value(key);
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

QJsonObject UserSettings::getDefaultSettings() {
    QJsonObject head_object;
    QJsonObject collect_objects;

    QJsonObject window_object;
    window_object["width"] = 1600;
    window_object["height"] = 900;

    QJsonObject sound_object;
    sound_object["InputVolume"]             = 100;
    sound_object["OutputVolume"]            = 100;
    sound_object["InputHeadphoneVolume"]    = 100;
    sound_object["OutputHeadphoneVolume"]   = 100;

    collect_objects["VersionJSON"]  = VERSION;
    collect_objects["Resolution"]   = window_object;
    collect_objects["Sound"]        = sound_object;
    // TODO: User should choose this one
    collect_objects["ChatsHistory"] = "Path to current directory";
    collect_objects["Avatar"]       = "Path to default avatar";
    // TODO: Memory limit for all chats. Exceeding the limit starts to remove old messages
    collect_objects["ChatMemoryLimit"]  = DEFAULT_1GB_MEMORY_LIMIT;
    collect_objects["Theme"]            = "default";
    collect_objects["VideoQuality"]     = "We Will See In The Future";
    // collect_objects["Testing"]     = "Testing";

    head_object.insert("SelfSettings", collect_objects);
    return head_object;
}

void UserSettings::setDefaultSettings() {
    m_log->info("UserSettings: ", getFilePath());
    const QJsonObject defaults = getDefaultSettings();

    const QJsonDocument doc(defaults);
    const QString json_string = doc.toJson(QJsonDocument::Indented);
    if (writeJsonToFile(json_string) == SettingsStates::CANT_CREATE_FILE) {
        // TODO: Create messagebox with warning or something else
    }
}

UserSettings::SettingsStates UserSettings::isFieldsUpdated() {
    const std::optional<QString> json_value = readFileToString();
    if (!json_value.has_value()) {
        return SettingsStates::CANT_READ_FILE;
    } else if (json_value.value().isEmpty()) {
        return SettingsStates::FILE_IS_EMPTY;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json_value.value().toUtf8(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        m_log->warning("Settings: JSON parse error. ", err.errorString().toUtf8().constData());
        return SettingsStates::PARSE_ERROR;
    }

    QJsonValue version = doc.object().value("SelfSettings").toObject().value("VersionJSON");
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
    const std::optional<QString> json_value = readFileToString();
    if (!json_value.has_value()) {
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    } else if (json_value.value().isEmpty()) {
        // hmmmmmmmmmmmmmmmm
        return;
    }

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json_value.value().toUtf8(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        m_log->warning("Settings: Updating file. JSON parse error. ", err.errorString().toUtf8().constData());
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    }

    const QJsonObject default_settings = getDefaultSettings();
    const QJsonObject old_settings = doc.object();
    const auto paths_to_old_settings_values = bypassJson(old_settings);
    const auto paths_to_default_settings_values = bypassJson(default_settings);

    configureNewSettings(paths_to_default_settings_values, paths_to_old_settings_values);
}
