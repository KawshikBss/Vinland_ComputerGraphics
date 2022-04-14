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
int weatherTImer = 0;
bool clearWeather = true;
bool rainy = false;
bool snow = false;
bool rainedOnce = false;

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

void sky_display() {
    glBegin(GL_POLYGON);
    glColor3f(skyColor[0], skyColor[1], skyColor[2]);
    glVertex2f(skyXPos, skyYPos);
    glVertex2f(skyXPos + skyWidth, skyYPos);
    glColor3f(skyColorShade[0], skyColorShade[1], skyColorShade[2]);
    glVertex2f(skyXPos + skyWidth, skyYPos - skyHeight);
    glVertex2f(skyXPos, skyYPos - skyHeight);
    glEnd();
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
        }
    }
    else if (evening) {
        dayTimer++;
        if (dayTimer >= FPS * 5) {
            evening = false;
            noon = true;
            dayTimer = 0;
        }
    }
    else if (noon) {
        if (sunYPos > 0)
            sunYPos--;
        if (sunYPos == 0) {
            noon = false;
            night = true;
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
        }
    }
    else if (midnight) {
        nightTimer++;
        if (nightTimer >= FPS * 5) {
            midnight = false;
            morning = true;
            weatherTImer++;
            nightTimer = 0;
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
int carCount = 2;
float carWidth = 40.0f, carHeight = 20.0f;
GLfloat carsPos[2][2] = {{bridgeHorizonX - carWidth, bridgeHorizonY + bridgeHorizonHeight},
                                {0.0f + carWidth, bridgeHorizonY + bridgeHorizonHeight}};

void bridge_car_update() {
    if (carsPos[0][0] < carWidth)
        carsPos[0][0]++;
    if (carsPos[1][0] > bridgeHorizonX - carWidth)
        carsPos[1][0]--;
}
// TODO
void bridge_car_display() {
    for (int i = 0; i < carCount; i++) {
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_POLYGON);
        glVertex2f(carsPos[i][0], carsPos[i][1]);
        glVertex2f(carsPos[i][0] + carWidth, carsPos[i][1]);
        glVertex2f(carsPos[i][0] + carWidth / 2.0f, carsPos[i][1]);
        glVertex2f(carsPos[i][0] + carWidth / 2.0f, carsPos[i][1] + carHeight);
        glVertex2f(carsPos[i][0] + carWidth, carsPos[i][1] + carHeight);
        glVertex2f(carsPos[i][0], carsPos[i][1] + carHeight);
        glEnd();
    }
}

void bridge_update(int) {
    glutPostRedisplay();
    glutTimerFunc(FPS, bridge_update, 0);
    bridge_car_update();
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
    bridge_car_display();
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

}

void ocean_waves_display() {
    for (int j = 0; j < 4; j++) {
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
}

// island
GLfloat islandCenterXpos = -WIDTH, islandCenterYpos = -HEIGHT;
GLfloat islandRadius = WIDTH / 1.5f;
float islandSegments = 20.0f, islandAngle;
GLfloat islandGrassColor[3] = {0.18f, 0.54f, 0.34f};
GLfloat islandGrassColorShade[3] = {0.67f, 1.0f, 0.18f};

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
    if (weatherTImer >= 1) {
        clearWeather = false;
        rainy = true;
    }
    if (weatherTImer >= 2) {
        clearWeather = true;
        rainy = false;
        if (!rainedOnce)
            rainedOnce = true;
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
    glutTimerFunc(FPS, tree_color_update, 0);
    glutTimerFunc(FPS, tree_update, 0);
    glutTimerFunc(FPS, weather_update, 0);
    glutTimerFunc(FPS, cloud_update, 0);
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
    island_display();
    tree_display();
    weather_display();
    glFlush();
}

void reshape(int Width, int Height) {
    glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-Width, Width, -Height, Height);
    glMatrixMode(GL_MODELVIEW);
}
