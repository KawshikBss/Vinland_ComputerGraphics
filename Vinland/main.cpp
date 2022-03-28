#include <iostream>
#include <windows.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

using namespace std;

int WIDTH = 1280;
int HEIGHT = 960;
float FPS = 1000 / 60.f;

void update();
void display();
void reshape(int, int);

// weather
int dayTimer = 0;
int nightTimer = 0;
bool morning = true;
bool evening = false;
bool noon = false;
bool night = false;
bool midnight = true;

// sun
GLfloat sunXPos = - WIDTH / 2.0f, sunYPos = 0.0f;
float sunAngle, sunSegments = 100.0f, sunRadius = 100.0f;
float sunColor[3] = {1.0f, 0.64f, 0.0f};
float sunColorShade[3] = {1.0f, 1.0f, 0.0f};

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
    glBegin(GL_POLYGON);
    for (int i = 0; i <= sunSegments; i++) {
        if (i % 2 == 0)
            glColor3f(sunColor[0], sunColor[1], sunColor[2]);
        else
            glColor3f(sunColorShade[0], sunColorShade[1], sunColorShade[2]);
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
    glBegin(GL_POLYGON);
    for (int i = 0; i <= moonSegments; i++) {
        if (i % 2 == 0)
            glColor3f(moonColor[0], moonColor[1], moonColor[2]);
        else
            glColor3f(moonColorShade[0], moonColorShade[1], moonColorShade[2]);
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
    if (night || midnight)
        glutTimerFunc(FPS, moon_update, 0);
    if (morning || noon || evening)
        glutTimerFunc(FPS, sun_update, 0);
    glutTimerFunc(FPS, bridge_update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    if (night || midnight)
        moon_display();
    if (morning || noon || evening)
        sun_display();
    bridge_display();
    glFlush();
}

void reshape(int Width, int Height) {
    glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-Width, Width, -Height, Height);
    glMatrixMode(GL_MODELVIEW);
}
