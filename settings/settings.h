#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

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
    };

protected:
    [[nodiscard]] const std::filesystem::path getSettingsPath() const;
    [[nodiscard]] const bool fileExist() const;
    [[nodiscard]] const std::filesystem::path getFilePath() const;

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
    void setDefaults() { m_log->info("ContactSettings: ", getFilePath()); };
    const std::string_view getFileName() const override { return m_SETTINGS_FILE; };
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
    [[nodiscard]] SettingsStates isFieldsUpdated();
    void setDefaultSettings();
    void bypassJson(const QJsonValue& value, const QString& path = "");
    void updateJSON();
};

#endif  // SETTINGS_H
