#include <gtest/gtest.h>
#include "../../src/core/settings/settings.h"

class SettingsTest : public ::testing::Test {
public:
    MainSettings main_settings;

protected:
    void SetUp() override {
        main_settings = MainSettings();
        main_settings.resetToDefaults();
    }

    void TearDown() override {

    }
};

TEST_F(SettingsTest, GetValue_ReturnsCorrectValue) {
    EXPECT_EQ(main_settings.getValue("Theme").toString(), "default");
    EXPECT_EQ(main_settings.getValue("Language").toString(), "en");
    EXPECT_EQ(main_settings.getValue("ChatMemoryLimit").toInt(), 1 * 1024 * 1024);

    EXPECT_EQ(main_settings.getValue("Window.Resolution.Width").toInt(), 1600);
    EXPECT_EQ(main_settings.getValue("Window.Resolution.Height").toInt(), 900);
    EXPECT_EQ(main_settings.getValue("Sound.Devices.InputDevice").toString(), "default");
    EXPECT_EQ(main_settings.getValue("Sound.Volume.OutputVolume").toInt(), 100);
}

TEST_F(SettingsTest, GetValue_ReturnsDefault_WhenKeyNotFound) {
    QJsonValue defaultVal = "not_found_default";

    EXPECT_EQ(main_settings.getValue("NonExisting.Key", defaultVal), defaultVal);
    EXPECT_EQ(main_settings.getValue("Window.NonExisting", defaultVal), defaultVal);
}


TEST_F(SettingsTest, SetValue_SimpleField) {
    main_settings.setValue("Theme", "dark");
    EXPECT_EQ(main_settings.getValue("Theme").toString(), "dark");
}

TEST_F(SettingsTest, SetValue_NestedField) {
    main_settings.setValue("Window.Resolution.Width", 1920);
    main_settings.setValue("Sound.Volume.OutputVolume", 75);

    EXPECT_EQ(main_settings.getValue("Window.Resolution.Width").toInt(), 1920);
    EXPECT_EQ(main_settings.getValue("Sound.Volume.OutputVolume").toInt(), 75);
}

TEST_F(SettingsTest, SetValue_CreatesIntermediateObjects) {
    main_settings.setValue("NewSection.SubSection.DeepValue", 42);

    EXPECT_EQ(main_settings.getValue("NewSection.SubSection.DeepValue").toInt(), 42);
    EXPECT_TRUE(main_settings.getValue("NewSection").isObject());
    EXPECT_TRUE(main_settings.getValue("NewSection.SubSection").isObject());
}

TEST_F(SettingsTest, SetValue_OverwritesExistingValue) {
    main_settings.setValue("Window.StartMaximized", true);
    EXPECT_TRUE(main_settings.getValue("Window.StartMaximized").toBool());
}


TEST_F(SettingsTest, MigrateSettings_UpdatesOldVersion) {
    main_settings.setValue("VersionJSON", 0);
    main_settings.setValue("Theme", "old_theme");
    main_settings.setValue("Window.Resolution.Width", 1280);

    main_settings.migrateSettings();

    EXPECT_EQ(main_settings.getValue("VersionJSON").toInt(), main_settings.getCurrentVersion());

    EXPECT_EQ(main_settings.getValue("Theme").toString(), "old_theme");
    EXPECT_EQ(main_settings.getValue("Window.Resolution.Width").toInt(), 1280);
    EXPECT_EQ(main_settings.getValue("Language").toString(), "en");
}

TEST_F(SettingsTest, MigrateSettings_AddsMissingFields) {
    main_settings.setValue("VersionJSON", 0);

    main_settings.migrateSettings();

    EXPECT_EQ(main_settings.getValue("VersionJSON").toInt(), main_settings.getCurrentVersion());
    EXPECT_FALSE(main_settings.getValue("Window").isUndefined());
    EXPECT_FALSE(main_settings.getValue("Sound").isUndefined());
    EXPECT_FALSE(main_settings.getValue("Theme").isUndefined());
}

TEST_F(SettingsTest, MigrateSettings_PreservesExistingNestedValues) {
    main_settings.setValue("VersionJSON", 0);
    main_settings.setValue("Window.Resolution.Width", 1366);
    main_settings.setValue("Window.StartMaximized", true);
    main_settings.setValue("Sound.Volume.OutputVolume", 50);

    main_settings.migrateSettings();

    EXPECT_EQ(main_settings.getValue("Window.Resolution.Width").toInt(), 1366);
    EXPECT_TRUE(main_settings.getValue("Window.StartMaximized").toBool());
    EXPECT_EQ(main_settings.getValue("Sound.Volume.OutputVolume").toInt(), 50);

    EXPECT_EQ(main_settings.getValue("Sound.Devices.InputDevice").toString(), "default");
}
