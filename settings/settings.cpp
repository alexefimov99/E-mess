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
        setDefaults();
        return;
    }

    checkFields();
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

UserSettings::SettingsStates UserSettings::setDefaultSettings() {
    m_log->info("UserSettings: ", getFilePath());
    QJsonObject defaults = getDefaultSettings();

    QJsonDocument doc(defaults);
    QString json_string = doc.toJson(QJsonDocument::Indented);

    QFile file(getFilePath().c_str());
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << json_string;
    file.close();
}

void UserSettings::checkFields() {
    QFile file(getFilePath().c_str());
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString json_value = file.readAll();
    file.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json_value.toUtf8(), &err);
    if (err.error != QJsonParseError::ParseError::NoError) {
        m_log->warning("Settings: JSON parse error. ", err.errorString().toUtf8().constData());
        return;
    }

    QJsonValue version = doc.object().value("VersionJSON");
    if (version == 1) {
        return;
    }

    // TODO: Realize it next
    updateJSON();
}
