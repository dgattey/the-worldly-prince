#include "PlanetDataParser.h"
#include "ResourceLoader.h"
#include <QFile>

PlanetDataParser::PlanetDataParser(const char * file) {
    QString fileLoaded = ResourceLoader::fileToString(file);
    QXmlStreamReader xml(fileLoaded);
    parse(xml);
}

PlanetDataParser::~PlanetDataParser() {

}

QList<int> PlanetDataParser::getResolutions() {
    return m_resolutions.values(); // TODO: Make it return the QHash and use the QHash better?
}

QHash<QString, PlanetData> PlanetDataParser::getPlanets() {
    return m_planets;
}

void PlanetDataParser::parse(QXmlStreamReader &xml) {
    while(!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (!xml.isStartElement()) continue;

        // We only care about start elements - check which case it belongs to
        QStringRef name = xml.name();
        if(name == "resolutions") parseResolutions(xml);
        else if (name == "colors") parseColors(xml);
        else if (name == "planets") parsePlanets(xml);
        else if (name == "data") continue; // top level element

        // Anything else is unexpected, so throw error
        else throwError(xml, "Unexpected top level token %s", name.toString());
    }

    // Handles errors by clearing all data and writing to user
    if (xml.hasError()) {
        throwError(xml, "%s", xml.errorString());
        return;
    }

    // Reset to original state and delete
    xml.clear();
}

void PlanetDataParser::parseColors(QXmlStreamReader &xml) {
    glm::vec4 currColor = glm::vec4(-1);
    while(!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Back to main parser
        if (xml.isEndElement() && xml.name() == "colors") return;

        // Start of an element - figure out which one it is
        if (xml.isStartElement()){
            QStringRef color = xml.name();
            currColor = glm::vec4(-1);
            foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
                QStringRef val = attr.value();
                QStringRef name = attr.name();
                if (name == "noisebase") currColor.a = parseFloat(xml, val);
                else if (name == "r") currColor.r = parseInt(xml, val)/255.0f;
                else if (name == "g") currColor.g = parseInt(xml, val)/255.0f;
                else if (name == "b") currColor.b = parseInt(xml, val)/255.0f;
                else throwError(xml,"Colors - Unexpected attribute: %s", name.toString());
            }
            if (currColor.r < 0 || currColor.g < 0 || currColor.b < 0 || currColor.a < 0)
                throwError(xml,"Current color has negative value (%s)", color.toString());
            m_colors.insert(color.toString(), currColor);
        }
    }
}

void PlanetDataParser::parseResolutions(QXmlStreamReader &xml) {
    QStringRef name = NULL;
    while(!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Back to main parser
        if (xml.isEndElement() && xml.name() == "resolutions") return;

        // Start of an element, so save/delete the name
        if (xml.isStartElement() && name == NULL) name = xml.name();
        else if (xml.isStartElement()) throwError(xml,"Saw extra name: %s", xml.name().toString());
        else if (xml.isEndElement() && xml.name() == name) name = NULL;

        // Number! We have all we need
        else if (xml.isCharacters() && !xml.isWhitespace()) {
            if (name == NULL) throwError(xml, "In resolutions, saw a number without a name");
            else m_resolutions.insert(name.toString(), parseInt(xml));
        }
    }
}

void PlanetDataParser::parsePlanets(QXmlStreamReader &xml) {
    while(!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Back to main parser
        if (xml.isEndElement() && xml.name() == "planets") return;

        // We only care about the starting planet element
        else if (xml.isStartElement() && xml.name() == "planet") {
            QStringRef planetName = NULL;
            QXmlStreamAttributes attrs = xml.attributes();
            if (attrs.size() > 1) throwError(xml, "Too many attributes for planet (%d)", attrs.size());
            QXmlStreamAttribute attr = xml.attributes().at(0);
            if (attr.name() == "name") {
                planetName = attr.value();
                parsePlanet(xml, planetName.toString());
            }
            else throwError(xml,"Unexpected attribute: %s", attr.name().toString());
        }

        // Problem
        else if (!xml.isWhitespace() && xml.isCharacters()) {
            throwError(xml, "Planet token not found (%s instead)", xml.text().toString());
            return;
        }
    }
}

void PlanetDataParser::parsePlanet(QXmlStreamReader &xml, QString planetName) {
    QStringRef currTag = NULL;
    PlanetData data = PlanetData();
    data.name = planetName;
    while(!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Back to planets parser - save data
        if (xml.isEndElement() && xml.name() == "planet") {
            m_planets.insert(planetName, data);
            return;
        }

        // Check the start and end tags and save tag and move on if need be
        else if (xml.isStartElement() && currTag == NULL) {
            currTag = xml.name();
            if (currTag == "tilt") data.tilt = parseVec3(xml);
            else if (currTag == "position") data.position = parseVec3(xml);
            else if (currTag == "color") data.color = parsePlanetColor(xml);
            else if (xml.attributes().size() > 0) throwError(xml, "Extra attributes on tag %s", currTag.toString());
        }
        else if (xml.isStartElement()) throwError(xml, "No nesting allowed (planet): %s", xml.name().toString());
        else if (xml.isEndElement() && xml.name() == currTag) currTag = NULL;

        // Parse other tags
        else if (!xml.isWhitespace() && xml.isCharacters()) {
            QStringRef text = xml.text();
            if (currTag == "resolution") data.resolution = m_resolutions.value(text.toString());
            else if (currTag == "size") data.size = parseFloat(xml, text);
            else if (currTag == "dayLength") data.day = parseFloat(xml, text);
            else if (currTag == "yearLength") data.year = parseFloat(xml, text);
            else throwError(xml, "Unexpected token found in planet (%s)", xml.text().toString());
        }
    }
}

glm::vec3 PlanetDataParser::parseVec3(QXmlStreamReader &xml, bool only) {
    glm::vec3 v(-1);
    foreach(const QXmlStreamAttribute &attr, xml.attributes()) {
        if (attr.name() == "x") v.x = parseFloat(xml, attr.value());
        else if (attr.name() == "y") v.y = parseFloat(xml, attr.value());
        else if (attr.name() == "z") v.z = parseFloat(xml, attr.value());
        else if (only) throwError(xml,"Unexpected attribute: %s", attr.name().toString());
    }
    return v;
}

int PlanetDataParser::parseInt(QXmlStreamReader &xml) {
    QStringRef ref = xml.text();
    return parseInt(xml, ref);
}

int PlanetDataParser::parseInt(QXmlStreamReader &xml, QStringRef ref) {
    bool okay = true;
    int res = ref.toInt(&okay);
    if (!okay) throwError(xml, "Expecting int but saw \"%s\"", ref.toString());
    return res;
}

float PlanetDataParser::parseFloat(QXmlStreamReader &xml) {
    QStringRef ref = xml.text();
    return parseFloat(xml, ref);
}

float PlanetDataParser::parseFloat(QXmlStreamReader &xml, QStringRef ref) {
    bool okay = true;
    float res = ref.toFloat(&okay);
    if (!okay) throwError(xml, "Expecting float but saw \"%s\"", ref.toString());
    return res;
}

/**
 * @brief Parses either attributes into a color or text itself into a color
 * @param xml
 * @return
 */
PlanetColor PlanetDataParser::parsePlanetColor(QXmlStreamReader &xml) {
    PlanetColor color = PlanetColor();
    QXmlStreamAttributes attrs = xml.attributes();

    // Parse the attributes
    if (attrs.size() > 0) {
        foreach(const QXmlStreamAttribute &attr, attrs) {
            QString val = attr.value().toString();
            QStringRef name = attr.name();
            if (name == "low") {
                if (!m_colors.contains(val)) throwError(xml, "Color for planet not found (%s)", val);
                color.low = m_colors.value(val);
            }
            else if (name == "high") {
                if (!m_colors.contains(val)) throwError(xml, "Color for planet not found (%s)", val);
                color.high = m_colors.value(val);
            }
            else if (name == "threshold") color.threshold = parseFloat(xml, attr.value());
            else throwError(xml,"Unexpected attribute: %s", name.toString());
        }
    }

    // Parse the text itself
    else {
        xml.readNext();
        if (!(xml.isCharacters() && !xml.isWhitespace()))
            throwError(xml, "Unexpected data in color tag (%s)", xml.text().toString());
        QString val = xml.text().toString();
        if (!m_colors.contains(val)) throwError(xml, "Color for planet not found (%s)", val);
        color.low = m_colors.value(val);
        color.high = color.low;
    }

    return color;
}

void PlanetDataParser::errorBegin() {
    m_planets.clear();
    m_resolutions.clear();
    m_colors.clear();

    fprintf(stderr, "Error parsing XML: ");
}

void PlanetDataParser::errorEnd(QXmlStreamReader &xml) {
    fprintf(stderr, "\n");
    xml.clear();
}

void PlanetDataParser::throwError(QXmlStreamReader &xml, const char *msg, QString error) {
    errorBegin();
    const char *errString = error.toStdString().c_str();
    if (strlen(errString) > 0) fprintf(stderr, msg, errString);
    else fprintf(stderr, "%s", msg);
    errorEnd(xml);
}

void PlanetDataParser::throwError(QXmlStreamReader &xml, const char *msg, int num) {
    errorBegin();
    fprintf(stderr, msg, num);
    errorEnd(xml);
}
