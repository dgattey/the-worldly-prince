#ifndef PLANETDATA_H
#define PLANETDATA_H

#include "ResourceLoader.h"

/**
 * @brief Represents all data for a color of a planet
 */
struct PlanetColor {
    /**
     * @brief Sets up default arguments
     */
    PlanetColor() : low(glm::vec4(1)), high(glm::vec4(1)), threshold(0) {}

    glm::vec4 low;
    glm::vec4 high;
    float threshold;
};

/**
 * @brief Represents all data for a given planet
 */
struct PlanetData {
    /**
     * @brief Sets up default arguments
     */
    PlanetData() : name(""), size(1), tilt(glm::vec3(0)), day(1), year(1),
        position(glm::vec3(0)), color(PlanetColor()), resolution(5) {}

    QString name;
    float size;
    glm::vec3 tilt;
    float day;
    float year;
    glm::vec3 position;
    PlanetColor color;
    int resolution;
};

/**
 * @brief Class used to load in/parse planet data
 * Given a file to load in, can create a list of resolutions,
 * a list of colors, and a list of planets separately. Assumes
 * formatted correctly - but will print informative errors if
 * wrong
 */
class PlanetDataParser {
public:
    PlanetDataParser(const char *file);
    ~PlanetDataParser();

    QList<int> getResolutions();
    QHash<QString, PlanetData> getPlanets();

private:
    void parse(QXmlStreamReader &xml);
    void parseResolutions(QXmlStreamReader &xml);
    void parseColors(QXmlStreamReader &xml);
    void parsePlanets(QXmlStreamReader &xml);
    void parsePlanet(QXmlStreamReader &xml, QString planetName);

    glm::vec3 parseVec3(QXmlStreamReader &xml, bool only = true);
    int parseInt(QXmlStreamReader &xml);
    int parseInt(QXmlStreamReader &xml, QStringRef ref);
    float parseFloat(QXmlStreamReader &xml);
    float parseFloat(QXmlStreamReader &xml, QStringRef ref);
    PlanetColor parsePlanetColor(QXmlStreamReader &xml);

    void throwError(QXmlStreamReader &xml, const char *msg, QString error = 0);
    void throwError(QXmlStreamReader &xml, const char *msg, int error);
    void errorBegin();
    void errorEnd(QXmlStreamReader &xml);

    QHash<QString, int> m_resolutions; // Need QString for name
    QHash<QString, PlanetData> m_planets;
    QHash<QString, glm::vec4> m_colors; // 4th component is noisebase
};

#endif // PLANETDATA_H
