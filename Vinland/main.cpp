#include <iostream>
#include <windows.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

int WIDTH = 1280;
int HEIGHT = 960;
float FPS = 1000 / 60.f;

void update();
void display();
void reshape(int, int);

// weather
float weatherTImer = 0;
bool clearWeather = false;
bool rainy = false;
bool snow = false;
bool rainbow = true;

// time
int dayTimer = 0;
int nightTimer = 0;
bool morning = true;
bool evening = false;
bool noon = false;
bool night = false;
bool midnight = false;

// sun
GLfloat sunXPos = - WIDTH / 2.0f, sunYPos = 0.0f;
float sunAngle, sunSegments = 100.0f, sunRadius = 100.0f;
float sunColor[3] = {1.0f, 0.64f, 0.0f};
float sunColorShade[3] = {1.0f, 1.0f, 0.0f};

// sky
GLfloat skyColor[3] = {0.0f, 0.4f, 0.6f};
GLfloat skyColorShade[3] = {0.9f, 0.9f, 1.0f};
GLfloat skyXPos = - WIDTH, skyYPos = HEIGHT, skyWidth = 2 * WIDTH, skyHeight = HEIGHT;
void sky_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, sky_update, 0);
    if (morning) {
        if (skyColor[1] < 0.4f)
            skyColor[1] += 0.01f;
        if (skyColor[2] < 0.6f)
            skyColor[2] += 0.01f;
        if (skyColorShade[0] < 0.9f)
            skyColorShade[0] += 0.1f;
        if (skyColorShade[1] < 0.9f)
            skyColorShade[1] += 0.1f;
        if (skyColorShade[2] < 1.0f)
            skyColorShade[2] += 0.1f;
    }
    else if (noon && sunYPos <= HEIGHT / 2.0f) {
        if (skyColor[1] > 0.1f)
            skyColor[1] -= 0.01f;
        if (skyColor[2] > 0.1f)
            skyColor[2] -= 0.01f;
        if (skyColorShade[0] > 0.0f)
            skyColorShade[0] -= 0.1f;
        if (skyColorShade[1] > 0.0f)
            skyColorShade[1] -= 0.1f;
        if (skyColorShade[2] > 0.5f)
            skyColorShade[2] -= 0.1f;
    }
}

// stars
vector <vector <GLfloat > > starPositions;

void load_stars() {
    for (float i = - WIDTH + 20.0f; i < WIDTH - 20.0f; i += 50.0f) {
        for (float j = 0.0f; j < HEIGHT - 20.0f; j += 20.0f) {
            float ranConst = rand() % 20 + 50;
            if (fmod(i, ranConst) == 0 || fmod(j, ranConst) == 0) {
                vector <GLfloat > tmpPos;
                tmpPos.push_back(i);
                tmpPos.push_back(j);
                starPositions.push_back(tmpPos);
            }
        }
    }
}

void stars_display() {
    if (starPositions.empty())
        load_stars();
    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < starPositions.size(); i++)
        glVertex2f(starPositions[i][0], starPositions[i][1]);
    glEnd();
}

void sky_display() {
    glBegin(GL_POLYGON);
    glColor3f(skyColor[0], skyColor[1], skyColor[2]);
    glVertex2f(skyXPos, skyYPos);
    glVertex2f(skyXPos + skyWidth, skyYPos);
    glColor3f(skyColorShade[0], skyColorShade[1], skyColorShade[2]);
    glVertex2f(skyXPos + skyWidth, skyYPos - skyHeight);
    glVertex2f(skyXPos, skyYPos - skyHeight);
    glEnd();
    if (!morning && !evening && sunYPos < HEIGHT / 2.0f)
        stars_display();
}

// sun functions

void sun_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, sun_update, 0);
    if (morning) {
        if (sunYPos < HEIGHT)
            sunYPos += 2;
        if (sunYPos == HEIGHT){
            morning = false;
            evening = true;
            weatherTImer += 0.2f;
        }
    }
    else if (evening) {
        dayTimer++;
        if (dayTimer >= FPS * 5) {
            evening = false;
            noon = true;
            dayTimer = 0;
            weatherTImer += 0.2f;
        }
    }
    else if (noon) {
        if (sunYPos > 0)
            sunYPos--;
        if (sunYPos == 0) {
            noon = false;
            night = true;
            weatherTImer += 0.2f;
        }
    }
}

void sun_display() {
    GLfloat sunColorTmp[3];
    sunColorTmp[0] = sunColor[0];
    sunColorTmp[1] = sunColor[1];
    sunColorTmp[2] = sunColor[2];
    glBegin(GL_POLYGON);
    for (int i = 0; i <= sunSegments; i++) {
        if (sunColorTmp[1] < sunColorShade[1])
            sunColorTmp[1] += 0.05f;
        glColor3f(sunColorTmp[0], sunColorTmp[1], sunColorTmp[2]);
        sunAngle = 2.0f * 3.141615f * i / sunSegments;
        glVertex2f((sunRadius * cosf(sunAngle)) + sunXPos, (sunRadius * sinf(sunAngle)) + sunYPos);
    }
    glEnd();
}

// moon
GLfloat moonXPos = - WIDTH / 2.0f, moonYPos = 0.0f;
float moonAngle, moonSegments = 100.0f, moonRadius = 100.0f;
float moonColor[3] = {1.0f, 1.0f, 1.0f};
float moonColorShade[3] = {0.0f, 0.5f, 0.5f};
void moon_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, moon_update, 0);
    if (night) {
        if (moonYPos < HEIGHT - moonRadius)
            moonYPos += 2;
        if (moonYPos == HEIGHT - moonRadius) {
            night = false;
            midnight = true;
            weatherTImer += 0.2f;
        }
    }
    else if (midnight) {
        nightTimer++;
        if (nightTimer >= FPS * 5) {
            midnight = false;
            morning = true;
            nightTimer = 0;
            weatherTImer += 0.2f;
        }
    }
    else if (morning) {
        if (moonYPos < HEIGHT + moonRadius && moonYPos >= HEIGHT - moonRadius)
            moonYPos += 2;
        if (moonYPos == HEIGHT + moonRadius)
            moonYPos = 0.0f;
    }
}

void moon_display() {
    GLfloat moonColorTmp[3];
    moonColorTmp[0] = moonColor[0];
    moonColorTmp[1] = moonColor[1];
    moonColorTmp[2] = moonColor[2];
    glBegin(GL_POLYGON);
    for (int i = 0; i <= moonSegments; i++) {
        if (moonColorTmp[0] > moonColorShade[0])
            moonColorTmp[0] -= 0.01f;
        if (moonColorTmp[1] > moonColorShade[1])
            moonColorTmp[1] -= 0.01f;
        if (moonColorTmp[2] > moonColorShade[2])
            moonColorTmp[2] -= 0.01f;
        glColor3f(moonColorTmp[0], moonColorTmp[1], moonColorTmp[2]);
        moonAngle = 2.0f * 3.141615f * i / moonSegments;
        glVertex2f((moonRadius * cosf(moonAngle)) + moonXPos, (moonRadius * sinf(moonAngle)) + moonYPos);
    }
    glEnd();
}

// bridge
float bridgeColor[3] = {0.0f, 0.0f, 1.0f};
float bridgeColorShade[3] = {0.85f, 0.43f, 0.58f};

// bridge horizon
GLfloat bridgeHorizonX = - WIDTH, bridgeHorizonY = HEIGHT / 5.0f + 100.0f;
GLfloat bridgeHorizonWidth = WIDTH, bridgeHorizonHeight = 20.0f;
void bridge_horizon_display(){
    glBegin(GL_POLYGON);
    glColor3f(bridgeColor[0], bridgeColor[1], bridgeColor[2]);
    glVertex2f(bridgeHorizonX, bridgeHorizonY);
    glVertex2f(bridgeHorizonX, bridgeHorizonY + bridgeHorizonHeight);
    glColor3f(bridgeColorShade[0], bridgeColorShade[1], bridgeColorShade[2]);
    glVertex2f(bridgeHorizonX + bridgeHorizonWidth, bridgeHorizonY + bridgeHorizonHeight);
    glVertex2f(bridgeHorizonX + bridgeHorizonWidth, bridgeHorizonY);
    glEnd();
}
// bridge legs
int bridgeLegCount = 4;
float bridgeLegGap = bridgeHorizonWidth / bridgeLegCount;
GLfloat bridgeLegXStart = - WIDTH, bridgeLegY = 100.f;
GLfloat bridgeLegWidth = 50.0f, bridgeLegHeight = HEIGHT / 2.0f;
void bridge_legs_display() {
    for (int i = 0; i < bridgeLegCount; i++) {
        glBegin(GL_POLYGON);
        glColor3f(bridgeColor[0], bridgeColor[1], bridgeColor[2]);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap)), bridgeLegY);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap) + (bridgeLegWidth / 2.5)), bridgeLegY + bridgeLegHeight);
        glColor3f(bridgeColorShade[0], bridgeColorShade[1], bridgeColorShade[2]);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap) - (bridgeLegWidth / 2.5)) + bridgeLegWidth, bridgeLegY + bridgeLegHeight);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap)) + bridgeLegWidth, bridgeLegY);
        glEnd();
    }
}
// bridge arches
void bridge_arches_display() {
    int archeSteps = 1000;
    float angle;
    float radius = bridgeLegGap / 2.0f;
    for (int i = 0; i < bridgeLegCount; i++) {
        float centerX = - WIDTH + (bridgeLegGap / 1.75f) + (i * bridgeLegGap);
        float centerY = bridgeLegY + bridgeLegHeight;
        glColor3f(bridgeColor[0], bridgeColor[1], bridgeColor[2]);
        glPointSize(2.5f);
        glBegin(GL_POINTS);
        for (int j = archeSteps; j >= archeSteps / 2; j--) {
            angle = 2.0f * 3.141615f * j / archeSteps;
            glVertex2f((radius * cosf(angle) + centerX), (radius * sinf(angle) + centerY));
        }
        glEnd();
    }
    archeSteps = 20;
    for (int i = 0; i < bridgeLegCount; i++) {
        float centerX = - WIDTH + (bridgeLegGap / 1.75f) + (i * bridgeLegGap);
        float centerY = bridgeLegY + bridgeLegHeight;
        for (int j = archeSteps; j >= archeSteps / 2; j--) {
            angle = 2.0f * 3.141615f * j / archeSteps;
            glColor3f(bridgeColor[0], bridgeColor[1], bridgeColor[2]);
            glLineWidth(4.0f);
            glBegin(GL_LINES);
            glVertex2f((radius * cosf(angle) + centerX), (radius * sinf(angle) + centerY));
            glVertex2f((radius * cosf(angle) + centerX), bridgeHorizonY);
            glEnd();
        }
    }
}

// bridge cars
vector <vector <GLfloat > > bridgeCars;
vector <vector <GLfloat > > bridgeCarColors;
float carWidth = 50.0f, carHeight = 20.0f;

void load_bridge_cars() {
    // 1st car
    vector <GLfloat > car1;
    car1.push_back(bridgeHorizonX); // car1 x pos
    car1.push_back(bridgeHorizonY + bridgeHorizonHeight); // car1 y pos
    car1.push_back(1); // car1 direction
    car1.push_back(bridgeHorizonX + bridgeHorizonWidth + carWidth); // car1 last pos
    vector <GLfloat > car1Color;
    car1Color.push_back(0.18f);
    car1Color.push_back(0.11f);
    car1Color.push_back(0.51f);
    bridgeCarColors.push_back(car1Color);
    // 2nd car
    vector <GLfloat > car2;
    car2.push_back(bridgeHorizonX + bridgeHorizonWidth); // car2 x pos
    car2.push_back(bridgeHorizonY + bridgeHorizonHeight); // car2 y pos
    car2.push_back(-1); // car2 direction
    car2.push_back(bridgeHorizonX - carWidth); // car2 last pos
    vector <GLfloat > car2Color;
    car2Color.push_back(1.0f);
    car2Color.push_back(0.11f);
    car2Color.push_back(0.11f);
    bridgeCarColors.push_back(car2Color);

    bridgeCars.push_back(car1);
    bridgeCars.push_back(car2);
}

void append_car(int direction) {
    vector <GLfloat > car;
    if (direction > 0)
        car.push_back(bridgeHorizonX);
    else if (direction < 0)
        car.push_back(bridgeHorizonX + bridgeHorizonWidth);
    car.push_back(bridgeHorizonY + bridgeHorizonHeight);
    car.push_back(direction);
    if (direction > 0)
        car.push_back(bridgeHorizonX + bridgeHorizonWidth + carWidth);
    else if (direction < 0)
        car.push_back(bridgeHorizonX - carWidth);
    bridgeCars.push_back(car);
}

void bridge_cars_update() {
    if (bridgeCars.empty())
        load_bridge_cars();
    else {
        for (int i = 0; i < bridgeCars.size(); i++) {
            if (bridgeCars[i][2] > 0) {
                if (bridgeCars[i][0] < bridgeCars[i][3])
                    bridgeCars[i][0] += bridgeCars[i][2];
                else {
                    bridgeCars.erase(bridgeCars.begin() + i);
                    append_car(1);
                }
            }
            else if (bridgeCars[i][2] < 0) {
                if (bridgeCars[i][0] > bridgeCars[i][3])
                    bridgeCars[i][0] += bridgeCars[i][2];
                else {
                    bridgeCars.erase(bridgeCars.begin() + i);
                    append_car(- 1);
                }
            }
        }
    }
}

void bridge_cars_display() {
    for (int i = 0; i < bridgeCars.size(); i++) {
        glBegin(GL_POLYGON);
        glColor3f(bridgeCarColors[i][0], bridgeCarColors[i][1], bridgeCarColors[i][2]);
        if (bridgeCars[i][2] > 0) {
            glVertex2f(bridgeCars[i][0], bridgeCars[i][1]);
            glVertex2f(bridgeCars[i][0], bridgeCars[i][1] + carHeight);
            glVertex2f(bridgeCars[i][0] + carWidth / 2.0f, bridgeCars[i][1] + carHeight);
            glVertex2f(bridgeCars[i][0] + carWidth, bridgeCars[i][1] + carHeight / 3.0f);
            glVertex2f(bridgeCars[i][0] + carWidth, bridgeCars[i][1]);
        }
        else {
            glVertex2f(bridgeCars[i][0], bridgeCars[i][1]);
            glVertex2f(bridgeCars[i][0], bridgeCars[i][1] + carHeight / 3.0f);
            glVertex2f(bridgeCars[i][0] + carWidth / 2.0f, bridgeCars[i][1] + carHeight);
            glVertex2f(bridgeCars[i][0] + carWidth, bridgeCars[i][1] + carHeight);
            glVertex2f(bridgeCars[i][0] + carWidth, bridgeCars[i][1]);
        }
        glEnd();
    }
}

void bridge_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, bridge_update, 0);
    bridge_cars_update();
    if (morning) {
        if (bridgeColor[0] < 1.0f)
            bridgeColor[0] += 0.005f;
        if (bridgeColor[2] > 0.0f)
            bridgeColor[2] -= 0.005f;
        if (bridgeColorShade[0] < 1.0f)
            bridgeColorShade[0] += 0.005f;
        if (bridgeColorShade[1] < 0.64f)
            bridgeColorShade[1] += 0.005f;
        if (bridgeColorShade[2] > 0.0f)
            bridgeColorShade[2] -= 0.005f;
    }
    if (noon && sunYPos <= HEIGHT / 2.0f) {
        if (bridgeColor[0] > 0.0f)
            bridgeColor[0] -= 0.005f;
        if (bridgeColor[2] < 1.0f)
            bridgeColor[2] += 0.005f;
        if (bridgeColorShade[0] > 0.85f)
            bridgeColorShade[0] -= 0.005f;
        if (bridgeColorShade[1] > 0.43f)
            bridgeColorShade[1] -= 0.005f;
        if (bridgeColorShade[2] < 0.58f)
            bridgeColorShade[2] += 0.005f;
    }
}

void bridge_display() {
    bridge_cars_display();
    bridge_arches_display();
    bridge_horizon_display();
    bridge_legs_display();
}

// buildings
vector <vector<GLfloat > > buildings;
int buildingsCount = 6;
float gapBetweenBuildings = 10.0f;
float buildingMaxHeight = 200.0f;
float buildingColorShades[6][3] = {
    {0.28f, 0.04f, 0.31},
    {0.73f, 0.73, 0.77f},
    {0.13f, 0.13, 0.08f},
    {0.23f, 0.14, 0.17f},
    {0.13f, 0.13, 0.08f},
    {0.73f, 0.73, 0.77f},
};
float buildingColors[6][3] = {
    {0.38f, 0.14f, 0.41},
    {0.83f, 0.83, 0.87f},
    {0.23f, 0.23, 0.18f},
    {0.33f, 0.24, 0.27f},
    {0.23f, 0.23, 0.18f},
    {0.83f, 0.83, 0.87f},
};

float windowColor[3] = {0.0f, 0.3f, 0.4f};

void generate_building_positions(int buildingLen) {
    float buildingsGap = WIDTH / buildingLen;
    for (int i = 0; i < buildingLen; i++) {
        vector <GLfloat > tmpPos;
        if (i == 0) {
            tmpPos.push_back(i * buildingsGap); // xPos
            tmpPos.push_back(buildingMaxHeight * 3.5);
        }
        else {
            tmpPos.push_back((i * buildingsGap) + gapBetweenBuildings); // xPos
            if (i < 3)
                tmpPos.push_back((i + 1) * buildingMaxHeight);
            else
                tmpPos.push_back((i + 1) * buildingMaxHeight / 2);
        }
        buildings.push_back(tmpPos);
    }
}

void buildings_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, buildings_update, 0);

    if (buildings.empty()) {
        generate_building_positions(buildingsCount);
    }
    if (morning) {
        if (windowColor[0] > 0.0f)
            windowColor[0] -= 0.1f;
        if (windowColor[1] > 0.3f)
            windowColor[1] -= 0.1f;
        if (windowColor[2] < 0.4f)
            windowColor[2] += 0.1f;
    }
    else if (night) {
        if (windowColor[0] < 1.0f)
            windowColor[0] += 0.1f;
        if (windowColor[1] < 1.0f)
            windowColor[1] += 0.1f;
        if (windowColor[2] > 0.0f)
            windowColor[2] -= 0.1f;
    }
}

void buildings_display() {
    if (!buildings.empty()) {
        for (int i = 0; i < buildings.size(); i++) {
            float floorHeight = buildings[i][1] / 4;
            for (int j = 4; j > 0; j--) {
                glBegin(GL_POLYGON);
                glColor3f(buildingColorShades[i][0], buildingColorShades[i][1], buildingColorShades[i][2]);
                glVertex2f(buildings[i][0], 0.0f);
                glVertex2f(buildings[i][0], j * floorHeight);
                glColor3f(buildingColors[i][0], buildingColors[i][1], buildingColors[i][2]);
                if (i < buildings.size() - 1) {
                    glVertex2f(buildings[i + 1][0] - gapBetweenBuildings, j * floorHeight);
                }
                else
                    glVertex2f(WIDTH, j * floorHeight);
                glColor3f(0.0f, 0.0f, 0.0f);
                if (i < buildings.size() - 1) {
                    glVertex2f(buildings[i + 1][0] - gapBetweenBuildings, 0.0f);
                }
                else
                    glVertex2f(WIDTH, 0.0f);
                glEnd();
                float windowXLim = WIDTH;
                float windowYLim = j * floorHeight;
                if (i < buildings.size() - 1)
                    windowXLim = buildings[i + 1][0] - gapBetweenBuildings;
                glBegin(GL_POINTS);
                glColor3f(windowColor[0], windowColor[1], windowColor[2]);
                for (int k = buildings[i][0] + 30; k < windowXLim; k += 50) {
                    for (int l = 20.0f; l < windowYLim; l += 20)
                    glVertex2f(k, l);
                }
                glEnd();
            }
        }
    }
}

// boats
vector <vector <GLfloat > > boats;
float boatWidth = 250.0f, boatHeight = 140.0f;
GLfloat boatsBaseColor[3] = {0.0f, 0.0f, 0.0f};
GLfloat boatsTopColor[3] = {1.0f, 1.0f, 1.0f};
GLfloat boatsTopColorShade[3] = {0.6f, 0.6f, 0.6f};
GLfloat boatsWindowColor[3] = {0.0f, 0.2f, 0.7f};
GLfloat boatsWindowColorShade[3] = {0.49f, 0.97f, 1.0f};

void load_boats(float boatXpos) {
    vector <GLfloat> boat;
    boat.push_back(boatXpos); // boat x pos
    boat.push_back(boatWidth); // boat width
    boat.push_back(boatHeight); // boat height
    boat.push_back(- 2); // boat direction and speed
    boat.push_back(- WIDTH - boatWidth); // boat x pos limit
    boats.push_back(boat);
}

void boats_update() {
    if (boats.empty())
        load_boats(WIDTH + boatWidth);
    else if (boats.size() < 2)
        load_boats(WIDTH * 1.5 + boatWidth);
    else {
        for (int i = 0; i < boats.size(); i++) {
            if (boats[i][3] < 0) {
                if (boats[i][0] > boats[i][4])
                    boats[i][0] += boats[i][3];
                else
                    boats.erase(boats.begin() + i);
            }
        }
    }
}

void boats_display(float yPos, int i) {
    if (!boats.empty() && boats.size() > i) {
            glBegin(GL_POLYGON);
            if (i == 0)
                glColor3f(boatsBaseColor[0], boatsBaseColor[1], boatsBaseColor[2]);
            else
                glColor3f(0.8f, 0.0f, 0.0f);
            glVertex2f(boats[i][0], yPos);
            glVertex2f(boats[i][0] + boats[i][1], yPos);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 5.0f)), yPos - boats[i][2] / 3.0f);
            glVertex2f(boats[i][0] + (boats[i][1] / 5.0f), yPos - boats[i][2] / 3.0f);
            glEnd();
            glBegin(GL_POLYGON);
            glColor3f(boatsBaseColor[0], boatsBaseColor[1], boatsBaseColor[2]);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 2.0f)), yPos);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 4.0f)), yPos + boats[i][2] * 1.5f);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 4.2f)), yPos + boats[i][2] * 1.5f);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 2.2f)), yPos);
            glEnd();
            glBegin(GL_POLYGON);
            glColor3f(boatsTopColor[0], boatsTopColor[1], boatsTopColor[2]);
            glVertex2f(boats[i][0] + (boats[i][1] / 8.0f), yPos + boats[i][2] / 8.0f);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 4.0f)), yPos + boats[i][2] * 1.5f);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 4.2f)), yPos + boats[i][2] * 1.5f);
            glColor3f(boatsTopColorShade[0], boatsTopColorShade[1], boatsTopColorShade[2]);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 8.0f)), yPos + boats[i][2] / 4.0f);
            glVertex2f(boats[i][0] + (boats[i][1] - (boats[i][1] / 8.0f)), yPos + boats[i][2] / 6.0f);
            glEnd();
    }
}

// ocean
GLfloat oceanXpos = - WIDTH, oceanYpos = - HEIGHT;
GLfloat oceanWidth = WIDTH * 2.0f, oceanHeight = HEIGHT + 100.0f;
float oceanWaveOffsets = HEIGHT / 4.0f;
float oceanWaveLengths[4] = {50.0f, 50.0f, 50.0f, 50.0f};
GLfloat oceanColor[3] = {0.0f, 0.5f, 1.0f};
GLfloat oceanColorShade[3] = {0.49f, 0.97f, 1.0f};

void ocean_update(int) {
    glutTimerFunc(FPS, ocean_update, 0);
    if (morning) {
        if (oceanColor[1] < 0.5f)
            oceanColor[1] += 0.01f;
        if (oceanColor[2] < 1.0f)
            oceanColor[2] += 0.01f;
        if (oceanColorShade[0] < 0.49f)
            oceanColorShade[0] += 0.01f;
        if (oceanColorShade[1] < 0.97f)
            oceanColorShade[1] += 0.01f;
        if (oceanColorShade[2] < 1.0f)
            oceanColorShade[2] += 0.01f;
    }
    else if (noon && sunYPos <= HEIGHT / 2.0f) {
        if (oceanColor[1] > 0.0f)
            oceanColor[1] -= 0.01f;
        if (oceanColor[2] > 0.5f)
            oceanColor[2] -= 0.01f;
        if (oceanColorShade[0] > 0.05f)
            oceanColorShade[0] -= 0.01f;
        if (oceanColorShade[1] > 0.32f)
            oceanColorShade[1] -= 0.01f;
        if (oceanColorShade[2] > 0.72f)
            oceanColorShade[2] -= 0.01f;
    }
    boats_update();
}

void ocean_waves_display() {
    for (int j = 0; j < 4; j++) {
        if (j == 1) {
            boats_display(((sinf(oceanWaveLengths[j]) * 100) - (oceanWaveOffsets * j)) / 7.0f - 100.0f, 0);
        }
        if (j == 2) {
            boats_display(((sinf(oceanWaveLengths[j]) * 100) - (oceanWaveOffsets * j)) / 7.0f - 300.0f, 1);
        }
        glBegin(GL_POLYGON);
        glColor3f(oceanColor[0], oceanColor[1], oceanColor[2]);
        glVertex2f(- WIDTH, -HEIGHT);
        glColor3f(oceanColorShade[0], oceanColorShade[1], oceanColorShade[2]);
        glVertex2f(- WIDTH, - (oceanWaveOffsets * j));
        for (int i = - WIDTH; i < WIDTH * 2; i += 3) {
            glVertex2f(i * (j + 1), (sinf(oceanWaveLengths[j]) * 100) - (oceanWaveOffsets * j));
            if (oceanWaveLengths[j] > 100.0f)
                oceanWaveLengths[j] = 50.0f;
            if (i % 2 == 0)
                oceanWaveLengths[j] += 0.01f;
        }
        glVertex2f(WIDTH, -HEIGHT);
        glEnd();
    }
}

void ocean_display() {
    glBegin(GL_POLYGON);
    glColor3f(oceanColor[0], oceanColor[1], oceanColor[2]);
    glVertex2f(oceanXpos, oceanYpos);
    glVertex2f(oceanXpos + oceanWidth, oceanYpos);
    glColor3f(oceanColorShade[0], oceanColorShade[1], oceanColorShade[2]);
    glVertex2f(oceanXpos + oceanWidth, oceanYpos + oceanHeight);
    glVertex2f(oceanXpos, oceanYpos + oceanHeight);
    glEnd();
    ocean_waves_display();
    //boats_display();
}

// ocean rocks
void ocean_rocks_display() {
    glBegin(GL_POLYGON);
    glColor3f(0.3f, 0.3f, 0.3f);
    glVertex2f(- 10, 90.0f);
    glVertex2f(- 10, 100.0f);
    int j = 0;
    for (int i = - 10;  i < WIDTH; i += 20) {
        if (j % 2 == 0)
            glVertex2f(i, 110.0f);
        else
            glVertex2f(i, 100.0f);
        j++;
    }
    glVertex2f(WIDTH, 100.0f);
    glVertex2f(WIDTH, 90.0f);
    glEnd();
}

// island
GLfloat islandCenterXpos = - WIDTH, islandCenterYpos = -HEIGHT;
GLfloat islandRadius = WIDTH / 1.5f;
float islandSegments = 20.0f, islandAngle;
GLfloat islandGrassColor[3] = {0.08f, 0.54f, 0.14f};
GLfloat islandGrassColorShade[3] = {0.47f, 1.0f, 0.08f};
GLfloat islandRockXpos = - WIDTH, islandRockYpos = - HEIGHT / 4.5f;
float islandRockWidth = WIDTH / 2.0f;
float islandRockSegments = 20.0f;
float islandRockRad = 150.0f;

void island_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, island_update, 0);
    if (morning) {
        if (islandGrassColor[1] < 0.54f)
            islandGrassColor[1] += 0.1f;
        if (islandGrassColor[2] < 0.14f)
            islandGrassColor[2] += 0.1f;

        if (islandGrassColorShade[0] < 0.47f)
            islandGrassColorShade[0] += 0.1f;
        if (islandGrassColorShade[1] < 1.0f)
            islandGrassColorShade[1] += 0.1f;
    }
    if (noon && sunYPos <= HEIGHT / 2.0f) {
        if (islandGrassColor[1] > 0.34f)
            islandGrassColor[1] -= 0.1f;
        if (islandGrassColor[2] > 0.04f)
            islandGrassColor[2] -= 0.1f;

        if (islandGrassColorShade[0] > 0.27f)
            islandGrassColorShade[0] -= 0.1f;
        if (islandGrassColorShade[1] > 0.4f)
            islandGrassColorShade[1] -= 0.1f;
    }
}

void island_display() {
    glColor3f(0.8f, 0.3f, 0.2f);
    glBegin(GL_POLYGON);
    for (int i = islandSegments / 4; i >= 0; i--) {
        islandAngle = 2.0f * 3.141615f * i / islandSegments;
        if (i % 2 == 0)
            glVertex2f(((islandRadius + 10.0f + (i * 100)) * cosf(islandAngle) + islandCenterXpos), ((islandRadius) * sinf(islandAngle) + islandCenterYpos));
        else
            glVertex2f(((islandRadius + 10.0f) * cosf(islandAngle) + islandCenterXpos), ((islandRadius + 10.0f) * sinf(islandAngle) + islandCenterYpos));
    }
    glVertex2f(- WIDTH, - HEIGHT);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(islandGrassColorShade[0], islandGrassColorShade[1], islandGrassColorShade[2]);
    for (int i = islandSegments / 4; i >= 0; i--) {
        islandAngle = 2.0f * 3.141615f * i / islandSegments;
        if (i <= islandSegments / 8)
            glColor3f(islandGrassColor[0], islandGrassColor[1], islandGrassColor[2]);
        if (i % 2 == 0)
            glVertex2f(((islandRadius + (i * 100)) * cosf(islandAngle) + islandCenterXpos), (islandRadius * sinf(islandAngle) + islandCenterYpos));
        else
            glVertex2f((islandRadius * cosf(islandAngle) + islandCenterXpos), (islandRadius * sinf(islandAngle) + islandCenterYpos));
    }
    glVertex2f(- WIDTH, - HEIGHT);
    glEnd();
    int j = 0;
    glBegin(GL_POLYGON);
    glVertex2f(islandRockXpos, islandRockYpos - 150.0f);
    glVertex2f(islandRockXpos, islandRockYpos);
    for (int i = islandRockXpos; i <= islandRockXpos + islandRockWidth - 100; i += 30) {
        if (j % 5 == 0)
            glVertex2f(i, islandRockYpos + (islandRockRad + (10 * (j + 1))) / 2.0f);
        else
            glVertex2f(i, islandRockYpos + (islandRockRad + (10 * (j + 1))));
        if (i < (islandRockXpos + islandRockWidth - 100) / 2)
            glColor3f(islandGrassColorShade[0], islandGrassColorShade[1], islandGrassColorShade[2]);
        j++;
    }
    glVertex2f(islandRockXpos + islandRockWidth - 40, islandRockYpos - 150.0f);
    j = 0;
    for (int i = islandRockXpos + islandRockWidth - 100; i >= islandRockXpos; i -= 40.0f) {
        if (j % 5 == 0)
            glVertex2f(i, islandRockYpos + (islandRockRad + (10 * (j + 1))) / 2.0f);
        else
            glVertex2f(i, islandRockYpos + (islandRockRad + (10 * (j + 1))));
        if (i < (islandRockXpos + islandRockWidth - 100) / 2)
            glColor3f(islandGrassColorShade[0], islandGrassColorShade[1], islandGrassColorShade[2]);
        j++;
    }
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(islandRockXpos, islandRockYpos - 150);
    j = 0;
    for (int i = islandRockXpos; i <= islandRockXpos + islandRockWidth; i += 30) {
        if (j % 2 == 0) {
            glVertex2f(i, islandRockYpos - 110);
            glVertex2f(i + 5.0f, islandRockYpos - 110);
        }
        else {
            glVertex2f(i, islandRockYpos - 50);
            glVertex2f(i + 5.0f, islandRockYpos - 130);
        }
        if (i < (islandRockXpos + islandRockWidth) / 2.0f)
            glColor3f(islandGrassColor[0], islandGrassColor[1], islandGrassColor[2]);
        j++;
    }
    glVertex2f(islandRockXpos + islandRockWidth, islandRockYpos - 150);
    glEnd();
}

// tree
GLfloat treeStartXpos = - WIDTH / 1.5f, treeStartYpos = - HEIGHT / 2.0f;
GLfloat treeTrunkHeight = 100.0f;
GLfloat treeTrunkBotttomWidth = 25.0f, treeTrunkTopWidth = treeTrunkBotttomWidth;
GLfloat treeBranchLength = 70.0f, treeBranchStartWidth = 20.0f;
int treeLeafSegmentsMax = 100;
float treeLeafSegments = 0.0f;
GLfloat treeGrowthRate = 1.0f;
GLfloat treeBranchArea[4] = {treeStartXpos + 250.0f, treeStartYpos + treeTrunkHeight, treeStartXpos - 250.f, treeStartXpos + (2 * treeTrunkHeight)};
GLfloat treeColor[3] = {0.36f, 0.25f, 0.2f};
GLfloat treeColorShade[3] = {0.82f, 0.49f, 0.17f};
GLfloat treeLeafColor[3] = {0.0f, 1.0f, 0.49f};
GLfloat treeLeafColorShade[3] = {0.23f, 0.8f, 0.44f};
int growthTimer = 0;
bool treeBranchesGrown = false;

vector<vector<GLfloat> > treePositions;
vector<vector<GLfloat> > treeBranchPositions;
vector<vector<GLfloat> > treeLeafPositions;

void tree_color_update(int) {
    glutTimerFunc(FPS, tree_color_update, 0);
    if (morning) {
        if (treeColor[0] > 0.36f)
            treeColor[0] -= 0.01f;
        if (treeColor[1] > 0.25f)
            treeColor[1] -= 0.01f;
        if (treeColor[2] < 0.2f)
            treeColor[2] += 0.01f;
        if (treeColorShade[0] < 0.82f)
            treeColorShade[0] += 0.01f;
        if (treeColorShade[1] < 0.49f)
            treeColorShade[1] += 0.01f;
        if (treeColorShade[2] < 0.17f)
            treeColorShade[2] += 0.01f;

        if (treeLeafColor[1] < 1.0f)
            treeLeafColor[1] += 0.1f;
        if (treeLeafColor[2] < 0.47f)
            treeLeafColor[2] += 0.1f;
        if (treeLeafColorShade[0] < 0.23f)
            treeLeafColorShade[0] += 0.1f;
        if (treeLeafColorShade[1] < 0.8f)
            treeLeafColorShade[1] += 0.1f;
        if (treeLeafColorShade[0] < 0.44f)
            treeLeafColorShade[0] += 0.1f;
    }
    else if (noon && sunYPos <= HEIGHT / 2.0f) {
        if (treeColor[0] < 0.5f)
            treeColor[0] += 0.01f;
        if (treeColor[1] < 0.27f)
            treeColor[1] += 0.01f;
        if (treeColor[2] > 0.1f)
            treeColor[2] -= 0.01f;
        if (treeColorShade[0] > 0.64f)
            treeColorShade[0] -= 0.01f;
        if (treeColorShade[1] > 0.16f)
            treeColorShade[1] -= 0.01f;
        if (treeColorShade[2] > 0.16f)
            treeColorShade[2] -= 0.01f;

        if (treeLeafColor[1] > 0.7f)
            treeLeafColor[1] -= 0.1f;
        if (treeLeafColor[2] > 0.27f)
            treeLeafColor[2] -= 0.1f;
        if (treeLeafColorShade[0] > 0.13f)
            treeLeafColorShade[0] -= 0.1f;
        if (treeLeafColorShade[1] > 0.6f)
            treeLeafColorShade[1] -= 0.1f;
        if (treeLeafColorShade[0] > 0.24f)
            treeLeafColorShade[0] -= 0.1f;
    }
}

void tree_update(int) {
    if (treePositions.empty()) {
        vector<GLfloat> tmp;
        tmp.push_back(treeStartXpos);
        tmp.push_back(treeStartYpos);
        treePositions.push_back(tmp);
    }
    glutPostRedisplay();
    glutTimerFunc(FPS, tree_update, 0);
    if (!morning && sunYPos <= HEIGHT / 1.2f)
        return;
    GLfloat lastXpos = treePositions[treePositions.size() - 1][0];
    GLfloat lastYpos = treePositions[treePositions.size() - 1][1];
    if (lastXpos == treePositions[0][0] && lastYpos <= treeTrunkHeight) {
        vector<GLfloat> tmpPos;
        tmpPos.push_back(lastXpos);
        tmpPos.push_back(lastYpos + treeGrowthRate);
        treePositions.push_back(tmpPos);
        if (treeTrunkTopWidth >= 5.0f)
            treeTrunkTopWidth -= 0.05f;
    }
    else {
        if (treeBranchPositions.empty()) {
            for (int i = 0; i < treePositions.size(); i++) {
                if (i % 50 == 0 && i >= 200) {
                    vector<GLfloat> tmp;
                    tmp.push_back(treePositions[i][0]); // x1
                    tmp.push_back(treePositions[i][1]); // y1
                    tmp.push_back(treePositions[i][0]); // x2
                    tmp.push_back(treePositions[i][1] - treeBranchStartWidth); // y2
                    tmp.push_back(treePositions[i][0]); // x3
                    tmp.push_back(treePositions[i][1]); // y3
                    tmp.push_back(treePositions[i][0]); // x4
                    tmp.push_back(treePositions[i][1] - treeBranchStartWidth); // y4
                    treeBranchPositions.push_back(tmp);
                    vector<GLfloat> tmp2;
                    tmp2.push_back(treePositions[i][0]); // x1
                    tmp2.push_back(treePositions[i][1]); // y1
                    tmp2.push_back(treePositions[i][0]); // x2
                    tmp2.push_back(treePositions[i][1] - treeBranchStartWidth); // y2
                    tmp2.push_back(treePositions[i][0]); // x3
                    tmp2.push_back(treePositions[i][1]); // y3
                    tmp2.push_back(treePositions[i][0]); // x4
                    tmp2.push_back(treePositions[i][1] - treeBranchStartWidth); // y4
                    treeBranchPositions.push_back(tmp2);
                }
            }
        }
        else {
            if (!treeBranchesGrown) {
                for (int i = 0; i < treeBranchPositions.size(); i++) {
                    if (i % 2 == 0) {
                        if (treeBranchPositions[i][5] <= treeBranchLength && treeBranchPositions[i][7] <= treeBranchLength) {
                            treeBranchPositions[i][4] += treeGrowthRate;
                            treeBranchPositions[i][5] += treeGrowthRate;
                            treeBranchPositions[i][6] += treeGrowthRate;
                            treeBranchPositions[i][7] += treeGrowthRate;
                        }
                    }
                    else {
                        if (treeBranchPositions[i][5] <= treeBranchLength && treeBranchPositions[i][7] <= treeBranchLength) {
                            treeBranchPositions[i][4] -= treeGrowthRate;
                            treeBranchPositions[i][5] += treeGrowthRate;
                            treeBranchPositions[i][6] -= treeGrowthRate;
                            treeBranchPositions[i][7] += treeGrowthRate;
                        }
                    }
                }
                if (abs(treeBranchPositions[0][7] - treeBranchPositions[0][3]) >= 351)
                    treeBranchesGrown = true;
            }
        }
        if (treeBranchesGrown) {
            if (treeLeafPositions.empty()) {
                for (int i = 0; i < treeBranchPositions.size(); i++) {
                    vector<GLfloat> tmp;
                    tmp.push_back(treeBranchPositions[i][6]);
                    tmp.push_back(treeBranchPositions[i][7]);
                    treeLeafPositions.push_back(tmp);
                }
            }
        }
    }
}

void tree_trunk_display() {
    glBegin(GL_POLYGON);
    glColor3f(treeColor[0], treeColor[1], treeColor[2]);
    glVertex2f(treePositions[0][0] - treeTrunkBotttomWidth, treePositions[0][1]);
    glVertex2f(treePositions[0][0] + treeTrunkBotttomWidth, treePositions[0][1]);
    glColor3f(treeColorShade[0], treeColorShade[1], treeColorShade[2]);
    glVertex2f(treePositions[treePositions.size() - 1][0] + treeTrunkTopWidth, treePositions[treePositions.size() - 1][1]);
    glVertex2f(treePositions[treePositions.size() - 1][0] - treeTrunkTopWidth, treePositions[treePositions.size() - 1][1]);
    glEnd();
    glBegin(GL_POLYGON);
    int j = 0;
    for (int i = treePositions[0][0] - treeTrunkBotttomWidth - 10.0f; i < treePositions[0][0] + treeTrunkBotttomWidth + 10.0f; i += 2) {
        if (j % 2 == 0)
            glVertex2f(i, treePositions[0][1]);
        else if (i % 3 == 0)
            glVertex2f(i, treePositions[0][1] + 10.0f);
        else
            glVertex2f(i, treePositions[0][1] + 20.0f);
        j++;
    }
    glEnd();
}

void tree_branch_display() {
    for (int i = 0; i < treeBranchPositions.size(); i++) {
        glBegin(GL_POLYGON);
        glColor3f(treeColor[0], treeColor[1], treeColor[2]);
        glVertex2f(treeBranchPositions[i][0], treeBranchPositions[i][1]);
        glVertex2f(treeBranchPositions[i][2], treeBranchPositions[i][3]);
        glColor3f(treeColorShade[0], treeColorShade[1], treeColorShade[2]);
        glVertex2f(treeBranchPositions[i][4], treeBranchPositions[i][5]);
        glVertex2f(treeBranchPositions[i][6], treeBranchPositions[i][7]);
        glEnd();
    }
}

void tree_leafs_display() {
    float leafAngle, leafRadius;
    if (treeLeafSegments < treeLeafSegmentsMax)
        treeLeafSegments += 0.8f;
    for (int i = 0; i < treeLeafPositions.size(); i++) {
        GLfloat treeLeafColorTmp[3];
        treeLeafColorTmp[0] = treeLeafColor[0];
        treeLeafColorTmp[1] = treeLeafColor[1];
        treeLeafColorTmp[2] = treeLeafColor[2];
        glBegin(GL_POLYGON);
        for (int j = 0; j < treeLeafSegments; j++) {
            /*if (j < treeLeafSegments / 2)
                glColor3f(treeLeafColor[0], treeLeafColor[1], treeLeafColor[2]);
            else
                glColor3f(treeLeafColorShade[0], treeLeafColorShade[1], treeLeafColorShade[2]);*/
            glColor3f(treeLeafColorTmp[0], treeLeafColorTmp[1], treeLeafColorTmp[2]);
            leafAngle = 2.0f * 3.141615f * j / 100;
            leafRadius = (15 * (i + 1));
            if (j % 10 == 0)
                glVertex2f((leafRadius * cosf(leafAngle)) + treeLeafPositions[i][0], (leafRadius * sinf(leafAngle)) + treeLeafPositions[i][1]);
            else if (j % 5 == 0)
                glVertex2f((leafRadius * cosf(leafAngle)) / 2 + treeLeafPositions[i][0], (leafRadius * sinf(leafAngle)) / 2 + treeLeafPositions[i][1]);
            if (treeLeafColorTmp[0] < treeLeafColorShade[0])
                treeLeafColorTmp[0] += 0.01f;
            if (treeLeafColorTmp[1] > treeLeafColorShade[1])
                treeLeafColorTmp[1] -= 0.01f;
            if (treeLeafColorTmp[2] > treeLeafColorShade[2])
                treeLeafColorTmp[2] -= 0.01f;
        }
        glEnd();
    }
}

void tree_display() {
    tree_branch_display();
    tree_trunk_display();
    if (treeBranchesGrown)
        tree_leafs_display();
}

vector <vector <GLfloat> > rainPositions;
float rainSpeed = 50.0f;
float rainDropLengths = 50.0f;

void reload_rain_positions() {
    int j;
    for (int i = - WIDTH; i <= WIDTH; i += 100) {
        j = rand() % 100;
        if (j % 5 == 0) {
            vector <GLfloat > tmp;
            tmp.push_back(i);
            tmp.push_back(HEIGHT);
            rainPositions.push_back(tmp);
        }
    }
}

void rain_update() {
    if (rainPositions.empty()) {
        reload_rain_positions();
    }
    else {
        for (int i = 0; i < rainPositions.size(); i++) {
            if (rainPositions[i][1] < -HEIGHT)
                rainPositions.erase(rainPositions.begin() + i);
            else {
                rainPositions[i][1] -= rainSpeed;
            }
        }
        if (rainPositions[0][1] < HEIGHT - 100.0f)
            reload_rain_positions();
    }
}

void rain_display() {
    if (!rainPositions.empty()) {
        for (int i = 0; i < rainPositions.size(); i++) {
            glBegin(GL_POLYGON);
            glColor4f(0.0f, 0.4f, 0.5f, 0.2f);
            glVertex2f(rainPositions[i][0], rainPositions[i][1]);
            glVertex2f(rainPositions[i][0] + 5.0f, rainPositions[i][1]);
            glColor4f(0.0f, 0.2f, 0.5f, 0.3f);
            glVertex2f(rainPositions[i][0], rainPositions[i][1] + rainDropLengths);
            glVertex2f(rainPositions[i][0] - 5.0f, rainPositions[i][1] + rainDropLengths);
            glEnd();
        }
    }
}

// clouds
int cloudCount = 10;
vector<vector<GLfloat> > clouds;
GLfloat cloudColor[3] = {0.5f, 0.5f, 0.5f};
GLfloat cloudColorShade[3] = {1.0f, 1.0f, 1.0f};
float cloudSpeed = 2.0f;
float cloudMaxLength = 70.0f;
float cloudMinLength = 40.0f;
float cloudMaxRad = 30.0f;
float cloudMinRad = 20.0f;
float cloudSegments = 100.0f;

void reload_clouds(int xOffset) {
    float xPos = - WIDTH - (xOffset * 300);
    float yPos = HEIGHT - (rand() % 100 + 200);
    float length = rand() % (int)cloudMinLength + (cloudMaxLength - cloudMinLength);
    vector<GLfloat > tmpPos;
    tmpPos.push_back(xPos);
    tmpPos.push_back(yPos);
    tmpPos.push_back(length);
    clouds.push_back(tmpPos);
}

void add_cloud() {
    float xPos = - WIDTH;
    float yPos = HEIGHT - (rand() % 100 + 200);
    float length = rand() % (int)cloudMinLength + (cloudMaxLength - cloudMinLength);
    vector<GLfloat > tmpPos;
    tmpPos.push_back(xPos);
    tmpPos.push_back(yPos);
    tmpPos.push_back(length);
    clouds.push_back(tmpPos);
}

void cloud_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, cloud_update, 0);
    if (clouds.empty()) {
        for (int i = 0; i < cloudCount; i++) {
            reload_clouds(i);
        }
    }
    else if (rainy)
        return;
    else {
        if (clouds.size() < cloudCount) {
            while (clouds.size() < cloudCount) {
                add_cloud();
            }
        }
        else {
            for (int i = 0; i < clouds.size(); i++) {
                if (clouds[i][0] > WIDTH)
                    clouds.erase(clouds.begin() + i);
                else
                    clouds[i][0] += cloudSpeed;
            }
        }
    }
}

void cloud_display() {
    if (!clouds.empty()) {
        float cloudAngle;
        float radiusConst = 0.0f;
        for (int i = 0; i < clouds.size(); i++) {
            for (int j = 0; j < clouds[i][2] / 5; j++) {
                GLfloat cloudColorTmp[3];
                cloudColorTmp[0] = cloudColorShade[0];
                cloudColorTmp[1] = cloudColorShade[1];
                cloudColorTmp[2] = cloudColorShade[2];
                if (j < clouds[i][2] / 10)
                    radiusConst = j + 1;
                else
                    radiusConst = ((clouds[i][2] / 5) - j) + 1;
                glBegin(GL_POLYGON);
                for (int k = 0; k <= cloudSegments / 2; k++) {
                    cloudAngle = 2.0f * 3.141615f * k / cloudSegments;
                    glColor3f(cloudColorTmp[0], cloudColorTmp[1], cloudColorTmp[2]);
                    glVertex2f(((cloudMinRad * radiusConst) * cosf(cloudAngle)) + clouds[i][0] + (j * 30.0f),
                               ((cloudMinRad * radiusConst) * sinf(cloudAngle)) + clouds[i][1]);
                    if (cloudColorTmp[0] > cloudColor[0])
                        cloudColorTmp[0] -= 0.1f;
                    if (cloudColorTmp[1] > cloudColor[1])
                        cloudColorTmp[1] -= 0.1f;
                    if (cloudColorTmp[2] > cloudColor[2])
                        cloudColorTmp[2] -= 0.1f;
                }
                glEnd();
            }
        }
    }
}

// weather

void weather_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, weather_update, 0);
    if (weatherTImer >= 0.8f) {
        clearWeather = false;
        rainy = true;
    }
    if (weatherTImer >= 2) {
        rainbow = true;
        rainy = false;
    }
    if (weatherTImer >= 2.5) {
        clearWeather = true;
        rainbow = false;
        weatherTImer = 0;
    }
    if (rainy) {
        rain_update();
    }
}

void weather_display() {
    if (rainy)
        rain_display();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Vinland");
    glutInitWindowSize(WIDTH, HEIGHT);
    glutDisplayFunc(display);
    update();
    glutReshapeFunc(reshape);
    glutReshapeWindow(WIDTH, HEIGHT);
    glutMainLoop();
}

void update() {
    glutTimerFunc(FPS, sky_update, 0);
    glutTimerFunc(FPS, moon_update, 0);
    glutTimerFunc(FPS, sun_update, 0);
    glutTimerFunc(FPS, bridge_update, 0);
    glutTimerFunc(FPS, buildings_update, 0);
    glutTimerFunc(FPS, ocean_update, 0);
    glutTimerFunc(FPS, island_update, 0);
    glutTimerFunc(FPS, tree_color_update, 0);
    glutTimerFunc(FPS, tree_update, 0);
    glutTimerFunc(FPS, weather_update, 0);
    glutTimerFunc(FPS, cloud_update, 0);
}

void frame_display() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(- WIDTH, HEIGHT);
    glVertex2f(- WIDTH, - HEIGHT);
    glVertex2f(- WIDTH - 500.0f, - HEIGHT);
    glVertex2f(- WIDTH - 500.0f, HEIGHT);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(- WIDTH - 500.0f, - HEIGHT + 200.0f);
    glVertex2f(WIDTH + 500.0f, - HEIGHT + 200.0f);
    glVertex2f(WIDTH + 500.0f, - HEIGHT - 500.0f);
    glVertex2f(- WIDTH - 500.0f, - HEIGHT - 500.0f);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(WIDTH + 500.0f, HEIGHT);
    glVertex2f(WIDTH + 500.0f, - HEIGHT - 500.0f);
    glVertex2f(WIDTH, - HEIGHT - 500.0f);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    sky_display();
    cloud_display();
    if (night || midnight)
        moon_display();
    if (morning || noon || evening)
        sun_display();
    bridge_display();
    buildings_display();
    ocean_display();
    ocean_rocks_display();
    island_display();
    tree_display();
    weather_display();
    frame_display();
    glFlush();
}

void reshape(int Width, int Height) {
    glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-Width, Width, -Height, Height);
    glMatrixMode(GL_MODELVIEW);
}
