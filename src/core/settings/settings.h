#ifndef SETTINGS_H
#define SETTINGS_H

#include <QJsonObject>
// #include <QWidget>

#include <optional>
#include <string_view>

#include "../../utils/logger.h"


class Settings {
public:
    Settings(std::filesystem::path prefer_path = {});
    virtual ~Settings() = default;

    virtual void init() = 0;
    virtual const std::string_view getFileName() const = 0;

    [[nodiscard]] QJsonValue getValue(const QString& path, const QJsonValue& default_value = {}) const;
    void setValue(const QString& path, const QJsonValue& value);

protected:
    std::shared_ptr<Logger> m_log;
    std::filesystem::path m_prefer_path;
    QJsonObject m_json;

protected:
    [[nodiscard]] const std::filesystem::path getSettingsPath() const;
    [[nodiscard]] const std::filesystem::path getFilePath() const;
    [[nodiscard]] const bool fileExist() const;

    [[nodiscard]] bool writeFile(const QByteArray& data) const;
    [[nodiscard]] std::optional<QByteArray> readFile() const;

    void insertByPath(QJsonObject& obj, QStringList& path, const QJsonValue& value);

    virtual const bool load() = 0;
    virtual const bool save() = 0;
};

/////////////////////////////////////
class MainSettings : public Settings {
public:
    explicit MainSettings(const std::filesystem::path& prefer_path = {});
    ~MainSettings() = default;

    void init() override;
    const std::string_view getFileName() const override {
        return m_SETTINGS_FILE;
    }

    void migrateSettings();

    QString getAvatarPath() const;
    QString getChatHistoryPath() const;
    int getCurrentVersion() const {
        return CURR_VERSION;
    };
    QString getLanguage() const;
    QString getTheme() const;
    QSize getWindowSize() const;

    void setAvatarPath(const QString& size);
    void setChatHistoryPath(const QString& path);
    void setLanguage(const QString& lang);
    void setTheme(const QString& theme);
    void setWindowSize(const QSize& size);

    void resetToDefaults() {
        setDefaultSettings();
    }

protected:
    const bool load() override;
    const bool save() override;

private:
    QJsonObject createDefaultSettings() const;
    void setDefaultSettings();

    static constexpr int CURR_VERSION = 1;
    static constexpr std::string_view m_SETTINGS_FILE = "main_settings.json";
};

/////////////////////////////////////
// class ContactSettings : public Settings {

// };

#endif  // SETTINGS_H
