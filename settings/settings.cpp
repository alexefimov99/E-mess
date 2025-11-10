#include "settings.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
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

const std::filesystem::path Settings::getSettingsPath() const {
    return Utils::getPath("settings");
}

const bool Settings::fileExist() const {
    const auto file_path = getSettingsPath() / getFileName();
    return std::filesystem::exists(file_path);
}

const std::filesystem::path Settings::getFilePath() const{
    return getSettingsPath() / getFileName();
}


ContactSettings::ContactSettings() {
    if (!fileExist()) {
        setDefaults();
    }
}

ContactSettings::~ContactSettings() {

}


UserSettings::UserSettings() {
    if (!fileExist()) {
        setDefaultSettings();
        return;
    }

    switch (isFieldsUpdated()) {
    case SettingsStates::JSON_FILE_UPDATED:
        updateJSON();
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

    head_object.insert("SelfSettings", collect_objects);
    return head_object;
}

void UserSettings::setDefaultSettings() {
    m_log->info("UserSettings: ", getFilePath());
    QJsonObject defaults = getDefaultSettings();

    QJsonDocument doc(defaults);
    QString json_string = doc.toJson(QJsonDocument::Indented);

    QFile file(getFilePath().c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_log->error("Cannot create file ", file.fileName().toUtf8().constData(), ". Check the rights");
        // TODO: Create messagebox with warning
        return;
    }
    QTextStream stream(&file);
    stream << json_string;
    file.close();
}

UserSettings::SettingsStates UserSettings::isFieldsUpdated() {
    QFile file(getFilePath().c_str());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_log->warning("Can't read JSON file. ", getFilePath().c_str());
        return SettingsStates::CANT_READ_FILE;
    }
    QString json_value = file.readAll();
    file.close();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json_value.toUtf8(), &err);
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

// TODO: Think how I can compare two files and do replacements
void UserSettings::bypassJson(const QJsonValue& value, const QString& path) {
    if (value.isObject()) {
        const QJsonObject obj = value.toObject();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            const QString p = path.isEmpty() ? it.key() : path + "." + it.key();
            bypassJson(it.value(), p);
        }
    } else if (value.isArray()) {
        const QJsonArray arr = value.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            bypassJson(arr.at(i), path + "[" + QString::number(i) + "]");
        }
    } else {
        if (value.isString()) {
            m_log->info(path.toUtf8().constData(), " = ", value.toString().toUtf8().constData());
        } else {
            m_log->info(path.toUtf8().constData(), " = ", value.toInt());
        }
    }
};

void UserSettings::updateJSON() {
    QFile old_file(getFilePath().c_str());
    if (!old_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_log->warning("Can't read JSON file. ", getFilePath().c_str());
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    }
    QString json_value = old_file.readAll();
    old_file.close();
    old_file.remove();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(json_value.toUtf8(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        m_log->warning("Settings: Updating file. JSON parse error. ", err.errorString().toUtf8().constData());
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    }

    const QJsonObject defaults = getDefaultSettings();
    QJsonObject old_settings = doc.object();
    // for (const QString& key : old_settings.keys()) {
    //     bypassJson(old_settings);
    //     if (!defaults.contains(key)) {
    //         old_settings.remove(key);
    //     }
    // }

    QJsonObject new_settings;
    // for (const QString& key : defaults.keys()) {
    //     // bypassJson(new_settings);
    //     if (old_settings.contains(key) && old_settings.value(key).type() == defaults.value(key).type()) {
    //         new_settings[key] = old_settings.value(key);
    //     } else {
    //         new_settings[key] = defaults.value(key);
    //     }
    // }


    const QJsonDocument new_doc(new_settings);
    QFile new_file(getFilePath().c_str());
    if (!new_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_log->warning("Can't create new JSON file. ", getFilePath().c_str());
        // Maybe user wants to recreate JSON with default settings? Messagebox
        return;
    }
    new_file.write(new_doc.toJson(QJsonDocument::Indented));
    new_file.close();
}
