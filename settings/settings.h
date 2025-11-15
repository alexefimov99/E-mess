#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

#include <optional>
#include <string_view>

#include "../utils/logger.h"


class Settings : public QWidget {
public:
    Settings();
    virtual ~Settings();

    void setVolume() { }

protected:
    std::shared_ptr<Logger> m_log;

    enum class SettingsStates {
        JSON_FILE_UPDATED,
        JSON_FILE_NOT_UPDATED,

        PARSE_ERROR,

        CREATE_SUCCESSFUL,
        CANT_CREATE_FILE,
        CANT_OPEN_FILE,
        CANT_READ_FILE,
        FILE_IS_EMPTY
    };

protected:
    [[nodiscard]] const std::filesystem::path getSettingsPath() const;
    [[nodiscard]] const std::filesystem::path getFilePath() const;
    [[nodiscard]] const bool fileExist() const;

    [[nodiscard]] std::optional<QString> readFileToString() const;
    [[nodiscard]] SettingsStates writeJsonToFile(const QString& json, const QString& another_path = ""); // Maybe update to bool

    void insertByPath(QJsonObject& obj, const QStringList& path, const QJsonValue& value);
    void configureNewSettings(const QMap<QString, QString>& default_settings, const QMap<QString, QString>& old_settings);

private:
    void getVolume() { }

    virtual const std::string_view getFileName() const = 0;

    // TODO: Realize when video will be ready
    virtual void setVideoQuality() { }
    virtual void getVideoQuality() { }
};

/////////////////////////////////////////////
class ContactSettings : public Settings {
    Q_OBJECT
public:
    ContactSettings();
    ~ContactSettings();

protected:
    inline static constexpr std::string_view m_SETTINGS_FILE = "contact_settings.json";

private:
    bool global_settings = true;

private:
    const std::string_view getFileName() const override { return m_SETTINGS_FILE; };
    void setDefaultSettings() { m_log->info("ContactSettings: ", getFilePath()); };
};

/////////////////////////////////////////////
class UserSettings : public Settings {
    Q_OBJECT
public:
    UserSettings();
    ~UserSettings();

protected:
    inline static constexpr std::string_view m_SETTINGS_FILE = "user_settings.json";


private:
    [[nodiscard]] const std::string_view getFileName() const override { return m_SETTINGS_FILE; };

    [[nodiscard]] QJsonObject getDefaultSettings();
    void setDefaultSettings();

    [[nodiscard]] SettingsStates isFieldsUpdated();
    void updateJson();
    QMap<QString, QString> bypassJson(const QJsonValue& value, const QString& path = "");
};

#endif  // SETTINGS_H
