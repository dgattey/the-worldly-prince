#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>

/**
 * @brief  Stores application settings for the GUI
 *
 * Access to all app settings is through the "settings" global variable.
 * The settings will be automatically updated when things are changed in the
 * GUI (the reverse is not true however: changing the value of a setting does
 * not update the GUI).
 */
class Settings {
public:
    // Loads settings from disk, or fills in default values if no saved settings exist.
    void loadSettingsOrDefaults();

    // Saves the current settings to disk.
    void saveSettings();

    // Gives the next texture
    int getAndIncrementTextureIndex();

private:
    int textureIndex;
};

// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
