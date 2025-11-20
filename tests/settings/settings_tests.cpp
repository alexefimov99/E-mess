#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

#include <filesystem>

#include "../../settings/settings.h"

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-result"

// #pragma GCC diagnostic pop

static std::filesystem::path general_test_path;
static std::filesystem::path settings_test_path;
static std::filesystem::path json_files_dir;

static constexpr int VERSION = 1;

class UserSettingsTests : public UserSettings {
public:
    UserSettingsTests(const std::filesystem::path& prefer_path)
        : m_prefer_path(prefer_path)
        , UserSettings(prefer_path) {

    }

    [[nodiscard]] const std::string_view getFileName() const override { return m_SETTINGS_FILE; };
    QJsonObject getDefaultSettings() {
        QJsonObject head_object;
        QJsonObject collect_objects;

        collect_objects["VersionJSON"]  = VERSION;
        collect_objects["Resolution"]   = getWindowJsonObject();
        collect_objects["Sound"]        = getSoundJsonObject();

        head_object.insert("SelfTestSettings", collect_objects);
        return head_object;
    }

    QJsonObject getMoreSettings() {
        QJsonObject head_object;
        QJsonObject collect_objects;

        collect_objects["VersionJSON"]  = VERSION;
        collect_objects["Resolution"]   = getWindowJsonObject();
        collect_objects["Sound"]        = getSoundJsonObject();
        collect_objects["TestField"]    = "Testing";

        head_object.insert("SelfTestSettings", collect_objects);
        return head_object;
    }

    QJsonObject getLessSettings() {
        QJsonObject head_object;
        QJsonObject collect_objects;

        collect_objects["VersionJSON"]  = VERSION;
        collect_objects["Sound"]        = getSoundJsonObject();

        head_object.insert("SelfTestSettings", collect_objects);
        return head_object;
    }

private:
    QJsonObject getWindowJsonObject(const int new_values = -1) {
        QJsonObject object;
        if (new_values != -1) {
            object["width"] = new_values;
            object["height"] = new_values;
        } else {
            object["width"] = 1600;
            object["height"] = 900;
        }

        return object;
    }

    QJsonObject getSoundJsonObject(const int new_values = -1) {
        QJsonObject object;
        if (new_values != -1) {
            object["InputVolume"]             = new_values;
            object["OutputVolume"]            = new_values;
            object["InputHeadphoneVolume"]    = new_values;
            object["OutputHeadphoneVolume"]   = new_values;
        } else {
            object["InputVolume"]             = 100;
            object["OutputVolume"]            = 100;
            object["InputHeadphoneVolume"]    = 100;
            object["OutputHeadphoneVolume"]   = 100;
        }

        return object;
    }

private:
    std::filesystem::path m_prefer_path;
};

///////////////////////////////////////////////////

class SettingsTests : public ::testing::Test {
public:
    // Settings* contact_settings;
    Settings* user_settings;

protected:
    static void SetUpTestSuite() {
        general_test_path = std::filesystem::current_path() / "tests";
        settings_test_path = general_test_path / "settings";
        std::filesystem::create_directories(settings_test_path);

        json_files_dir = std::filesystem::path(TESTS_SOURCE_DIR) / "settings" / "json_settings";
    }

    static void TearDownTestSuite() {

    }

    void SetUp() override {
        // contact_settings = new ContactSettings();
        user_settings = new UserSettingsTests(settings_test_path);
        user_settings->settingsInit();
    }

    void TearDown() override {
        for (const auto& file : std::filesystem::recursive_directory_iterator(settings_test_path)) {
            std::filesystem::remove(file);
        }
    }
};

///////////////////////////////////////////////////

TEST_F(SettingsTests, SetDefaultSettings) {
    ASSERT_TRUE(std::filesystem::exists(settings_test_path / user_settings->getFileName()));

    auto* test_settings = dynamic_cast<UserSettingsTests*>(user_settings);
    ASSERT_NE(test_settings, nullptr);
    const QString json_from_file = test_settings->readFileToString().value_or("");

    QFile file((json_files_dir / "user_settings_default.json").c_str());
    ASSERT_TRUE(file.open(QIODevice::ReadOnly | QIODevice::Text));

    const QString default_settings = file.readAll();
    file.close();

    ASSERT_EQ(default_settings, json_from_file);
}

// TEST_F(SettingsTests, AddedNewJsonField) {
//     ASSERT_TRUE(std::filesystem::exists(settings_test_path / user_settings->getFileName()));
//     const QString json_content_with_new_field = R"JSON_CONTENT({
//     "SelfSettings": {
//         "Avatar": "Path to default avatar",
//         "ChatMemoryLimit": 1048576,
//         "ChatsHistory": "Path to current directory",
//         "NewField": "Test new field",
//         "Resolution": {
//             "height": 900,
//             "width": 1600
//         },
//         "Sound": {
//             "InputHeadphoneVolume": 100,
//             "InputVolume": 100,
//             "OutputHeadphoneVolume": 100,
//             "OutputVolume": 100
//         },
//         "Theme": "default",
//         "VersionJSON": 2,
//         "VideoQuality": "We Will See In The Future"
//     }
// }
// )JSON_CONTENT";

//     QFile file((settings_test_path / user_settings->getFileName()).c_str());
//     ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));

//     QTextStream stream(&file);
//     stream << json_content_with_new_field;
//     file.close();

//     const QString new_settings_json = user_settings->readFileToString().value_or("");
//     ASSERT_EQ(json_content_with_new_field, new_settings_json);
// }

// // TODO: Create testutils

// TEST_F(SettingsTests, RemovedJsonField) {
//     ASSERT_TRUE(std::filesystem::exists(settings_test_path / user_settings->getFileName()));
//     const QString json_content_without_field = R"JSON_CONTENT({
//     "SelfSettings": {
//         "Avatar": "Path to default avatar",
//         "ChatMemoryLimit": 1048576,
//         "Resolution": {
//             "height": 900,
//             "width": 1600
//         },
//         "Sound": {
//             "InputHeadphoneVolume": 100,
//             "InputVolume": 100,
//             "OutputHeadphoneVolume": 100,
//             "OutputVolume": 100
//         },
//         "Theme": "default",
//         "VersionJSON": 2,
//         "VideoQuality": "We Will See In The Future"
//     }
// }
// )JSON_CONTENT";

//     QFile file((settings_test_path / user_settings->getFileName()).c_str());
//     ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));

//     QTextStream stream(&file);
//     stream << json_content_without_field;
//     file.close();

//     const QString new_settings_json = user_settings->readFileToString().value_or("");
//     ASSERT_EQ(json_content_without_field, new_settings_json);
// }
