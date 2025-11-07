#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

class Settings : public QWidget {
    Q_OBJECT
public:
    Settings();
    ~Settings();
};

class ContactSettings : public Settings {
    Q_OBJECT
public:
    ContactSettings();
    ~ContactSettings();
};

class UserSettings : public Settings {
    Q_OBJECT
public:
    UserSettings();
    ~UserSettings();
};

#endif  // SETTINGS_H
