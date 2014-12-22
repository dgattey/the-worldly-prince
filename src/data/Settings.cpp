#include "Settings.h"
#include <QFile>
#include <QSettings>

Settings settings;

/**
 * Loads the application settings, or, if no saved settings are available,
 * loads default values for the settings. You can change the defaults here.
 */
void Settings::loadSettingsOrDefaults() {
    // Set the default values below
    QSettings s;

    // Load texture index
    textureIndex = s.value("textureIndex", 0).toInt();
}

void Settings::saveSettings() {
    QSettings s;

    // Save texture index
    s.setValue("textureIndex", textureIndex);
}

int Settings::getAndIncrementTextureIndex() {
    QSettings s;

    // Get and increment
    int tex = textureIndex;
    textureIndex++;
    s.setValue("textureIndex", textureIndex);
    return tex;
}
