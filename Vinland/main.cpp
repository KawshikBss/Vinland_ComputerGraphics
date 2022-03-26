#include <iostream>
#include <windows.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

using namespace std;

int WIDTH = 1280;
int HEIGHT = 960;

void update();
void display();
void reshape(int, int);

// bridge variables

void bridge_update() {
}

// bridge horizon
GLfloat bridgeHorizonX = - WIDTH, bridgeHorizonY = HEIGHT / 5.0f;
GLfloat bridgeHorizonWidth = WIDTH, bridgeHorizonHeight = 20.0f;
void bridge_horizon_display(){
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(bridgeHorizonX, bridgeHorizonY);
    glVertex2f(bridgeHorizonX, bridgeHorizonY + bridgeHorizonHeight);
    glColor3f(1.0f, 0.6f, 0.0f);
    glVertex2f(bridgeHorizonX + bridgeHorizonWidth, bridgeHorizonY + bridgeHorizonHeight);
    glVertex2f(bridgeHorizonX + bridgeHorizonWidth, bridgeHorizonY);
    glEnd();
}
// bridge legs
int bridgeLegCount = 4;
float bridgeLegGap = bridgeHorizonWidth / bridgeLegCount;
GLfloat bridgeLegXStart = - WIDTH, bridgeLegY = 0;
GLfloat bridgeLegWidth = 50.0f, bridgeLegHeight = HEIGHT / 2.0f;
void bridge_legs_display() {
    for (int i = 0; i < bridgeLegCount; i++) {
        glBegin(GL_POLYGON);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap)), bridgeLegY);
        glVertex2f((bridgeLegXStart + (i * bridgeLegGap) + (bridgeLegWidth / 2.5)), bridgeLegY + bridgeLegHeight);
        glColor3f(1.0f, 0.6f, 0.0f);
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
        glColor3f(1.0f, 0.6f, 0.0f);
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
            glColor3f(1.0f, 0.6f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f((radius * cosf(angle) + centerX), (radius * sinf(angle) + centerY));
            glVertex2f((radius * cosf(angle) + centerX), bridgeHorizonY);
            glEnd();
        }
    }
}


void bridge_display() {
    bridge_arches_display();
    bridge_horizon_display();
    bridge_legs_display();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("Vinland");
    glutInitWindowSize(WIDTH, HEIGHT);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutReshapeWindow(WIDTH, HEIGHT);
    glutMainLoop();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
