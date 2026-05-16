#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonObject>
// #include <QWidget>

#include <optional>
#include <string_view>

#include "../utils/logger.h"


class Settings {
public:
    Settings();
    virtual ~Settings();

    // void setVolume() { }
    virtual void settingsInit() = 0;

    virtual const std::string_view getFileName() const = 0;
    [[nodiscard]] const std::optional<QString> readFileToString() const;
    [[nodiscard]] const QByteArray jsonToByteArray() const;

protected:
    std::shared_ptr<Logger> m_log;
    std::filesystem::path m_prefer_path;

    QJsonObject m_actual_json;

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

    [[nodiscard]] SettingsStates writeJsonToFile(const QString& json, const QString& another_path = ""); // Maybe update to bool

    void insertByPath(QJsonObject& obj, const QStringList& path, const QJsonValue& value);
    void configureNewSettings(const QMap<QString, QString>& default_settings, const QMap<QString, QString>& old_settings);

    virtual bool load();
    virtual bool save() { return false; }
    // virtual bool isFileValid();

private:
    void getVolume() { }

    // TODO: Realize when video will be ready
    virtual void setVideoQuality() { }
    virtual void getVideoQuality() { }
};

/////////////////////////////////////////////
class ContactSettings : public Settings {
public:
    ContactSettings();
    ~ContactSettings();

    void settingsInit() override;

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
public:
    UserSettings(const std::filesystem::path& prefer_path = "");
    ~UserSettings();

    void settingsInit() override;

    [[nodiscard]] const std::string_view getFileName() const override { return m_SETTINGS_FILE; };

    [[nodiscard]] const QSize getWindowSize() const;

protected:
    inline static constexpr std::string_view m_SETTINGS_FILE = "user_settings.json";


private:

    [[nodiscard]] QJsonObject getDefaultSettings();
    void setDefaultSettings();

    [[nodiscard]] SettingsStates isFieldsUpdated();
    void updateJson();
    QMap<QString, QString> bypassJson(const QJsonValue& value, const QString& path = "");

    [[nodiscard]] const std::optional<QJsonValue> findNeededJsonField(const QString& key, QJsonObject obj = QJsonObject()) const;
};

#endif  // SETTINGS_H
