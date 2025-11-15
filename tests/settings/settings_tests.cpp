#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QFile>
#include <QTextStream>

#include <filesystem>
#include <memory>
#include <system_error>

#include "../../settings/settings.h"


static std::filesystem::path general_test_path;
static std::filesystem::path settings_test_path;
static QString default_json_content;

class SettingsTests : public ::testing::Test {
public:
    // Settings* contact_settings;
    Settings* user_settings;

protected:
    static void SetUpTestSuite() {
        general_test_path = std::filesystem::current_path() / "tests";
        settings_test_path = general_test_path / "settings";
        std::filesystem::create_directories(settings_test_path);

        default_json_content = R"JSON_CONTENT({
    "SelfSettings": {
        "Avatar": "Path to default avatar",
        "ChatMemoryLimit": 1048576,
        "ChatsHistory": "Path to current directory",
        "Resolution": {
            "height": 900,
            "width": 1600
        },
        "Sound": {
            "InputHeadphoneVolume": 100,
            "InputVolume": 100,
            "OutputHeadphoneVolume": 100,
            "OutputVolume": 100
        },
        "Theme": "default",
        "VersionJSON": 1,
        "VideoQuality": "We Will See In The Future"
    }
}
)JSON_CONTENT";
    }

    static void TearDownTestSuite() {

    }

    void SetUp() override {
        // contact_settings = new ContactSettings();
        user_settings = new UserSettings(settings_test_path);
        user_settings->settingsInit();
    }

    void TearDown() override {
        for (const auto file : std::filesystem::recursive_directory_iterator(settings_test_path)) {
            std::filesystem::remove(file);
        }
    }
};

TEST_F(SettingsTests, SetDefaultSettings) {
    ASSERT_TRUE(std::filesystem::exists(settings_test_path / user_settings->getFileName()));

    const QString json_from_file = user_settings->readFileToString().value_or("");
    ASSERT_EQ(default_json_content, json_from_file);
}

TEST_F(SettingsTests, AddedNewJsonField) {
    ASSERT_TRUE(std::filesystem::exists(settings_test_path / user_settings->getFileName()));
    const QString json_content_with_new_field = R"JSON_CONTENT({
    "SelfSettings": {
        "Avatar": "Path to default avatar",
        "ChatMemoryLimit": 1048576,
        "ChatsHistory": "Path to current directory",
        "NewField": "Test new field",
        "Resolution": {
            "height": 900,
            "width": 1600
        },
        "Sound": {
            "InputHeadphoneVolume": 100,
            "InputVolume": 100,
            "OutputHeadphoneVolume": 100,
            "OutputVolume": 100
        },
        "Theme": "default",
        "VersionJSON": 2,
        "VideoQuality": "We Will See In The Future"
    }
}
)JSON_CONTENT";

    QFile file((settings_test_path / user_settings->getFileName()).c_str());
    ASSERT_TRUE(file.open(QIODevice::WriteOnly | QIODevice::Text));

    QTextStream stream(&file);
    stream << json_content_with_new_field;
    file.close();

    const QString new_settings_json = user_settings->readFileToString().value_or("");
    ASSERT_EQ(json_content_with_new_field, new_settings_json);
}

// TODO: Create testutils
