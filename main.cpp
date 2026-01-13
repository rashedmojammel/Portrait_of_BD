#include <windows.h>
#include <GL/glut.h>
#include <cmath>
#include<iostream>
#include <mmsystem.h>  // For PlaySound
#pragma comment(lib, "winmm.lib")  // Link the Windows multimedia library

using namespace std;


/* ================= GLOBAL VARIABLES================= */

float personX = 0;  // Position of walking person
float personX2 = 1600;       // People walking LEFT (right to left)
float legAnimation = 0;

float personX_road2 = 0;
float personX2_road2 = 1600;
//float metroTrainX = 0;  // Position of metro train

float car1X = 0;
float bus1X = 400;
float truck1X = 800;

// Road 1 - moving left (right to left)
float car2X = 1600;
float bus2X = 1200;
float truck2X = 600;

// Road 2 (second road at y=210) - moving right
float car3X = 200;
float truck3X = 900;

// Road 2 - moving left
float bus3X = 1400;
float car4X = 700;

// Road 3 (third road at y=110) - moving right
float car5X = 100;
float bus4X = 600;

// Road 3 - moving left
float truck4X = 1500;
float car6X = 900;


float metroTrainX =0; // Start off-screen left (optional, better than 0)
int trainState = 0;
float stopTimer = 0.0f;

bool isNight = false;              // false = day, true = night
float nightTimer = 0.0f;           // Counts time to toggle day/night
const float DAY_NIGHT_CYCLE = 20.0f; // Change every 20 seconds (real time)


const float STATION_CENTER = 710.0f;
const float STATION_ENTER = STATION_CENTER - 200;
const float STATION_STOP_POS = STATION_CENTER - 140;




// For moving clouds
float cloudOffset = 0.0f;

float vehicleScale = 1.4f;


// Rain control
bool isRaining = false;          // toggle with key 'r'
float rainSpeed = 18.0f;         // how fast rain falls
float rainAngle = -35.0f;        // slant (negative = left to right)
float rainDensity = 0.7f;        // 0.1 = light rain, 1.0 = heavy
float rainOffset = 0.0f;         // for animation

bool rainSoundPlaying = false;

// Add these global variables at the top of your file
GLuint buildingsDisplayList = 0;
GLuint buildingsNightDisplayList = 0;  // ← ADD THIS LINE
GLuint treesDisplayList = 0;
bool displayListsCreated = false;


// Airplane
float planeX = -300.0f;           // Start far left, off-screen
float planeY = 750.0f;            // High in the sky (adjust between 650–820)
float planeSpeed = 3.2f;          // Quite fast – feels like a real plane
bool planeVisible = true;         // You can toggle this if you want

//=======================================Abir=====================

float boatPosX  = -900.0f;
float cloudPosX = 0.0f;
bool  moveCloud = false;

/* ================= COMMON UTILS ================= */

void rdrawRectangle(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void rdrawWindow(float x, float y, float w, float h) {
    rdrawRectangle(x - 2, y - 2, w + 4, h + 4, 0.3f, 0.2f, 0.1f); // Frame (same)

    if (isNight) {
        // Lit windows at night (warm yellow/orange)
        rdrawRectangle(x, y, w, h, 1.0f, 0.85f, 0.5f);
    } else {
        // Normal daytime windows
        rdrawRectangle(x, y, w, h, 0.8f, 0.9f, 1.0f);
    }

    // Keep cross lines (optional - can remove at night)
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
    glVertex2f(x + w / 2, y);
    glVertex2f(x + w / 2, y + h);
    glVertex2f(x, y + h / 2);
    glVertex2f(x + w, y + h / 2);
    glEnd();
}

void rdrawDoor(float x, float y, float w, float h) {
    rdrawRectangle(x - 2, y - 2, w + 4, h + 4, 0.2f, 0.2f, 0.2f);
    rdrawRectangle(x, y, w, h, 0.6f, 0.1f, 0.1f);
}

void rdrawCircle(float cx, float cy, float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 45) {  // Changed from +=30 to +=15 for smoother circles
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}
void rkeyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'n':                // Press 'n' for night
        case 'N':
            isNight = true;
            glutPostRedisplay(); // Immediately redraw with night mode
            break;

        case 'd':                // Press 'd' for day
        case 'D':
            isNight = false;
            glutPostRedisplay(); // Immediately redraw with day mode
            break;
        case 'r': case 'R':
            isRaining = !isRaining;
            if (isRaining) cout << "Rain started!\n";
            else cout << "Rain stopped.\n";
            break;
        case 27: exit(0);// ESC key to exit (optional)
            exit(0);
            break;
    }
}
void rdrawStreetLight(float x, float y) {
    // Pole (same)
    rdrawRectangle(x, y, 6, 120, 0.2f, 0.2f, 0.2f);

    // Lamp head
    rdrawRectangle(x - 15, y + 120, 25, 8, 0.1f, 0.1f, 0.1f);

    if (isNight) {
        // Bright yellow light when night
        glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 1.0f, 0.0f, 0.2f);
        glVertex2f(x + 3, y + 120);
        glVertex2f(x - 40, y + 40);
        glVertex2f(x + 45, y + 40);
    glEnd();
    glDisable(GL_BLEND);
    } else {
        // Dim/off during day
        rdrawRectangle(x - 12, y + 120, 18, 3, 0.6f, 0.6f, 0.4f);
    }

    // COMMENT OUT OR SIMPLIFY THE GLOW EFFECT:
    /*

    */
}


void rdrawRain() {
    if (!isRaining) {
        // Stop rain sound when rain stops
        if (rainSoundPlaying) {
           // PlaySound(TEXT("metro2.wav"), NULL, SND_FILENAME | SND_ASYNC |SND_LOOP);  // Play metro sound once
            rainSoundPlaying = false;
        }
        return;
    }

    // Start rain sound when rain begins
    if (!rainSoundPlaying) {
       // PlaySound(TEXT("RainMetro.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        rainSoundPlaying = true;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // === 1. Natural rain opacity ===
    float baseAlpha = 0.12f + rainDensity * 0.28f;

    // === 2. High drop count for realistic density ===
    int rainCount = (int)(3500 * rainDensity);

    for (int i = 0; i < rainCount; i++) {
        // === 3. Consistent random distribution per drop ===
        unsigned int seed = i * 48271 + (int)(rainOffset * 50);
        float x = (float)((seed * 2654435761u) % 2000) - 100.0f;
        float startY = (float)((seed * 1103515245u) % 1400) - 200.0f;

        // === 4. Different fall speeds create depth illusion ===
        float depthSpeed = 0.7f + (float)((seed * 196314165u) % 100) / 125.0f;
        float animY = fmod(startY + rainOffset * rainSpeed * depthSpeed, 1500.0f) - 200.0f;

        // === 5. Varying drop lengths (realistic motion blur) ===
        float length = 35.0f + (float)((seed * 48271u) % 90);

        // === 6. STRAIGHT DOWN - purely vertical ===
        float x1 = x;
        float y1 = animY;
        float x2 = x;
        float y2 = y1 - length;

        // === 7. Depth-based rendering ===
        float depthFactor = 0.4f + (float)((seed * 1664525u) % 100) / 166.0f;
        float alpha = baseAlpha * depthFactor;
        float thickness = 0.6f + depthFactor * 1.6f;

        glLineWidth(thickness);

        // === 8. Realistic rain color ===
        glColor4f(0.82f + depthFactor * 0.16f,
                  0.88f + depthFactor * 0.10f,
                  0.96f + depthFactor * 0.04f,
                  alpha);

        glBegin(GL_LINES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        glEnd();
    }

    glDisable(GL_BLEND);
}
void rdrawPineTree(float x, float y) {
    // === Realistic trunk with depth ===
    // Main trunk - darker brown
    rdrawRectangle(x, y, 10, 40, 0.30f, 0.20f, 0.10f);

    // Right side highlight (3D effect)
    rdrawRectangle(x + 8, y, 2, 40, 0.40f, 0.28f, 0.14f);

    // Left side shadow
    rdrawRectangle(x, y, 2, 40, 0.22f, 0.14f, 0.08f);

    // === Natural pine foliage with depth ===
    // Bottom layer - darkest (back)
    glColor3f(0.06f, 0.30f, 0.10f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 65);
        glVertex2f(x - 24, y + 35);
        glVertex2f(x + 34, y + 35);
    glEnd();

    // Bottom layer highlight (front part)
    glColor3f(0.10f, 0.38f, 0.14f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 63);
        glVertex2f(x - 20, y + 38);
        glVertex2f(x + 30, y + 38);
    glEnd();

    // Middle layer - medium tone
    glColor3f(0.08f, 0.36f, 0.12f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 77);
        glVertex2f(x - 18, y + 50);
        glVertex2f(x + 28, y + 50);
    glEnd();

    glColor3f(0.12f, 0.44f, 0.16f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 75);
        glVertex2f(x - 15, y + 53);
        glVertex2f(x + 25, y + 53);
    glEnd();

    // Top layer - lightest
    glColor3f(0.10f, 0.42f, 0.14f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 88);
        glVertex2f(x - 13, y + 65);
        glVertex2f(x + 23, y + 65);
    glEnd();

    glColor3f(0.14f, 0.50f, 0.18f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 5, y + 86);
        glVertex2f(x - 10, y + 68);
        glVertex2f(x + 20, y + 68);
    glEnd();
}
void rdrawUmbrella(float x, float y, float r, float g, float b) {
    // Umbrella handle (stick)
    glColor3f(0.15f, 0.15f, 0.15f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 35);      // Start from head level
    glVertex2f(x, y + 52);      // Go up
    glEnd();
    glLineWidth(1.0f);

    // Curved handle grip
    glColor3f(0.4f, 0.3f, 0.2f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x, y + 35);
    glVertex2f(x - 3, y + 33);
    glVertex2f(x - 5, y + 32);
    glEnd();
    glLineWidth(1.0f);

    // Umbrella canopy (INVERTED dome shape - upside down U)
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + 52);  // Top center point (now bottom of inverted dome)
    for (int i = 0; i <= 180; i += 15) {
        float angle = i * 3.14159f / 180.0f;  // Changed: 0 to 180 (upward curve)
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus instead of plus
    }
    glEnd();

    // Umbrella segments (darker lines) - INVERTED
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(1.5f);
    for (int i = 0; i <= 180; i += 30) {
        float angle = i * 3.14159f / 180.0f;  // Changed
        glBegin(GL_LINES);
        glVertex2f(x, y + 52);
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus
        glEnd();
    }
    glLineWidth(1.0f);

    // Umbrella rim (bottom edge) - INVERTED
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;  // Changed
        glVertex2f(x + cos(angle) * 18, y + 52 - sin(angle) * 10);  // Changed: minus
    }
    glEnd();
    glLineWidth(1.0f);

    // Tip of umbrella (small sphere) - now at the connection point
    rdrawCircle(x, y + 52, 2, 0.2f, 0.2f, 0.2f);
}
void rdrawWalkingPerson(float x, float y, float r, float g, float b) {
    // Head
    rdrawCircle(x, y + 35, 8, 0.9f, 0.7f, 0.6f);

    // Body
    rdrawRectangle(x - 8, y + 15, 16, 20, r, g, b);

    // Animated legs
    float legOffset = sin(legAnimation) * 3;
    rdrawRectangle(x - 7, y, 7, 15 + legOffset, 0.2f, 0.2f, 0.2f);
    rdrawRectangle(x + 1, y, 7, 15 - legOffset, 0.2f, 0.2f, 0.2f);

    // Arms - modified when raining (holding umbrella)
    if (isRaining) {
        // Arm holding umbrella up
        rdrawRectangle(x - 10, y + 25, 4, 10, 0.9f, 0.7f, 0.6f);
        rdrawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    } else {
        // Normal arms
        rdrawRectangle(x - 10, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
        rdrawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    }

    // Draw umbrella when raining
    if (isRaining) {
        rdrawUmbrella(x, y, r, g, b);  // Use person's shirt color for umbrella
    }
}
void rdrawWalkingPersonLeft(float x, float y, float r, float g, float b) {
    // Head
    rdrawCircle(x, y + 35, 8, 0.9f, 0.7f, 0.6f);

    // Body
    rdrawRectangle(x - 8, y + 15, 16, 20, r, g, b);

    // Animated legs (opposite animation)
    float legOffset = sin(legAnimation) * 3;
    rdrawRectangle(x - 7, y, 7, 15 - legOffset, 0.2f, 0.2f, 0.2f);
    rdrawRectangle(x + 1, y, 7, 15 + legOffset, 0.2f, 0.2f, 0.2f);

    // Arms - modified when raining
    if (isRaining) {
        // Arm holding umbrella up
        rdrawRectangle(x - 10, y + 25, 4, 10, 0.9f, 0.7f, 0.6f);
        rdrawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    } else {
        // Normal arms
        rdrawRectangle(x - 10, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
        rdrawRectangle(x + 7, y + 20, 4, 12, 0.9f, 0.7f, 0.6f);
    }

    // Draw umbrella when raining
    if (isRaining) {
        rdrawUmbrella(x, y, r, g, b);  // Use person's shirt color
    }
}
void rdrawTrafficLight(float x, float y) {
    // Pole
    rdrawRectangle(x - 2, y, 4, 100, 0.25f, 0.25f, 0.25f);

    // Box
    rdrawRectangle(x - 8, y + 100, 16, 54, 0.15f, 0.15f, 0.15f);

    // Red
    rdrawCircle(x, y + 115, 6, 0.9f, 0.1f, 0.1f);
    // Yellow
    rdrawCircle(x, y + 135, 6, 0.9f, 0.7f, 0.1f);
    // Green
    rdrawCircle(x, y + 155, 6, 0.1f, 0.8f, 0.1f);
}

void rdrawRoadAndFootpath() {

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();


    /* ========== DYNAMIC SKY WITH RAIN EFFECT ========== */
    // Update sky brightness based on rain
    static float currentSkyBrightness = 1.0f;
    float targetSkyBrightness = isRaining ? 0.45f : 1.0f;  // Dark when raining

    // Smooth transition
    float transitionSpeed = 0.02f;
    if (currentSkyBrightness < targetSkyBrightness) {
        currentSkyBrightness += transitionSpeed;
        if (currentSkyBrightness > targetSkyBrightness)
            currentSkyBrightness = targetSkyBrightness;
    } else if (currentSkyBrightness > targetSkyBrightness) {
        currentSkyBrightness -= transitionSpeed;
        if (currentSkyBrightness < targetSkyBrightness)
            currentSkyBrightness = targetSkyBrightness;
    }

    // Dynamic sky - day or night
    if (isNight) {
        // Night sky: deep blue-purple (no clouds)
        glBegin(GL_QUADS);
        glColor3f(0.05f * currentSkyBrightness, 0.08f * currentSkyBrightness, 0.25f * currentSkyBrightness);
        glVertex2f(0, 460);
        glVertex2f(1600, 460);
        glColor3f(0.10f * currentSkyBrightness, 0.15f * currentSkyBrightness, 0.40f * currentSkyBrightness);
        glVertex2f(1600, 700);
        glVertex2f(0, 700);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.10f * currentSkyBrightness, 0.15f * currentSkyBrightness, 0.40f * currentSkyBrightness);
        glVertex2f(0, 700);
        glVertex2f(1600, 700);
        glColor3f(0.02f * currentSkyBrightness, 0.05f * currentSkyBrightness, 0.15f * currentSkyBrightness);
        glVertex2f(1600, 900);
        glVertex2f(0, 900);
        glEnd();

        // Moon
        float moonBrightness = isRaining ? 0.3f : 1.0f;  // Dimmer moon when raining
        rdrawCircle(1300, 800, 50, 0.95f * moonBrightness, 0.95f * moonBrightness, 0.90f * moonBrightness);
        rdrawCircle(1320, 810, 40, 0.98f * moonBrightness, 0.98f * moonBrightness, 0.92f * moonBrightness);

        // Moon glow (reduced when raining)
        if (!isRaining) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 0.95f, 0.8f, 0.15f);
            rdrawCircle(1300, 800, 80, 1.0f, 1.0f, 0.9f);
            glDisable(GL_BLEND);
        }
    } else {
        // Day sky (darker when raining)
        glBegin(GL_QUADS);
        glColor3f(0.4f * currentSkyBrightness, 0.6f * currentSkyBrightness, 0.85f * currentSkyBrightness);
        glVertex2f(0, 460);
        glVertex2f(1600, 460);
        glColor3f(0.6f * currentSkyBrightness, 0.75f * currentSkyBrightness, 0.92f * currentSkyBrightness);
        glVertex2f(1600, 700);
        glVertex2f(0, 700);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.6f * currentSkyBrightness, 0.75f * currentSkyBrightness, 0.92f * currentSkyBrightness);
        glVertex2f(0, 700);
        glVertex2f(1600, 700);
        glColor3f(0.85f * currentSkyBrightness, 0.92f * currentSkyBrightness, 0.98f * currentSkyBrightness);
        glVertex2f(1600, 900);
        glVertex2f(0, 900);
        glEnd();

        // Clouds - darker and grayer when raining
        float cloudY = 750;
        float cloudBrightness = isRaining ? 0.35f : 1.0f;  // Much darker when raining
        float cloudR = isRaining ? 0.40f : 0.95f;  // Gray clouds when raining
        float cloudG = isRaining ? 0.42f : 0.95f;
        float cloudB = isRaining ? 0.45f : 0.98f;

        for (int i = 0; i < 6; i++) {
            float cx = (100 + i * 300 + cloudOffset);
            if (cx > 1800) cx -= 2000;

            // Main cloud shapes - darker when raining
            rdrawCircle(cx, cloudY, 30, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            rdrawCircle(cx + 25, cloudY + 5, 35, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            rdrawCircle(cx + 55, cloudY, 28, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);
            rdrawCircle(cx - 20, cloudY - 5, 25, cloudR * cloudBrightness, cloudG * cloudBrightness, cloudB * cloudBrightness);

            // Second layer clouds
            rdrawCircle(cx + 80, cloudY + 20, 20, cloudR * cloudBrightness * 0.95f, cloudG * cloudBrightness * 0.95f, cloudB * cloudBrightness * 0.95f);
            rdrawCircle(cx + 110, cloudY + 15, 25, cloudR * cloudBrightness * 0.95f, cloudG * cloudBrightness * 0.95f, cloudB * cloudBrightness * 0.95f);
        }

        // Add dark storm clouds when raining
        if (isRaining) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Additional dark cloud layer
            for (int i = 0; i < 8; i++) {
                float cx = (50 + i * 220 + cloudOffset * 0.7f);
                if (cx > 1800) cx -= 2000;
                float cy = 720 + (i % 3) * 20;

                // Dark gray storm clouds
                glColor4f(0.25f, 0.27f, 0.30f, 0.7f);
                rdrawCircle(cx, cy, 35, 0.25f, 0.27f, 0.30f);
                rdrawCircle(cx + 30, cy + 5, 40, 0.22f, 0.24f, 0.27f);
                rdrawCircle(cx + 60, cy, 32, 0.28f, 0.30f, 0.33f);
            }
            glDisable(GL_BLEND);
        }
    }

    /* ========== LAYERED GRASS WITH DEPTH (darker when raining) ========== */
    float grassBrightness = isRaining ? 0.6f : 1.0f;

    // Far background grass (darker)
    glBegin(GL_QUADS);
    glColor3f(0.08f * grassBrightness, 0.45f * grassBrightness, 0.08f * grassBrightness);
    glVertex2f(0, 200);
    glVertex2f(1600, 200);
    glColor3f(0.10f * grassBrightness, 0.50f * grassBrightness, 0.10f * grassBrightness);
    glVertex2f(1600, 280);
    glVertex2f(0, 280);
    glEnd();

    // Middle grass
    glBegin(GL_QUADS);
    glColor3f(0.10f * grassBrightness, 0.50f * grassBrightness, 0.10f * grassBrightness);
    glVertex2f(0, 100);
    glVertex2f(1600, 100);
    glColor3f(0.13f * grassBrightness, 0.55f * grassBrightness, 0.13f * grassBrightness);
    glVertex2f(1600, 200);
    glVertex2f(0, 200);
    glEnd();

    // Foreground grass (brightest)
    glBegin(GL_QUADS);
    glColor3f(0.13f * grassBrightness, 0.55f * grassBrightness, 0.13f * grassBrightness);
    glVertex2f(0, 0);
    glVertex2f(1600, 0);
    glColor3f(0.16f * grassBrightness, 0.60f * grassBrightness, 0.16f * grassBrightness);
    glVertex2f(1600, 100);
    glVertex2f(0, 100);
    glEnd();

    // Decorative grass clumps
    glColor3f(0.18f * grassBrightness, 0.65f * grassBrightness, 0.18f * grassBrightness);
    for (int x = 30; x < 1600; x += 120) {
        for (int i = 0; i < 3; i++) {
            rdrawCircle(x + i * 15, 50 + (i * 10), 8, 0.18f * grassBrightness, 0.65f * grassBrightness, 0.18f * grassBrightness);
        }
    }

    /* ========== PREMIUM ASPHALT ROAD WITH 3D EFFECT ========== */
    float roadBrightness = isRaining ? 0.7f : 1.0f;

    // Road shadow/depth (darker bottom edge)
    rdrawRectangle(0, 278, 1600, 3, 0.08f * roadBrightness, 0.08f * roadBrightness, 0.08f * roadBrightness);

    // Main road with gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f * roadBrightness, 0.18f * roadBrightness, 0.20f * roadBrightness);
    glVertex2f(0, 280);
    glVertex2f(1600, 280);
    glColor3f(0.22f * roadBrightness, 0.22f * roadBrightness, 0.24f * roadBrightness);
    glVertex2f(1600, 340);
    glVertex2f(0, 340);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f * roadBrightness, 0.22f * roadBrightness, 0.24f * roadBrightness);
    glVertex2f(0, 340);
    glVertex2f(1600, 340);
    glColor3f(0.18f * roadBrightness, 0.18f * roadBrightness, 0.20f * roadBrightness);
    glVertex2f(1600, 400);
    glVertex2f(0, 400);
    glEnd();

    // Road shine/reflection effect (more visible when raining)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float shineAlpha = isRaining ? 0.12f : 0.05f;  // More reflective when wet
    glColor4f(1.0f, 1.0f, 1.0f, shineAlpha);
    for (int x = 0; x < 1600; x += 200) {
        glBegin(GL_QUADS);
        glVertex2f(x, 285);
        glVertex2f(x + 80, 285);
        glVertex2f(x + 100, 320);
        glVertex2f(x + 20, 320);
        glEnd();
    }
    glDisable(GL_BLEND);

    /* ========== FUTURISTIC ROAD MARKINGS ========== */
    // Glowing white edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glBegin(GL_LINES);
    glVertex2f(0, 283);
    glVertex2f(1600, 283);
    glVertex2f(0, 397);
    glVertex2f(1600, 397);
    glEnd();

    // Glow effect on edge lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(8.0f);
    glColor4f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, 283);
    glVertex2f(1600, 283);
    glVertex2f(0, 397);
    glVertex2f(1600, 397);
    glEnd();
    glDisable(GL_BLEND);

    // LED-style center divider
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, 340);
        glVertex2f(x + 30, 340);
        glEnd();

        glVertex2f(x + 5, 340);
        glVertex2f(x + 30, 340);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
        glLineWidth(4.0f);
    }

    // Pedestrian crossing sign painted on road
    glColor3f(1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    rdrawCircle(640, 340, 20, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    glColor3f(0.2f * roadBrightness, 0.5f * roadBrightness, 0.9f * roadBrightness);
    rdrawCircle(640, 340, 18, 0.2f * roadBrightness, 0.5f * roadBrightness, 0.9f * roadBrightness);
    // Stick figure
    rdrawCircle(640, 348, 4, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);
    rdrawRectangle(638, 338, 4, 10, 1.0f * roadBrightness, 1.0f * roadBrightness, 1.0f * roadBrightness);

    glLineWidth(1.0f);

    /* ========== DESIGNER SIDEWALK/FOOTPATH ========== */
    float sidewalkBrightness = isRaining ? 0.65f : 1.0f;

    // Modern concrete with granite texture
    glBegin(GL_QUADS);
    glColor3f(0.70f * sidewalkBrightness, 0.72f * sidewalkBrightness, 0.75f * sidewalkBrightness);
    glVertex2f(0, 400);
    glVertex2f(1600, 400);
    glColor3f(0.78f * sidewalkBrightness, 0.80f * sidewalkBrightness, 0.83f * sidewalkBrightness);
    glVertex2f(1600, 430);
    glVertex2f(0, 430);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.78f * sidewalkBrightness, 0.80f * sidewalkBrightness, 0.83f * sidewalkBrightness);
    glVertex2f(0, 430);
    glVertex2f(1600, 430);
    glColor3f(0.82f * sidewalkBrightness, 0.84f * sidewalkBrightness, 0.87f * sidewalkBrightness);
    glVertex2f(1600, 460);
    glVertex2f(0, 460);
    glEnd();

    // Decorative metal edge strip
    rdrawRectangle(0, 399, 1600, 2, 0.6f * sidewalkBrightness, 0.62f * sidewalkBrightness, 0.65f * sidewalkBrightness);
    rdrawRectangle(0, 459, 1600, 2, 0.6f * sidewalkBrightness, 0.62f * sidewalkBrightness, 0.65f * sidewalkBrightness);

    // Premium interlocking paver pattern
    glColor3f(0.65f * sidewalkBrightness, 0.67f * sidewalkBrightness, 0.70f * sidewalkBrightness);
    glLineWidth(1.5f);

    // Diagonal pattern
    for (int x = -100; x < 1700; x += 60) {
        glBegin(GL_LINES);
        glVertex2f(x, 400);
        glVertex2f(x + 60, 460);
        glEnd();
    }

    for (int x = -100; x < 1700; x += 60) {
        glBegin(GL_LINES);
        glVertex2f(x + 30, 400);
        glVertex2f(x - 30, 460);
        glEnd();
    }

    // Accent colored stripe pattern
    glColor3f(0.75f * sidewalkBrightness, 0.25f * sidewalkBrightness, 0.15f * sidewalkBrightness);
    for (int y = 410; y < 460; y += 25) {
        rdrawRectangle(0, y, 1600, 3, 0.75f * sidewalkBrightness, 0.25f * sidewalkBrightness, 0.15f * sidewalkBrightness);
    }

    /* ========== SMART CITY ELEMENTS ========== */
    // Modern bus stop shelter
    int shelterX = 400;
    float furnitureBrightness = isRaining ? 0.7f : 1.0f;

    // Shelter poles
    rdrawRectangle(shelterX, 400, 6, 60, 0.4f * furnitureBrightness, 0.4f * furnitureBrightness, 0.45f * furnitureBrightness);
    rdrawRectangle(shelterX + 150, 400, 6, 60, 0.4f * furnitureBrightness, 0.4f * furnitureBrightness, 0.45f * furnitureBrightness);

    // Glass roof
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * furnitureBrightness, 0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(shelterX - 10, 460);
    glVertex2f(shelterX + 166, 460);
    glVertex2f(shelterX + 156, 480);
    glVertex2f(shelterX, 480);
    glEnd();
    glDisable(GL_BLEND);

    // Digital display board
    rdrawRectangle(shelterX + 40, 445, 70, 12, 0.1f * furnitureBrightness, 0.1f * furnitureBrightness, 0.15f * furnitureBrightness);
    rdrawRectangle(shelterX + 42, 447, 66, 8, 0.0f, 0.3f * furnitureBrightness, 0.0f);
    // LED text effect
    glColor3f(0.0f, 1.0f * furnitureBrightness, 0.2f * furnitureBrightness);
    for (int i = 0; i < 20; i++) {
        rdrawCircle(shelterX + 45 + i * 3, 451, 1, 0.0f, 1.0f * furnitureBrightness, 0.2f * furnitureBrightness);
    }

    // Seating bench
    rdrawRectangle(shelterX + 20, 410, 100, 6, 0.6f * furnitureBrightness, 0.35f * furnitureBrightness, 0.15f * furnitureBrightness);
    rdrawRectangle(shelterX + 20, 416, 100, 10, 0.6f * furnitureBrightness, 0.35f * furnitureBrightness, 0.15f * furnitureBrightness);
    rdrawRectangle(shelterX + 25, 405, 4, 11, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness);
    rdrawRectangle(shelterX + 111, 405, 4, 11, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness);

    /* ========== DECORATIVE URBAN LANDSCAPING ========== */
    // Modern concrete planters with tropical plants
    for (int x = 200; x < 1400; x += 300) {
        // Planter
        rdrawRectangle(x, 407, 50, 20, 0.55f * furnitureBrightness, 0.55f * furnitureBrightness, 0.58f * furnitureBrightness);
        rdrawRectangle(x + 2, 409, 46, 16, 0.45f * furnitureBrightness, 0.45f * furnitureBrightness, 0.48f * furnitureBrightness);

        // Palm-like plants
        glColor3f(0.15f * grassBrightness, 0.55f * grassBrightness, 0.2f * grassBrightness);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 10, 445);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 20, 445);
        glVertex2f(x + 15, 425);
        glVertex2f(x + 15, 448);
        glEnd();

        glColor3f(0.2f * grassBrightness, 0.65f * grassBrightness, 0.25f * grassBrightness);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 30, 442);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 40, 442);
        glVertex2f(x + 35, 425);
        glVertex2f(x + 35, 445);
        glEnd();

        // Colorful flowers
        rdrawCircle(x + 25, 422, 4, 1.0f * furnitureBrightness, 0.3f * furnitureBrightness, 0.5f * furnitureBrightness);
        rdrawCircle(x + 28, 420, 3, 0.9f * furnitureBrightness, 0.8f * furnitureBrightness, 0.2f * furnitureBrightness);
    }

    // Waste bins
    for (int x = 350; x < 1500; x += 400) {
        // Bin body
        rdrawRectangle(x, 410, 25, 30, 0.2f * furnitureBrightness, 0.6f * furnitureBrightness, 0.3f * furnitureBrightness);
        rdrawRectangle(x + 2, 412, 21, 26, 0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);

        // Recycling symbol
        glColor3f(1.0f * furnitureBrightness, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness);
        rdrawCircle(x + 12, 425, 6, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness, 1.0f * furnitureBrightness);
        glColor3f(0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);
        rdrawCircle(x + 12, 425, 4, 0.15f * furnitureBrightness, 0.5f * furnitureBrightness, 0.25f * furnitureBrightness);
    }

    /* ========== STREET FURNITURE DETAILS ========== */
    // Information kiosk/map board
    int kioskX = 900;
    rdrawRectangle(kioskX, 405, 4, 50, 0.35f * furnitureBrightness, 0.35f * furnitureBrightness, 0.4f * furnitureBrightness);
    rdrawRectangle(kioskX - 20, 455, 45, 35, 0.25f * furnitureBrightness, 0.5f * furnitureBrightness, 0.75f * furnitureBrightness);

    // Glass/display surface
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 1.0f * furnitureBrightness, 0.5f);
    rdrawRectangle(kioskX - 18, 457, 41, 31, 0.7f * furnitureBrightness, 0.9f * furnitureBrightness, 1.0f * furnitureBrightness);
    glDisable(GL_BLEND);

    // Map lines
    glColor3f(0.9f * furnitureBrightness, 0.3f * furnitureBrightness, 0.2f * furnitureBrightness);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(kioskX - 15, 465);
    glVertex2f(kioskX + 15, 465);
    glVertex2f(kioskX, 465);
    glVertex2f(kioskX, 480);
    glEnd();

    glLineWidth(1.0f);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Traffic lights



}

//======================Second road================//


void rdrawSecondRoad() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int roadY = 150;  // Position in green area

    // Road shadow
    rdrawRectangle(0, roadY - 2, 1600, 3, 0.08f, 0.08f, 0.08f);

    // Main road gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(0, roadY);
    glVertex2f(1600, roadY);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(1600, roadY + 60);
    glVertex2f(0, roadY + 60);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(0, roadY + 60);
    glVertex2f(1600, roadY + 60);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(1600, roadY + 120);
    glVertex2f(0, roadY + 120);
    glEnd();

    // White edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();

    // Yellow center divider
    glColor3f(1.0f, 1.0f, 1.0f);  // White
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();
    }
    rdrawTrafficLight(450, 400);

    // Traffic arrows
   /* glColor3f(0.95f, 0.95f, 1.0f);
    for (int x = 200; x < 1500; x += 400) {
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 85);
        glVertex2f(x + 40, roadY + 95);
        glVertex2f(x + 20, roadY + 105);
        glEnd();
        rdrawRectangle(x, roadY + 90, 20, 10, 0.95f, 0.95f, 1.0f);

        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 15);
        glVertex2f(x + 40, roadY + 25);
        glVertex2f(x + 20, roadY + 35);
        glEnd();
        rdrawRectangle(x, roadY + 20, 20, 10, 0.95f, 0.95f, 1.0f);
    } */

    glLineWidth(1.0f);

    // BOTTOM FOOTPATH (below the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY - 30);
    glVertex2f(1600, roadY - 30);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY);
    glVertex2f(0, roadY);
    glEnd();

    // Decorative edge strips
    rdrawRectangle(0, roadY - 31, 1600, 2, 0.6f, 0.62f, 0.65f);
    rdrawRectangle(0, roadY - 1, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern on bottom footpath
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY - 30);
        glVertex2f(x + 50, roadY);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY - 25; y < roadY; y += 12) {
        rdrawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    // TOP FOOTPATH (above the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY + 120);
    glVertex2f(1600, roadY + 120);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY + 150);
    glVertex2f(0, roadY + 150);
    glEnd();

    // Decorative edge strips
    rdrawRectangle(0, roadY + 119, 1600, 2, 0.6f, 0.62f, 0.65f);
    rdrawRectangle(0, roadY + 149, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern on top footpath
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY + 120);
        glVertex2f(x + 50, roadY + 150);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY + 125; y < roadY + 150; y += 12) {
        rdrawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    glLineWidth(1.0f);

    // Street lights on TOP footpath
    for (int x = 150; x < 1600; x += 250) {
        rdrawRectangle(x, roadY + 150, 5, 60, 0.2f, 0.2f, 0.2f);
        rdrawRectangle(x - 12, roadY + 210, 20, 6, 0.1f, 0.1f, 0.1f);
        rdrawRectangle(x - 10, roadY + 210, 16, 2, 1.0f, 1.0f, 0.8f);
    }

    // Small trees on TOP footpath


    // Bollards on BOTTOM footpath
    glColor3f(0.3f, 0.3f, 0.35f);
    for (int x = 120; x < 1600; x += 150) {
        rdrawRectangle(x, roadY - 26, 10, 20, 0.3f, 0.3f, 0.35f);

        // LED light
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rdrawCircle(x + 5, roadY - 10, 6, 0.2f, 0.8f, 1.0f);
        glDisable(GL_BLEND);
        glColor3f(0.3f, 0.3f, 0.35f);
    }

    // Small plants on BOTTOM footpath
    for (int x = 80; x < 1600; x += 180) {
        rdrawCircle(x, roadY - 18, 8, 0.2f, 0.6f, 0.25f);
        rdrawCircle(x, roadY - 12, 6, 0.25f, 0.65f, 0.3f);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void rdrawThirdRoad() {

    rdrawTrafficLight(1150, 400);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int roadY = 50;  // Position in lower green area

    // BOTTOM FOOTPATH (below the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY - 30);
    glVertex2f(1600, roadY - 30);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY);
    glVertex2f(0, roadY);
    glEnd();

    // Decorative edge strips
    rdrawRectangle(0, roadY - 31, 1600, 2, 0.6f, 0.62f, 0.65f);
    rdrawRectangle(0, roadY - 1, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY - 30);
        glVertex2f(x + 50, roadY);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY - 25; y < roadY; y += 12) {
        rdrawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    /* ========== ROAD ========== */
    // Road shadow
    rdrawRectangle(0, roadY - 2, 1600, 3, 0.08f, 0.08f, 0.08f);

    // Main road with gradient
    glBegin(GL_QUADS);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(0, roadY);
    glVertex2f(1600, roadY);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(1600, roadY + 60);
    glVertex2f(0, roadY + 60);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.22f, 0.22f, 0.24f);
    glVertex2f(0, roadY + 60);
    glVertex2f(1600, roadY + 60);
    glColor3f(0.18f, 0.18f, 0.20f);
    glVertex2f(1600, roadY + 120);
    glVertex2f(0, roadY + 120);
    glEnd();

    // White edge lines
    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();

    // Glow effect on edge lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(8.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(0, roadY + 3);
    glVertex2f(1600, roadY + 3);
    glVertex2f(0, roadY + 117);
    glVertex2f(1600, roadY + 117);
    glEnd();
    glDisable(GL_BLEND);

    // Yellow center divider
    glColor3f(1.0f, 1.0f, 1.0f);  // White
    glLineWidth(4.0f);
    for (int x = 0; x < 1600; x += 40) {
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();

        // Glow effect
        /* glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.85f, 0.0f, 0.4f);
        glLineWidth(8.0f); */
        glBegin(GL_LINES);
        glVertex2f(x + 5, roadY + 60);
        glVertex2f(x + 30, roadY + 60);
        glEnd();
        glDisable(GL_BLEND);
       glColor3f(1.0f, 1.0f, 1.0f);  // White
        glLineWidth(4.0f);
    }

    // Traffic arrows
    /*glColor3f(0.95f, 0.95f, 1.0f);
    for (int x = 200; x < 1500; x += 400) {
        // Upper lane arrow (pointing right)
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 85);
        glVertex2f(x + 40, roadY + 95);
        glVertex2f(x + 20, roadY + 105);
        glEnd();
        rdrawRectangle(x, roadY + 90, 20, 10, 0.95f, 0.95f, 1.0f);

        // Lower lane arrow (pointing right)
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 20, roadY + 15);
        glVertex2f(x + 40, roadY + 25);
        glVertex2f(x + 20, roadY + 35);
        glEnd();
        rdrawRectangle(x, roadY + 20, 20, 10, 0.95f, 0.95f, 1.0f);
    } */

    glLineWidth(1.0f);

    // TOP FOOTPATH (above the road)
    glBegin(GL_QUADS);
    glColor3f(0.70f, 0.72f, 0.75f);
    glVertex2f(0, roadY + 120);
    glVertex2f(1600, roadY + 120);
    glColor3f(0.78f, 0.80f, 0.83f);
    glVertex2f(1600, roadY + 150);
    glVertex2f(0, roadY + 150);
    glEnd();

    // Decorative edge strips
    rdrawRectangle(0, roadY + 119, 1600, 2, 0.6f, 0.62f, 0.65f);
    rdrawRectangle(0, roadY + 149, 1600, 2, 0.6f, 0.62f, 0.65f);

    // Paver pattern
    glColor3f(0.65f, 0.67f, 0.70f);
    glLineWidth(1.5f);
    for (int x = -50; x < 1650; x += 50) {
        glBegin(GL_LINES);
        glVertex2f(x, roadY + 120);
        glVertex2f(x + 50, roadY + 150);
        glEnd();
    }

    // Accent stripes
    glColor3f(0.75f, 0.25f, 0.15f);
    for (int y = roadY + 125; y < roadY + 150; y += 12) {
        rdrawRectangle(0, y, 1600, 2, 0.75f, 0.25f, 0.15f);
    }

    glLineWidth(1.0f);

    // Street lights on TOP footpath
    for (int x = 150; x < 1600; x += 250) {
        rdrawRectangle(x, roadY + 150, 5, 60, 0.2f, 0.2f, 0.2f);
        rdrawRectangle(x - 12, roadY + 210, 20, 6, 0.1f, 0.1f, 0.1f);
        rdrawRectangle(x - 10, roadY + 210, 16, 2, 1.0f, 1.0f, 0.8f);
    }

    // Small trees on TOP footpath
    for (int x = 100; x < 1600; x += 200) {
        rdrawRectangle(x, roadY + 155, 8, 25, 0.4f, 0.25f, 0.1f);
        rdrawCircle(x + 4, roadY + 185, 12, 0.15f, 0.6f, 0.15f);
        rdrawCircle(x + 4, roadY + 193, 10, 0.18f, 0.65f, 0.18f);
    }

    // Bollards on BOTTOM footpath
    glColor3f(0.3f, 0.3f, 0.35f);
    for (int x = 120; x < 1600; x += 150) {
        rdrawRectangle(x, roadY - 26, 10, 20, 0.3f, 0.3f, 0.35f);

        // LED light
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        rdrawCircle(x + 5, roadY - 10, 6, 0.2f, 0.8f, 1.0f);
        glDisable(GL_BLEND);
        glColor3f(0.3f, 0.3f, 0.35f);
    }

    // Small plants on BOTTOM footpath
    for (int x = 80; x < 1600; x += 180) {
        rdrawCircle(x, roadY - 18, 8, 0.2f, 0.6f, 0.25f);
        rdrawCircle(x, roadY - 12, 6, 0.25f, 0.65f, 0.3f);
    }

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void  rthired() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int metroY = 460;  // ABOVE the buildings level

    // DHAKA METRO PILLARS (gray concrete) - TALL pillars from ground
    for (int x = 100; x < 1600; x += 200) {
        // Pillar shadow at ground
        rdrawRectangle(x - 25, 120, 50, 8, 0.35f, 0.35f, 0.38f);

        // Main tall pillar (light gray concrete)
        glBegin(GL_QUADS);
        glColor3f(0.60f, 0.60f, 0.63f);
        glVertex2f(x - 20, 128);
        glVertex2f(x + 20, 128);
        glColor3f(0.68f, 0.68f, 0.71f);
        glVertex2f(x + 18, metroY);
        glVertex2f(x - 18, metroY);
        glEnd();

        // Pillar segments (visible construction joints)
        glColor3f(0.50f, 0.50f, 0.53f);
        for (int seg = 150; seg < metroY; seg += 40) {
            rdrawRectangle(x - 20, seg, 40, 2, 0.50f, 0.50f, 0.53f);
        }

        // Pillar capital/support beam
        rdrawRectangle(x - 28, metroY, 56, 12, 0.55f, 0.55f, 0.58f);

        // Red and Green bands (Bangladesh flag colors)
        rdrawRectangle(x - 20, 200, 40, 5, 0.85f, 0.1f, 0.1f);  // Red
        rdrawRectangle(x - 20, 350, 40, 5, 0.0f, 0.55f, 0.25f);  // Green
    }

    // ELEVATED TRACK BOX STRUCTURE (orange/brown like reference)
    // Track shadow
    rdrawRectangle(0, metroY + 11, 1600, 4, 0.30f, 0.30f, 0.33f);

    // Main concrete guideway (orange-brown/terracotta)
    rdrawRectangle(0, metroY + 15, 1600, 45, 0.82f, 0.42f, 0.28f);

    // Top edge (darker)
    rdrawRectangle(0, metroY + 60, 1600, 3, 0.62f, 0.32f, 0.20f);

    // Side structural lines
    glColor3f(0.70f, 0.38f, 0.25f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 22);
    glVertex2f(1600, metroY + 22);
    glVertex2f(0, metroY + 53);
    glVertex2f(1600, metroY + 53);
    glEnd();
    glLineWidth(1.0f);

    // RAIL TRACKS
    glColor3f(0.35f, 0.35f, 0.38f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 30);
    glVertex2f(1600, metroY + 30);
    glVertex2f(0, metroY + 45);
    glVertex2f(1600, metroY + 45);
    glEnd();
    glLineWidth(1.0f);

    // OVERHEAD CATENARY SYSTEM
    for (int x = 100; x < 1600; x += 200) {
        // Support poles on track
        rdrawRectangle(x - 3, metroY + 63, 6, 30, 0.35f, 0.35f, 0.40f);
        // Cantilever arm
        rdrawRectangle(x - 28, metroY + 91, 56, 4, 0.40f, 0.40f, 0.45f);
    }

    // Overhead wire
    glLineWidth(2.0f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 88);
    glVertex2f(1600, metroY + 88);
    glEnd();
    glLineWidth(1.0f);

    // METRO TRAIN (WHITE like Dhaka Metro MRT-6)
    float trainX = metroTrainX;
    float trainY = metroY + 63;
    float trainLength = 320;
    float trainHeight = 55;

    // Train shadow on track
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.15f);
    rdrawRectangle(trainX + 5, trainY - 8, trainLength, 8, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Main train body (white)
    rdrawRectangle(trainX, trainY, trainLength, trainHeight, 0.95f, 0.95f, 0.98f);

    // Rounded front nose (aerodynamic)
    glColor3f(0.88f, 0.88f, 0.92f);
    glBegin(GL_POLYGON);
    glVertex2f(trainX + trainLength, trainY + 5);
    glVertex2f(trainX + trainLength + 30, trainY + 18);
    glVertex2f(trainX + trainLength + 35, trainY + 28);
    glVertex2f(trainX + trainLength + 30, trainY + 38);
    glVertex2f(trainX + trainLength, trainY + 50);
    glEnd();

    // Train roof (slightly darker)
    rdrawRectangle(trainX, trainY + trainHeight, trainLength + 20, 8, 0.85f, 0.85f, 0.88f);

    // Green stripe - TOP (Dhaka Metro Line 6 signature color)
    rdrawRectangle(trainX, trainY + trainHeight - 6, trainLength, 6, 0.0f, 0.65f, 0.32f);

    // Green stripe - BOTTOM
    rdrawRectangle(trainX, trainY + 2, trainLength, 8, 0.0f, 0.65f, 0.32f);

    // Red accent stripe (Bangladesh flag color)
    rdrawRectangle(trainX, trainY + 12, trainLength, 3, 0.85f, 0.15f, 0.15f);

    // Windows (dark tinted glass) - Multiple coaches
    for (int i = 0; i < 9; i++) {
        float wx = trainX + 20 + (i * 34);

        // Window glass (dark blue tint)
        rdrawRectangle(wx, trainY + 20, 28, 25, 0.12f, 0.18f, 0.30f);

        // Window frame (light gray)
        glColor3f(0.75f, 0.75f, 0.78f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(wx, trainY + 20);
        glVertex2f(wx + 28, trainY + 20);
        glVertex2f(wx + 28, trainY + 45);
        glVertex2f(wx, trainY + 45);
        glEnd();
    }

    // Front windshield (driver cabin)
    rdrawRectangle(trainX + trainLength - 15, trainY + 24, 13, 22, 0.15f, 0.22f, 0.35f);

    // Headlight (bright white)
    rdrawCircle(trainX + trainLength + 20, trainY + 20, 6, 1.0f, 1.0f, 0.88f);

    // Red taillight on back
    rdrawCircle(trainX + 10, trainY + 20, 5, 0.95f, 0.15f, 0.15f);

    // Door markings (red emergency indicators)
    rdrawRectangle(trainX + 100, trainY + 2, 3, 50, 0.90f, 0.12f, 0.12f);
    rdrawRectangle(trainX + 210, trainY + 2, 3, 50, 0.90f, 0.12f, 0.12f);

    // Undercarriage (dark gray)
    rdrawRectangle(trainX + 15, trainY - 6, trainLength - 30, 6, 0.28f, 0.28f, 0.32f);

    // Wheels/Bogies (3 sets per train)
    for (int i = 0; i < 4; i++) {
        float wx = trainX + 50 + (i * 80);
        // Wheel outer
        rdrawCircle(wx, trainY - 10, 9, 0.18f, 0.18f, 0.22f);
        // Wheel inner (hub)
        rdrawCircle(wx, trainY - 10, 5, 0.38f, 0.38f, 0.42f);
        // Second wheel
        rdrawCircle(wx + 35, trainY - 10, 9, 0.18f, 0.18f, 0.22f);
        rdrawCircle(wx + 35, trainY - 10, 5, 0.38f, 0.38f, 0.42f);
    }

    // Pantograph (current collector on roof)
    float pantoX = trainX + trainLength / 2;
    rdrawRectangle(pantoX - 4, trainY + 63, 8, 18, 0.32f, 0.32f, 0.37f);

    // Pantograph arms
    glColor3f(0.35f, 0.35f, 0.40f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(pantoX, trainY + 81);
    glVertex2f(pantoX - 18, metroY + 88);
    glVertex2f(pantoX, trainY + 81);
    glVertex2f(pantoX + 18, metroY + 88);
    glEnd();

    // Pantograph contact shoe
    rdrawRectangle(pantoX - 20, metroY + 85, 40, 4, 0.30f, 0.30f, 0.35f);

    glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}


//=================Building 1========//


void rdrawBuilding1() {
    //glClear(GL_COLOR_BUFFER_BIT);

    // ===== LEFT BUILDING =====
    rdrawRectangle(55, 100, 85, 280, 1.0f, 0.7f, 0.4f); // left wall (peach)

    // Windows (3 rows x 2 cols)
    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 2; ++col)
            rdrawWindow(65 + col * 40, 170 + row * 70, 25, 43);

    // Door with frame
    rdrawDoor(83, 100, 30, 45);



    // ===== MIDDLE BUILDING =====
    rdrawRectangle(140, 60, 90, 220, 0.8f, 0.3f, 0.2f); // middle wall (red-orange)

    for (int row = 0; row < 3; ++row)
        for (int col = 0; col < 2; ++col)
            rdrawWindow(150 + col * 40, 100 + row * 55, 30, 42);

     //gound Chimney
    rdrawRectangle(51, 70, 92, 30, 0.4f, 0.4f, 0.4f);

    //gound
    rdrawRectangle(51, 60, 92, 10, 0.8f, 0.8f, 0.8f);

    // Stairs (2 steps)
    rdrawRectangle(78, 90, 40, 10, 0.5f, 0.5f, 0.5f);
    rdrawRectangle(73, 80, 50, 10, 0.6f, 0.6f, 0.6f);
    rdrawRectangle(67, 70, 62, 10, 0.7f, 0.7f, 0.7f);

    // Chimney
    rdrawRectangle(150, 280, 13, 25, 0.5f, 0.2f, 0.2f);



    // ===== RIGHT TOWER =====
    rdrawRectangle(230, 60, 35, 265, 1.0f, 0.7f, 0.4f); // tower wall

    // Roof caps
    rdrawRectangle(225, 325, 45, 10, 0.35f, 0.2f, 0.1f); // tower cap
    rdrawRectangle(138, 280, 93, 5, 0.35f, 0.2f, 0.1f);  // middle cap
    rdrawRectangle(52, 380, 92, 5, 0.35f, 0.2f, 0.1f);   // left cap
    rdrawRectangle(147, 300, 19, 5, 0.35f, 0.2f, 0.1f);

    // Tower window with frame
    float tx = 238, ty = 260, tw = 20, th = 40;
    float frame = 2.0f;
    rdrawRectangle(tx - frame, ty - frame, tw + 1.5f * frame, th + 2 * frame, 0.35f, 0.2f, 0.1f); // frame
    rdrawRectangle(tx, ty, tw, th, 0.75f, 0.95f, 1.0f); // glass
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINES);
        glVertex2f(tx + tw / 2, ty); glVertex2f(tx + tw / 2, ty + th);
        glVertex2f(tx, ty + th / 2); glVertex2f(tx + tw, ty + th / 2);
    glEnd();

    glFlush();
}

/* ================= BUILDING 2 ================= */

void rdrawPerfectBuilding() {
    rdrawRectangle(40, 60, 100, 260, 0.85f, 0.85f, 0.9f);
    rdrawRectangle(260, 60, 100, 260, 0.85f, 0.85f, 0.9f);
    rdrawRectangle(140, 60, 120, 320, 0.75f, 0.75f, 0.85f);

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 2; c++) {
            rdrawWindow(50 + c * 45, 80 + r * 80, 35, 60);
            rdrawWindow(270 + c * 45, 80 + r * 80, 35, 60);
        }

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rdrawWindow(150 + c * 45, 80 + r * 100, 40, 80);

    rdrawDoor(175, 60, 50, 60);
}

/* ================= BUILDING 3 ================= */

void rdrawFramedWindow3(float x, float y) {
    rdrawRectangle(x - 8, y - 2, 38, 34, 1, 1, 1);
    rdrawRectangle(x - 4, y, 30, 30, 0.2f, 0.5f, 0.8f);
}

void rdrawTowerWindows3(float bx, float by) {
    for (int i = 0; i < 5; i++) {
        rdrawFramedWindow3(bx + 30, by + i * 90);
        rdrawFramedWindow3(bx + 90, by + i * 90);
    }
}

void rdrawMiddleStripWindows3() {
    for (int i = 0; i < 14; i++)
        rdrawFramedWindow3(390, 215 + i * 30);
}

void rdrawRoof3() {
    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(220, 640);
    glVertex2f(320, 740);
    glVertex2f(460, 640);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(340, 640);
    glVertex2f(480, 740);
    glVertex2f(581, 640);
    glEnd();
}

void rdrawGroundFloor3() {
    rdrawRectangle(225, 20, 350, 80, 0.4f, 0.1f, 0.1f);

    rdrawRectangle(240, 20, 95, 60, 0.1f, 0.1f, 0.1f);
    rdrawRectangle(352, 20, 95, 60, 0.1f, 0.1f, 0.1f);
    rdrawRectangle(465, 20, 95, 60, 0.1f, 0.1f, 0.1f);
}

void rdrawBuilding3() {
    glPushMatrix();
    glScalef(0.5f, 0.5f, 1.0f);

    rdrawRectangle(230, 100, 200, 540, 1.0f, 0.8f, 0.6f);
    rdrawRectangle(370, 100, 200, 540, 1.0f, 0.8f, 0.6f);
    rdrawRectangle(370, 140, 60, 530, 0.8f, 0.2f, 0.2f);

    rdrawTowerWindows3(230, 200);
    rdrawTowerWindows3(430, 200);
    rdrawMiddleStripWindows3();
    rdrawRoof3();
    rdrawGroundFloor3();

    glPopMatrix();
}

/* ================= BUILDING 4 ================= */
void rdrawBuilding4() {
    glPushMatrix();
    glScalef(0.67f, 0.67f, 1.0f);

    float winWidth = 20, winHeight = 15;
    float frameThickness = 2;

    rdrawRectangle(110, 60, 180, 490, 0.94f, 0.87f, 0.75f);
    rdrawRectangle(110, 60, 140, 470, 1.0f, 0.6f, 0.3f);
    rdrawRectangle(250, 60, 80, 380, 0.94f, 0.87f, 0.75f);
    rdrawRectangle(70, 60, 40, 450, 0.8f, 0.35f, 0.0f);

    rdrawRectangle(128, 110, 103, 14, 1.0f, 1.0f, 1.0f);
    rdrawRectangle(221, 60, 7, 55, 1.0f, 1.0f, 1.0f);
    rdrawRectangle(132, 60, 7, 55, 1.0f, 1.0f, 1.0f);
    rdrawRectangle(146, 60, 68, 44, 0.6f, 0.2f, 0.0f);
    rdrawRectangle(150, 60, 60, 40, 1.0f, 0.9f, 0.8f);
    rdrawRectangle(178, 60, 2, 40, 0.8f, 0.35f, 0.0f);
    rdrawRectangle(153, 77, 20, 20, 0.0f, 0.5f, 0.7f);
    rdrawRectangle(187, 77, 20, 20, 0.0f, 0.5f, 0.7f);

    float startX = 120, startY = 140;
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 6; ++j) {
            float x = startX + j * 20;
            float y = startY + i * 38;
            rdrawRectangle(x - frameThickness, y - frameThickness,
                          winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                          1.0f, 1.0f, 1.0f);
            rdrawRectangle(x, y, winWidth, winHeight, 0.0f, 0.6f, 0.9f);
        }
    }

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 2; ++j) {
            float x = 260 + j * 40;
            float y = 140 + i * 38;
            rdrawRectangle(x - frameThickness, y - frameThickness,
                          winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                          1.0f, 1.0f, 1.0f);
            rdrawRectangle(x, y, winWidth, winHeight, 0.0f, 0.6f, 0.9f);
        }
    }

    for (int i = 0; i < 10; ++i) {
        float x = 80;
        float y = 140 + i * 38;
        rdrawRectangle(x - frameThickness, y - frameThickness,
                      winWidth + 2 * frameThickness, winHeight + 2 * frameThickness,
                      1.0f, 1.0f, 1.0f);
        rdrawRectangle(x, y, winWidth, winHeight, 0.03f, 0.6f, 0.9f);
    }

    glPopMatrix();
}

/* ================= BUILDING 5 - SCHOOL ================= */
void rdrawSchool() {
    // Main building - yellow
    rdrawRectangle(60, 80, 280, 240, 1.0f, 0.95f, 0.6f);

    // Roof - red
    glColor3f(0.9f, 0.2f, 0.2f);
    glBegin(GL_TRIANGLES);
    glVertex2f(50, 320);
    glVertex2f(200, 380);
    glVertex2f(350, 320);
    glEnd();

    // Clock tower
    rdrawRectangle(180, 320, 40, 60, 1.0f, 0.95f, 0.6f);
    rdrawCircle(200, 350, 15, 1.0f, 1.0f, 1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(200, 350);
    glVertex2f(200, 360);
    glVertex2f(200, 350);
    glVertex2f(207, 350);
    glEnd();

    // Windows - 3 rows, 5 columns
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 5; c++)
            rdrawWindow(80 + c * 50, 120 + r * 60, 35, 45);

    // Main entrance door - double door
    rdrawRectangle(170, 80, 60, 70, 0.4f, 0.2f, 0.1f);
    rdrawRectangle(173, 80, 25, 65, 0.7f, 0.5f, 0.3f);
    rdrawRectangle(202, 80, 25, 65, 0.7f, 0.5f, 0.3f);
}

/* ================= BUILDING 6 - HOSPITAL ================= */
void rdrawHospital() {
    // Main building - white/light blue (darker at night)
    if (isNight) {
        rdrawRectangle(50, 70, 300, 280, 0.75f, 0.85f, 0.95f);     // Slightly darker blue tint
    } else {
        rdrawRectangle(50, 70, 300, 280, 0.95f, 0.98f, 1.0f);       // Daytime bright
    }

    // Red cross on top - brighter + subtle glow at night
    if (isNight) {
        rdrawRectangle(180, 350, 40, 15, 1.0f, 0.15f, 0.15f);       // Stronger red
        rdrawRectangle(188, 342, 24, 30, 1.0f, 0.15f, 0.15f);

        // Soft red glow around cross
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.3f, 0.3f, 0.35f);
        rdrawCircle(200, 350, 40, 1.0f, 0.3f, 0.3f);
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(180, 350, 40, 15, 1.0f, 0.0f, 0.0f);
        rdrawRectangle(188, 342, 24, 30, 1.0f, 0.0f, 0.0f);
    }

    // Windows - medical blue tint during day, warm glowing light at night
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 6; col++) {
            float wx = 70 + col * 45;
            float wy = 100 + row * 60;

            // Window frame (darker at night)
            if (isNight) {
                rdrawRectangle(wx - 2, wy - 2, 34, 44, 0.2f, 0.2f, 0.25f);
            } else {
                rdrawRectangle(wx - 2, wy - 2, 34, 44, 0.3f, 0.3f, 0.3f);
            }

            // Window glass - glowing warm light at night
            if (isNight) {
                rdrawRectangle(wx, wy, 30, 40, 1.0f, 0.85f, 0.55f);     // Warm yellow-orange glow

                // Inner shine for more realistic lighting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.92f, 0.65f, 0.55f);
                rdrawRectangle(wx + 3, wy + 3, 24, 34, 1.0f, 0.92f, 0.65f);
                glDisable(GL_BLEND);
            } else {
                rdrawRectangle(wx, wy, 30, 40, 0.7f, 0.95f, 1.0f);      // Daytime cool blue
            }
        }
    }

    // Emergency entrance - lit interior at night
    if (isNight) {
        rdrawRectangle(150, 70, 100, 80, 1.0f, 0.6f, 0.2f);         // Warm orange interior
        rdrawRectangle(160, 75, 80, 70, 1.0f, 0.85f, 0.5f);         // Bright inside glow
    } else {
        rdrawRectangle(150, 70, 100, 80, 1.0f, 0.0f, 0.0f);         // Red daytime
        rdrawRectangle(160, 75, 80, 70, 1.0f, 1.0f, 1.0f);          // White
    }

    // Ambulance bay - subtle lighting at night
    if (isNight) {
        rdrawRectangle(165, 75, 30, 50, 0.3f, 0.3f, 0.35f);
        rdrawRectangle(205, 75, 30, 50, 0.3f, 0.3f, 0.35f);
        // Small glow inside bays
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.9f, 0.6f, 0.3f);
        rdrawCircle(180, 100, 15, 1.0f, 0.9f, 0.6f);
        rdrawCircle(220, 100, 15, 1.0f, 0.9f, 0.6f);
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(165, 75, 30, 50, 0.0f, 0.0f, 0.0f);
        rdrawRectangle(205, 75, 30, 50, 0.0f, 0.0f, 0.0f);
    }
}

/* ================= BUILDING 7 - POLICE STATION ================= */
void rdrawPoliceStation() {
    // Main building - blue/gray (darker at night)
    if (isNight) {
        rdrawRectangle(60, 80, 280, 260, 0.2f, 0.3f, 0.5f);     // Darker night tint
    } else {
        rdrawRectangle(60, 80, 280, 260, 0.3f, 0.4f, 0.6f);     // Daytime color
    }

    // Top banner - darker blue + subtle glow at night
    if (isNight) {
        rdrawRectangle(60, 300, 280, 40, 0.05f, 0.15f, 0.4f);
        // Soft blue glow on banner
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(0.4f, 0.6f, 1.0f, 0.3f);
        rdrawRectangle(62, 302, 276, 36, 0.4f, 0.6f, 1.0f);
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(60, 300, 280, 40, 0.1f, 0.2f, 0.4f);
    }

    // Police badge/emblem - brighter + glowing at night
    if (isNight) {
        rdrawCircle(200, 320, 18, 1.0f, 0.9f, 0.3f);             // Bright gold
        rdrawCircle(200, 320, 12, 0.2f, 0.4f, 0.9f);             // Bright blue center
        // Subtle glow around badge
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.95f, 0.6f, 0.35f);
        rdrawCircle(200, 320, 28, 1.0f, 0.95f, 0.6f);
        glDisable(GL_BLEND);
    } else {
        rdrawCircle(200, 320, 18, 1.0f, 0.8f, 0.0f);
        rdrawCircle(200, 320, 12, 0.1f, 0.2f, 0.4f);
    }

    // Windows - day: cool blue, night: warm glowing interior light
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 5; c++) {
            float wx = 80 + c * 50;
            float wy = 140 + r * 60;

            // Window frame (darker at night)
            if (isNight) {
                rdrawRectangle(wx - 2, wy - 2, 34, 44, 0.15f, 0.15f, 0.2f);
            } else {
                rdrawRectangle(wx - 2, wy - 2, 34, 44, 0.2f, 0.2f, 0.3f);
            }

            // Window glass - glowing at night
            if (isNight) {
                rdrawRectangle(wx, wy, 30, 40, 1.0f, 0.88f, 0.55f);     // Warm yellow-white glow

                // Inner shine for realistic lighting
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.94f, 0.7f, 0.55f);
                rdrawRectangle(wx + 3, wy + 3, 24, 34, 1.0f, 0.94f, 0.7f);
                glDisable(GL_BLEND);
            } else {
                rdrawRectangle(wx, wy, 30, 40, 0.5f, 0.6f, 0.7f);       // Daytime cool blue
            }
        }
    }

    // Main entrance - lit interior at night
    if (isNight) {
        rdrawRectangle(160, 80, 80, 70, 0.9f, 0.7f, 0.4f);          // Warm interior light
        rdrawRectangle(165, 85, 70, 60, 1.0f, 0.9f, 0.6f);          // Bright inside
    } else {
        rdrawRectangle(160, 80, 80, 70, 0.2f, 0.3f, 0.4f);
        rdrawRectangle(165, 85, 70, 60, 0.6f, 0.7f, 0.8f);
    }

    // Garage doors for police cars - subtle lighting at night
    if (isNight) {
        rdrawRectangle(70, 80, 60, 50, 0.3f, 0.35f, 0.4f);
        rdrawRectangle(270, 80, 60, 50, 0.3f, 0.35f, 0.4f);
        // Small glow inside garages
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.9f, 0.6f, 0.3f);
        rdrawCircle(100, 105, 20, 1.0f, 0.9f, 0.6f);   // Left garage
        rdrawCircle(300, 105, 20, 1.0f, 0.9f, 0.6f);   // Right garage
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(70, 80, 60, 50, 0.4f, 0.4f, 0.4f);
        rdrawRectangle(270, 80, 60, 50, 0.4f, 0.4f, 0.4f);
    }
}
/* ================= BUILDING 8 - SHOPPING MALL ================= */
void rdrawShoppingMall() {
    // Main structure - modern glass (darker at night)
    if (isNight) {
        rdrawRectangle(40, 60, 320, 290, 0.5f, 0.6f, 0.75f);   // Darker reflective glass
    } else {
        rdrawRectangle(40, 60, 320, 290, 0.85f, 0.9f, 0.95f);
    }

    // Upper section - subtle glow at night
    if (isNight) {
        rdrawRectangle(80, 280, 240, 70, 0.1f, 0.4f, 0.7f);
    } else {
        rdrawRectangle(80, 280, 240, 70, 0.2f, 0.6f, 0.8f);
    }

    // Large display windows (glow handled by rdrawWindow() if you use it later)
    for (int c = 0; c < 3; c++) {
        float wx = 60 + c * 100;
        if (isNight) {
            rdrawRectangle(wx - 3, 100 - 3, 76, 156, 0.2f, 0.2f, 0.3f);
            rdrawRectangle(wx, 100, 70, 150, 1.0f, 0.85f, 0.5f);   // Warm glow
        } else {
            rdrawRectangle(wx - 3, 100 - 3, 76, 156, 0.3f, 0.3f, 0.4f);
            rdrawRectangle(wx, 100, 70, 150, 0.7f, 0.85f, 0.95f);
        }
    }

    // Upper windows (simple glow)
    for (int c = 0; c < 6; c++) {
        float ux = 90 + c * 40;
        if (isNight) {
            rdrawRectangle(ux - 2, 295 - 2, 28, 38, 0.2f, 0.2f, 0.3f);
            rdrawRectangle(ux, 295, 24, 34, 1.0f, 0.9f, 0.6f);     // Warm light
        } else {
            rdrawRectangle(ux - 2, 295 - 2, 28, 38, 0.3f, 0.3f, 0.4f);
            rdrawRectangle(ux, 295, 24, 34, 0.6f, 0.8f, 1.0f);
        }
    }

    // Entrance - automatic doors (lit interior at night)
    if (isNight) {
        rdrawRectangle(150, 60, 100, 80, 0.8f, 0.7f, 0.4f);     // Warm interior
        rdrawRectangle(155, 65, 40, 70, 1.0f, 0.95f, 0.7f);     // Bright left door
        rdrawRectangle(205, 65, 40, 70, 1.0f, 0.95f, 0.7f);     // Bright right door
    } else {
        rdrawRectangle(150, 60, 100, 80, 0.1f, 0.1f, 0.2f);
        rdrawRectangle(155, 65, 40, 70, 0.5f, 0.7f, 0.9f);
        rdrawRectangle(205, 65, 40, 70, 0.5f, 0.7f, 0.9f);
    }
}

/* ================= BUILDING 9 - RESTAURANT ================= */
void rdrawRestaurant() {
    // Main building - warm colors
    rdrawRectangle(70, 90, 260, 250, 0.95f, 0.8f, 0.6f);

    // Awning - striped
    rdrawRectangle(60, 280, 280, 40, 0.9f, 0.3f, 0.2f);
    for (int i = 0; i < 7; i++) {
        rdrawRectangle(60 + i * 40, 280, 20, 40, 1.0f, 1.0f, 1.0f);
    }

    // Large front windows
    rdrawRectangle(80 - 3, 130 - 3, 106, 126, 0.3f, 0.2f, 0.1f);
    rdrawRectangle(80, 130, 100, 120, 0.8f, 0.95f, 1.0f);

    rdrawRectangle(220 - 3, 130 - 3, 106, 126, 0.3f, 0.2f, 0.1f);
    rdrawRectangle(220, 130, 100, 120, 0.8f, 0.95f, 1.0f);

    // Upper windows
    for (int c = 0; c < 4; c++) {
        float x = 100 + c * 55;
        rdrawWindow(x, 270, 35, 40);
    }

    // Entrance door with glass
    rdrawRectangle(175, 90, 50, 75, 0.6f, 0.3f, 0.2f);
    rdrawRectangle(180, 95, 40, 65, 0.7f, 0.9f, 1.0f);
}
void rdrawApartment() {
    // Main structure
    rdrawRectangle(60, 60, 260, 300, 0.9f, 0.9f, 0.85f);

    // Roof
    rdrawRectangle(50, 360, 280, 15, 0.4f, 0.3f, 0.2f);

    // Windows (4x4)
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            rdrawWindow(80 + c * 55, 100 + r * 60, 35, 45);
        }
    }

    // Entrance
    rdrawDoor(175, 60, 50, 60);
}
void rdrawLibrary() {
    // Main building
    rdrawRectangle(70, 80, 260, 260, 0.85f, 0.75f, 0.6f);

    // Top banner
    rdrawRectangle(70, 300, 260, 40, 0.5f, 0.3f, 0.2f);

    // Columns
    for (int i = 0; i < 4; i++) {
        rdrawRectangle(100 + i * 45, 80, 20, 120, 0.95f, 0.95f, 0.9f);
    }

    // Windows
    for (int i = 0; i < 5; i++) {
        rdrawWindow(90 + i * 45, 220, 30, 40);
    }

    // Door
    rdrawDoor(180, 80, 40, 70);
}
void rdrawModernGlassTower() {
    // Main tower - glass effect with gradient
    rdrawRectangle(80, 60, 240, 380, 0.7f, 0.85f, 0.95f);

    // Vertical accent strips
    rdrawRectangle(100, 60, 30, 380, 0.2f, 0.4f, 0.6f);
    rdrawRectangle(270, 60, 30, 380, 0.2f, 0.4f, 0.6f);

    // Reflective glass windows in grid pattern
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 4; c++) {
            float x = 140 + c * 35;
            float y = 80 + r * 45;
            rdrawRectangle(x - 1, y - 1, 27, 37, 0.3f, 0.5f, 0.7f);
            rdrawRectangle(x, y, 25, 35, 0.6f, 0.85f, 1.0f);
        }
    }

    // Rooftop helipad
    rdrawCircle(200, 420, 25, 0.9f, 0.3f, 0.1f);
    rdrawCircle(200, 420, 15, 0.2f, 0.2f, 0.2f);

    // Modern entrance
    rdrawRectangle(160, 60, 80, 70, 0.1f, 0.1f, 0.1f);
    rdrawRectangle(165, 65, 70, 60, 0.5f, 0.8f, 0.95f);
}

/* ================= BUILDING 11 - ART DECO SKYSCRAPER ================= */
void rdrawModernGlass() {
    // Main tower - glass effect with gradient (darker at night)
    if (isNight) {
        // Night: darker reflective glass
        rdrawRectangle(80, 60, 240, 380, 0.25f, 0.35f, 0.55f);          // Deep blue tint
    } else {
        // Day: normal light blue glass
        rdrawRectangle(80, 60, 240, 380, 0.7f, 0.85f, 0.95f);
    }

    // Vertical accent strips (brighter at night)
    if (isNight) {
        rdrawRectangle(100, 60, 30, 380, 0.4f, 0.6f, 0.9f);   // Glow blue
        rdrawRectangle(270, 60, 30, 380, 0.4f, 0.6f, 0.9f);
    } else {
        rdrawRectangle(100, 60, 30, 380, 0.2f, 0.4f, 0.6f);
        rdrawRectangle(270, 60, 30, 380, 0.2f, 0.4f, 0.6f);
    }

    // Reflective glass windows in grid pattern
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 4; c++) {
            float wx = 140 + c * 35;
            float wy = 80 + r * 45;

            // Window frame (darker at night)
            if (isNight) {
                rdrawRectangle(wx - 1, wy - 1, 27, 37, 0.15f, 0.25f, 0.4f);
            } else {
                rdrawRectangle(wx - 1, wy - 1, 27, 37, 0.3f, 0.5f, 0.7f);
            }

            // Window glass - glowing at night
            if (isNight) {
                // Warm lit window (yellow-orange glow)
                rdrawRectangle(wx, wy, 25, 35, 1.0f, 0.85f, 0.5f);

                // Inner shine for extra glow
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(1.0f, 0.92f, 0.65f, 0.6f);
                rdrawRectangle(wx + 3, wy + 3, 19, 29, 1.0f, 0.92f, 0.65f);
                glDisable(GL_BLEND);
            } else {
                rdrawRectangle(wx, wy, 25, 35, 0.6f, 0.85f, 1.0f);
            }
        }
    }



    // Modern entrance - lit interior at night
    if (isNight) {
        rdrawRectangle(160, 60, 80, 70, 0.8f, 0.7f, 0.3f);     // Warm interior light
        rdrawRectangle(165, 65, 70, 60, 1.0f, 0.95f, 0.7f);    // Bright inside
    } else {
        rdrawRectangle(160, 60, 80, 70, 0.1f, 0.1f, 0.1f);
        rdrawRectangle(165, 65, 70, 60, 0.5f, 0.8f, 0.95f);
    }
}
void rdrawLuxuryHotel() {
    // Base pedestal
    rdrawRectangle(60, 60, 280, 60, 0.8f, 0.7f, 0.6f);

    // Main tower
    rdrawRectangle(90, 120, 220, 260, 0.95f, 0.93f, 0.88f);

    // Gold trim bands
    rdrawRectangle(85, 120, 230, 8, 0.85f, 0.65f, 0.13f);
    rdrawRectangle(85, 200, 230, 8, 0.85f, 0.65f, 0.13f);
    rdrawRectangle(85, 280, 230, 8, 0.85f, 0.65f, 0.13f);
    rdrawRectangle(85, 372, 230, 8, 0.85f, 0.65f, 0.13f);

    // Balcony windows
    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 4; c++) {
            float x = 110 + c * 50;
            float y = 140 + r * 50;

            // Window frame - gold
            rdrawRectangle(x - 3, y - 3, 36, 41, 0.85f, 0.65f, 0.13f);

            // Glass
            rdrawRectangle(x, y, 30, 35, 0.4f, 0.5f, 0.6f);

            // Balcony railing
            glColor3f(0.85f, 0.65f, 0.13f);
            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x + 30, y);
            glEnd();
            glLineWidth(1.0f);
        }
    }

    // Rooftop restaurant with dome
    rdrawRectangle(140, 380, 120, 30, 0.9f, 0.9f, 0.95f);

    // Dome
    glColor3f(0.85f, 0.65f, 0.13f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(200, 440);
    for (int i = 0; i <= 180; i += 10) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(200 + 60 * cos(angle), 410 + 30 * sin(angle));
    }
    glEnd();

    // Hotel sign
    rdrawRectangle(150, 340, 100, 25, 0.1f, 0.1f, 0.2f);

    // Letters effect
    glColor3f(1.0f, 0.9f, 0.0f);
    for (int i = 0; i < 5; i++) {
        rdrawRectangle(160 + i * 18, 345, 12, 15, 1.0f, 0.9f, 0.0f);
    }

    // Grand entrance with canopy
    rdrawRectangle(150, 60, 100, 90, 0.7f, 0.1f, 0.1f);
    rdrawRectangle(155, 65, 90, 80, 0.95f, 0.95f, 1.0f);

    // Canopy
    rdrawRectangle(130, 135, 140, 10, 0.85f, 0.65f, 0.13f);

    // Entrance pillars
    rdrawRectangle(155, 60, 15, 75, 0.9f, 0.9f, 0.85f);
    rdrawRectangle(230, 60, 15, 75, 0.9f, 0.9f, 0.85f);
}
void rdrawEcoGreenBuilding() {
    // Main structure - earth tones (darker at night)
    if (isNight) {
        rdrawRectangle(70, 60, 260, 460, 0.65f, 0.62f, 0.55f);   // Darker eco tone
    } else {
        rdrawRectangle(70, 60, 260, 460, 0.85f, 0.82f, 0.7f);
    }

    // Green vertical gardens - subtle glow at night
    if (isNight) {
        rdrawRectangle(75, 100, 35, 400, 0.15f, 0.5f, 0.25f);
        rdrawRectangle(290, 100, 35, 400, 0.15f, 0.5f, 0.25f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(0.3f, 0.9f, 0.45f, 0.35f);
        rdrawRectangle(75, 100, 35, 400, 0.3f, 0.9f, 0.45f);
        rdrawRectangle(290, 100, 35, 400, 0.3f, 0.9f, 0.45f);
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(75, 100, 35, 400, 0.2f, 0.6f, 0.3f);
        rdrawRectangle(290, 100, 35, 400, 0.2f, 0.6f, 0.3f);
    }

    // Solar panels on roof
    for (int i = 0; i < 4; i++) {
        if (isNight) {
            rdrawRectangle(90 + i * 55, 500, 45, 20, 0.05f, 0.05f, 0.2f);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(0.4f, 0.8f, 1.0f, 0.3f);  // Soft blue glow
            rdrawRectangle(92 + i * 55, 502, 41, 16, 0.4f, 0.8f, 1.0f);
            glDisable(GL_BLEND);
        } else {
            rdrawRectangle(90 + i * 55, 500, 45, 20, 0.1f, 0.1f, 0.3f);
        }

        // Grid lines (unchanged)
        glColor3f(0.3f, 0.3f, 0.5f);
        glLineWidth(1.0f);
        for (int j = 0; j < 3; j++) {
            glBegin(GL_LINES);
            glVertex2f(90 + i * 55 + j * 15, 500);
            glVertex2f(90 + i * 55 + j * 15, 520);
            glEnd();
        }
    }
    glLineWidth(1.0f);

    // Terraced balconies with plants
    for (int r = 0; r < 8; r++) {
        float by = 100 + r * 52;
        if (isNight) {
            rdrawRectangle(115, by, 170, 8, 0.45f, 0.4f, 0.35f);
        } else {
            rdrawRectangle(115, by, 170, 8, 0.6f, 0.5f, 0.4f);
        }

        for (int p = 0; p < 5; p++) {
            float px = 125 + p * 30;
            if (isNight) {
                rdrawCircle(px, by + 8, 5, 0.3f, 0.9f, 0.4f);  // Glowing plants
            } else {
                rdrawCircle(px, by + 8, 5, 0.1f, 0.5f, 0.2f);
            }
        }
    }

    // Large energy-efficient windows - glowing at night
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 3; c++) {
            float wx = 130 + c * 55;
            float wy = 115 + r * 52;

            if (isNight) {
                rdrawRectangle(wx - 2, wy - 2, 44, 44, 0.2f, 0.3f, 0.25f);
                rdrawRectangle(wx, wy, 40, 40, 0.7f, 0.95f, 0.65f);  // Eco green glow
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glColor4f(0.85f, 1.0f, 0.8f, 0.5f);
                rdrawRectangle(wx + 4, wy + 4, 32, 32, 0.85f, 1.0f, 0.8f);
                glDisable(GL_BLEND);
            } else {
                rdrawRectangle(wx - 2, wy - 2, 44, 44, 0.3f, 0.4f, 0.3f);
                rdrawRectangle(wx, wy, 40, 40, 0.7f, 0.85f, 0.75f);
            }
        }
    }

    // Wind turbine
    if (isNight) {
        rdrawRectangle(195, 520, 10, 80, 0.75f, 0.75f, 0.8f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);
        rdrawRectangle(193, 520, 14, 80, 1.0f, 1.0f, 1.0f);
        glDisable(GL_BLEND);
    } else {
        rdrawRectangle(195, 520, 10, 80, 0.9f, 0.9f, 0.9f);
    }

    // Turbine blades
    glColor3f(0.95f, 0.95f, 0.95f);
    glLineWidth(3.0f);
    for (int i = 0; i < 3; i++) {
        float angle = i * 3.14159f * 2.0f / 3.0f;
        glBegin(GL_LINES);
        glVertex2f(200, 570);
        glVertex2f(200 + 30 * cos(angle), 570 + 30 * sin(angle));
        glEnd();
    }
    glLineWidth(1.0f);

    // Eco entrance - lit at night
    if (isNight) {
        rdrawRectangle(165, 60, 70, 75, 0.8f, 0.7f, 0.4f);     // Warm light
        rdrawRectangle(170, 65, 60, 65, 1.0f, 0.9f, 0.6f);     // Bright glow
    } else {
        rdrawRectangle(165, 60, 70, 75, 0.4f, 0.3f, 0.2f);
        rdrawRectangle(170, 65, 60, 65, 0.6f, 0.7f, 0.5f);
    }
}
void rdrawMetroRail() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, 1600, 900);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int metroY = 225;

    // DHAKA METRO PILLARS - cooler/darker at night
    for (int px = 80; px < 1600; px += 180) {
        // Pillar base
        if (isNight) {
            rdrawRectangle(px - 22, metroY - 95, 44, 8, 0.35f, 0.35f, 0.38f);
        } else {
            rdrawRectangle(px - 22, metroY - 95, 44, 8, 0.45f, 0.45f, 0.48f);
        }

        // Main pillar with slight gradient
        glBegin(GL_QUADS);
        if (isNight) {
            glColor3f(0.50f, 0.50f, 0.55f);
        } else {
            glColor3f(0.65f, 0.65f, 0.68f);
        }
        glVertex2f(px - 18, metroY - 87);
        glVertex2f(px + 18, metroY - 87);
        if (isNight) {
            glColor3f(0.55f, 0.55f, 0.60f);
        } else {
            glColor3f(0.70f, 0.70f, 0.73f);
        }
        glVertex2f(px + 15, metroY + 35);
        glVertex2f(px - 15, metroY + 35);
        glEnd();

        // Pillar cap
        if (isNight) {
            rdrawRectangle(px - 25, metroY + 35, 50, 12, 0.45f, 0.45f, 0.50f);
        } else {
            rdrawRectangle(px - 25, metroY + 35, 50, 12, 0.55f, 0.55f, 0.58f);
        }

        // Red/Green bands - brighter at night
        rdrawRectangle(px - 18, metroY - 20, 36, 4, 0.95f, 0.15f, 0.15f);
        rdrawRectangle(px - 18, metroY + 10, 36, 4, 0.0f, 0.65f, 0.25f);
    }

    // ELEVATED TRACK BOX STRUCTURE
    if (isNight) {
        rdrawRectangle(0, metroY + 46, 1600, 3, 0.20f, 0.20f, 0.25f);         // Darker shadow
        rdrawRectangle(0, metroY + 49, 1600, 38, 0.65f, 0.35f, 0.25f);         // Darker concrete
    } else {
        rdrawRectangle(0, metroY + 46, 1600, 3, 0.30f, 0.30f, 0.33f);
        rdrawRectangle(0, metroY + 49, 1600, 38, 0.85f, 0.45f, 0.30f);
    }

    // Side detail lines
    glColor3f(0.80f, 0.50f, 0.35f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 55); glVertex2f(1600, metroY + 55);
    glVertex2f(0, metroY + 80); glVertex2f(1600, metroY + 80);
    glEnd();

    // RAIL TRACKS
    if (isNight) {
        glColor3f(0.50f, 0.50f, 0.55f);
    } else {
        glColor3f(0.35f, 0.35f, 0.38f);
    }
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 62); glVertex2f(1600, metroY + 62);
    glVertex2f(0, metroY + 74); glVertex2f(1600, metroY + 74);
    glEnd();

    // OVERHEAD WIRES
    for (int px = 80; px < 1600; px += 180) {
        if (isNight) {
            rdrawRectangle(px - 3, metroY + 90, 6, 35, 0.45f, 0.45f, 0.50f);
            rdrawRectangle(px - 25, metroY + 123, 50, 4, 0.50f, 0.50f, 0.55f);
        } else {
            rdrawRectangle(px - 3, metroY + 90, 6, 35, 0.35f, 0.35f, 0.38f);
            rdrawRectangle(px - 25, metroY + 123, 50, 4, 0.40f, 0.40f, 0.43f);
        }
    }
    if (isNight) {
        glColor3f(0.45f, 0.45f, 0.50f);
    } else {
        glColor3f(0.25f, 0.25f, 0.28f);
    }
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0, metroY + 120); glVertex2f(1600, metroY + 120);
    glEnd();

    // METRO TRAIN
    float trainX = metroTrainX;
    float trainY = metroY + 90;
    float trainLength = 280;
    float trainHeight = 50;

    // Train shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.2f);
    rdrawRectangle(trainX + 5, trainY - 15, trainLength, 12, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Main train body
    if (isNight) {
        rdrawRectangle(trainX, trainY, trainLength, trainHeight, 0.85f, 0.85f, 0.90f);
    } else {
        rdrawRectangle(trainX, trainY, trainLength, trainHeight, 0.95f, 0.95f, 0.97f);
    }

    // Front nose
    glColor3f(0.88f, 0.88f, 0.92f);
    glBegin(GL_TRIANGLES);
    glVertex2f(trainX + trainLength, trainY);
    glVertex2f(trainX + trainLength + 25, trainY + 15);
    glVertex2f(trainX + trainLength, trainY + 30);
    glEnd();

    // Train roof
    rdrawRectangle(trainX, trainY + trainHeight, trainLength, 8, 0.80f, 0.80f, 0.85f);

    // Green stripe
    rdrawRectangle(trainX, trainY + 8, trainLength, 8, 0.0f, 0.60f, 0.30f);
    rdrawRectangle(trainX, trainY + trainHeight - 8, trainLength, 8, 0.0f, 0.60f, 0.30f);

    // Windows - dark tinted by day, warm yellow interior reflection/glow at night
    for (int i = 0; i < 8; i++) {
        float wx = trainX + 15 + (i * 33);
        if (isNight) {
            // Base warm yellow interior light (reflection on glass)
            rdrawRectangle(wx, trainY + 20, 28, 22, 1.0f, 0.92f, 0.65f);

            // Subtle yellow reflection glow (realistic interior light bouncing on glass)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1.0f, 0.95f, 0.75f, 0.45f);  // Soft yellow reflection
            rdrawRectangle(wx + 2, trainY + 22, 24, 18, 1.0f, 0.95f, 0.75f);
            glDisable(GL_BLEND);
        } else {
            // Daytime: dark tinted windows
            rdrawRectangle(wx, trainY + 20, 28, 22, 0.15f, 0.20f, 0.30f);
        }

        // Window frame (unchanged)
        glColor3f(0.75f, 0.75f, 0.78f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(wx, trainY + 20);
        glVertex2f(wx + 28, trainY + 20);
        glVertex2f(wx + 28, trainY + 42);
        glVertex2f(wx, trainY + 42);
        glEnd();
    }

    // Front windshield
    rdrawRectangle(trainX + trainLength - 20, trainY + 22, 18, 20, 0.20f, 0.25f, 0.35f);

    // Train door (red emergency marking)
    rdrawRectangle(trainX + 130, trainY + 2, 4, trainHeight - 4, 0.9f, 0.1f, 0.1f);

    // Undercarriage
    rdrawRectangle(trainX + 10, trainY - 5, trainLength - 20, 5, 0.30f, 0.30f, 0.35f);

    // Wheels/Bogies
    for (int i = 0; i < 3; i++) {
        float wx = trainX + 40 + (i * 100);
        rdrawCircle(wx, trainY - 8, 8, 0.20f, 0.20f, 0.25f);
        rdrawCircle(wx, trainY - 8, 5, 0.40f, 0.40f, 0.45f);
        rdrawCircle(wx + 30, trainY - 8, 8, 0.20f, 0.20f, 0.25f);
        rdrawCircle(wx + 30, trainY - 8, 5, 0.40f, 0.40f, 0.45f);
    }

    // Pantograph
    float pantoX = trainX + trainLength / 2;
    rdrawRectangle(pantoX - 3, trainY + 58, 6, 25, 0.35f, 0.35f, 0.40f);
    rdrawRectangle(pantoX - 15, trainY + 83, 30, 3, 0.30f, 0.30f, 0.35f);

    // Connection to overhead wire
    glColor3f(0.40f, 0.40f, 0.45f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(pantoX, trainY + 86);
    glVertex2f(pantoX, metroY + 120);
    glEnd();

    // METRO STATION
    int stationX = 700;

    // Platform
    if (isNight) {
        rdrawRectangle(stationX - 120, metroY + 49, 240, 38, 0.85f, 0.85f, 0.90f);
    } else {
        rdrawRectangle(stationX - 120, metroY + 49, 240, 38, 0.70f, 0.70f, 0.73f);
    }

    // Yellow safety line
    rdrawRectangle(stationX - 115, metroY + 52, 230, 4, 1.0f, 0.95f, 0.0f);
    rdrawRectangle(stationX - 115, metroY + 81, 230, 4, 1.0f, 0.95f, 0.0f);

    // Station canopy supports
    if (isNight) {
        rdrawRectangle(stationX - 100, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
        rdrawRectangle(stationX, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
        rdrawRectangle(stationX + 100, metroY + 90, 10, 50, 0.60f, 0.60f, 0.65f);
    } else {
        rdrawRectangle(stationX - 100, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
        rdrawRectangle(stationX, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
        rdrawRectangle(stationX + 100, metroY + 90, 10, 50, 0.50f, 0.50f, 0.55f);
    }

    // Glass canopy
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (isNight) {
        glColor4f(0.8f, 0.95f, 1.0f, 0.6f);
    } else {
        glColor4f(0.6f, 0.8f, 0.95f, 0.4f);
    }
    glBegin(GL_QUADS);
    glVertex2f(stationX - 130, metroY + 140);
    glVertex2f(stationX + 130, metroY + 140);
    glVertex2f(stationX + 120, metroY + 175);
    glVertex2f(stationX - 120, metroY + 175);
    glEnd();
    glDisable(GL_BLEND);

    // Canopy frame
    if (isNight) {
        glColor3f(0.60f, 0.60f, 0.65f);
    } else {
        glColor3f(0.40f, 0.40f, 0.45f);
    }
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(stationX - 130, metroY + 140);
    glVertex2f(stationX + 130, metroY + 140);
    glVertex2f(stationX + 120, metroY + 175);
    glVertex2f(stationX - 120, metroY + 175);
    glEnd();

    // Station name board
    if (isNight) {
        rdrawRectangle(stationX - 70, metroY + 152, 140, 16, 0.0f, 0.75f, 0.35f);
    } else {
        rdrawRectangle(stationX - 70, metroY + 152, 140, 16, 0.0f, 0.55f, 0.28f);
    }

    // Station lights - simple yellow light (no glow, as per your request)
    for (int i = -50; i <= 50; i += 50) {
        if (isNight) {
            rdrawCircle(stationX + i, metroY + 160, 6, 1.0f, 0.98f, 0.70f);  // Soft yellow
        } else {
            rdrawCircle(stationX + i, metroY + 160, 5, 1.0f, 0.98f, 0.90f);
        }
    }

    glLineWidth(1.0f);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}
//=====================Vehicles==============//

// ================= VEHICLE DRAWING FUNCTIONS =================

/* CAR - Modern Sedan */
void rdrawCar(float x, float y, float r, float g, float b) {
    // Car shadow with gradient (same in day/night)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 48, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 45, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Lower car body (chassis)
    rdrawRectangle(x, y, 50, 12, r * 0.85f, g * 0.85f, b * 0.85f);

    // Main car body with slight curve effect
    rdrawRectangle(x + 2, y + 10, 46, 10, r, g, b);

    // Hood
    rdrawRectangle(x + 35, y + 10, 13, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Trunk
    rdrawRectangle(x + 2, y + 10, 10, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Car roof (cabin) - aerodynamic shape
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 14, y + 20);
    glVertex2f(x + 38, y + 20);
    glVertex2f(x + 36, y + 28);
    glVertex2f(x + 16, y + 28);
    glEnd();

    // Front windshield (angled)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 40, y + 18);
    glVertex2f(x + 38, y + 26);
    glVertex2f(x + 36, y + 28);
    glEnd();

    // Rear windshield (angled)
    glBegin(GL_QUADS);
    glVertex2f(x + 14, y + 28);
    glVertex2f(x + 16, y + 28);
    glVertex2f(x + 14, y + 26);
    glVertex2f(x + 10, y + 18);
    glEnd();

    // Side windows with frames
    rdrawRectangle(x + 25, y + 21, 9, 6, 0.3f, 0.3f, 0.35f);
    rdrawRectangle(x + 25.5f, y + 21.5f, 8, 5, 0.2f, 0.3f, 0.5f);
    rdrawRectangle(x + 17, y + 21, 7, 6, 0.3f, 0.3f, 0.35f);
    rdrawRectangle(x + 17.5f, y + 21.5f, 6, 5, 0.2f, 0.3f, 0.5f);

    // Door lines
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 24, y + 4); glVertex2f(x + 24, y + 20);
    glVertex2f(x + 14, y + 4); glVertex2f(x + 14, y + 20);
    glEnd();
    glLineWidth(1.0f);

    // Side mirrors
    rdrawRectangle(x + 36, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);
    rdrawRectangle(x + 11, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);

    // Door handles
    rdrawRectangle(x + 20, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);
    rdrawRectangle(x + 28, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);

    // Bottom trim
    rdrawRectangle(x + 2, y + 2, 46, 1, 0.2f, 0.2f, 0.25f);

    // Wheels
    rdrawCircle(x + 12, y + 2, 6, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 12, y + 2, 4, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 12, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    rdrawCircle(x + 38, y + 2, 6, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 38, y + 2, 4, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 38, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    glLineWidth(1.0f);

    // ─────────────── HEADLIGHTS & GLOW EFFECT ───────────────
   if (isNight) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Very small & sharp beam
    glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
        glVertex2f(x + 47, y + 10);
        glVertex2f(x + 70, y - 8);    // very short left
        glVertex2f(x + 80, y + 28);   // short right rise
    glEnd();

    glDisable(GL_BLEND);
}


    // License plate
    rdrawRectangle(x + 48, y + 2, 1, 3, 1.0f, 1.0f, 1.0f);

    // Antenna
    glColor3f(0.2f, 0.2f, 0.25f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 18, y + 28);
    glVertex2f(x + 18, y + 32);
    glEnd();
    glLineWidth(1.0f);
}
/* CAR FACING LEFT (for opposite direction) */
void rdrawCarLeft(float x, float y, float r, float g, float b) {
    // Car shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 48, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 45, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Lower car body
    rdrawRectangle(x, y, 50, 12, r * 0.85f, g * 0.85f, b * 0.85f);

    // Main car body
    rdrawRectangle(x + 2, y + 10, 46, 10, r, g, b);

    // Hood (now on left)
    rdrawRectangle(x + 2, y + 10, 13, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Trunk (now on right)
    rdrawRectangle(x + 35, y + 10, 10, 8, r * 0.95f, g * 0.95f, b * 0.95f);

    // Car roof (flipped)
    glColor3f(r * 0.75f, g * 0.75f, b * 0.75f);
    glBegin(GL_POLYGON);
    glVertex2f(x + 12, y + 20);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 34, y + 28);
    glVertex2f(x + 14, y + 28);
    glEnd();

    // Front windshield (now on left)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 14, y + 20);
    glVertex2f(x + 10, y + 18);
    glVertex2f(x + 12, y + 26);
    glVertex2f(x + 14, y + 28);
    glEnd();

    // Rear windshield (now on right)
    glBegin(GL_QUADS);
    glVertex2f(x + 36, y + 20);
    glVertex2f(x + 34, y + 28);
    glVertex2f(x + 36, y + 26);
    glVertex2f(x + 40, y + 18);
    glEnd();

    // Side windows
    rdrawRectangle(x + 16, y + 21, 9, 6, 0.3f, 0.3f, 0.35f);
    rdrawRectangle(x + 16.5f, y + 21.5f, 8, 5, 0.2f, 0.3f, 0.5f);
    rdrawRectangle(x + 26, y + 21, 7, 6, 0.3f, 0.3f, 0.35f);
    rdrawRectangle(x + 26.5f, y + 21.5f, 6, 5, 0.2f, 0.3f, 0.5f);

    // Door lines
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 26, y + 4); glVertex2f(x + 26, y + 20);
    glVertex2f(x + 36, y + 4); glVertex2f(x + 36, y + 20);
    glEnd();
    glLineWidth(1.0f);

    // Side mirrors
    rdrawRectangle(x + 11, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);
    rdrawRectangle(x + 36, y + 22, 3, 2, 0.2f, 0.2f, 0.25f);

    // Door handles
    rdrawRectangle(x + 19, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);
    rdrawRectangle(x + 29, y + 12, 3, 1, 0.5f, 0.5f, 0.55f);

    // Bottom trim
    rdrawRectangle(x + 2, y + 2, 46, 1, 0.2f, 0.2f, 0.25f);

    // Wheels
    rdrawCircle(x + 12, y + 2, 6, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 12, y + 2, 4, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 12, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    rdrawCircle(x + 38, y + 2, 6, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 38, y + 2, 4, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 38, y + 2, 2.5f, 0.25f, 0.25f, 0.28f);

    glLineWidth(1.0f);

    // ─────────────── NIGHT MODE HEADLIGHTS (RIGHT-TO-LEFT) ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp beam - mirrored for left-facing car
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);   // Bright beam color
            glVertex2f(x + 3, y + 10);             // headlight center (left side of car)
            glVertex2f(x - 30, y - 8);             // left spread (backward in screen direction)
            glVertex2f(x - 20, y + 28);            // right spread (slightly higher)
        glEnd(); */

        // Very soft outer layer (small size)
       glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 3, y + 10);
            glVertex2f(x - 50, y - 30);
            glVertex2f(x - 45, y + 50);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (optional LED look)
        rdrawCircle(x + 3, y + 10, 3.0f, 1.0f, 1.0f, 0.98f);
    } else {
        // Daytime: normal small lights
        rdrawCircle(x + 3, y + 6,  2.5f, 1.0f, 1.0f, 0.95f);
        rdrawCircle(x + 3, y + 14, 2.5f, 1.0f, 1.0f, 0.95f);
    }

    // Front grille (now on left)
    rdrawRectangle(x + 0.5f, y + 8, 1.5f, 8, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 0.5f, y + 10 + i * 1.5f);
        glVertex2f(x + 2, y + 10 + i * 1.5f);
        glEnd();
    }

    // License plate
   // rdrawRectangle(x + 1, y + 2, 1, 3, 1.0f, 1.0f, 1.0f);

    // Antenna
    /*glColor3f(0.2f, 0.2f, 0.25f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x + 32, y + 28);
    glVertex2f(x + 32, y + 32);
    glEnd();
    glLineWidth(1.0f);  */
}

/* BUS - City Transport */
void rdrawBus(float x, float y, float r, float g, float b) {
    // Bus shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 108, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 105, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Main bus body (larger than car)
    rdrawRectangle(x, y, 110, 30, r, g, b);

    // Lower chassis
    rdrawRectangle(x, y, 110, 8, r * 0.85f, g * 0.85f, b * 0.85f);

    // Roof
    rdrawRectangle(x + 2, y + 30, 106, 2, r * 0.75f, g * 0.75f, b * 0.75f);

    // Front windshield
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 90, y + 30);
    glVertex2f(x + 108, y + 30);
    glVertex2f(x + 108, y + 10);
    glVertex2f(x + 90, y + 10);
    glEnd();

    // Side windows (multiple for bus)
    for (int i = 0; i < 5; i++) {
        rdrawRectangle(x + 10 + i * 15, y + 15, 12, 12, 0.3f, 0.3f, 0.35f);
        rdrawRectangle(x + 11 + i * 15, y + 16, 10, 10, 0.2f, 0.3f, 0.5f);
    }

    // Door
    rdrawRectangle(x + 70, y + 8, 10, 20, r * 0.9f, g * 0.9f, b * 0.9f);
    rdrawRectangle(x + 71, y + 9, 8, 18, 0.2f, 0.3f, 0.5f);

    // Wheels (larger and more spaced out)
    rdrawCircle(x + 20, y + 2, 8, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 20, y + 2, 6, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 20, y + 2, 3, 0.25f, 0.25f, 0.28f);

    rdrawCircle(x + 90, y + 2, 8, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 90, y + 2, 6, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 90, y + 2, 3, 0.25f, 0.25f, 0.28f);

    // ─────────────── NIGHT MODE HEADLIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 108, y + 15);
            glVertex2f(x + 150, y - 5);
            glVertex2f(x + 155, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        rdrawCircle(x + 108, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        rdrawCircle(x + 108, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        rdrawCircle(x + 2, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        rdrawCircle(x + 2, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        rdrawRectangle(x + 0, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        rdrawRectangle(x + 0, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        rdrawCircle(x + 108, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        rdrawCircle(x + 108, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        rdrawRectangle(x + 0, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        rdrawRectangle(x + 0, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille
    rdrawRectangle(x + 108, y + 10, 2, 10, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 108, y + 12 + i * 1.5f);
        glVertex2f(x + 110, y + 12 + i * 1.5f);
        glEnd();
    }

    // License plate
    rdrawRectangle(x + 108, y + 4, 2, 4, 1.0f, 1.0f, 1.0f);
}
/* BUS FACING LEFT */
void rdrawBusLeft(float x, float y, float r, float g, float b) {
    // Bus shadow with gradient
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POLYGON);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 5, y - 2);
    glColor4f(0.0f, 0.0f, 0.0f, 0.1f);
    glVertex2f(x + 2, y - 5);
    glVertex2f(x + 108, y - 5);
    glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
    glVertex2f(x + 105, y - 2);
    glEnd();
    glDisable(GL_BLEND);

    // Main bus body
    rdrawRectangle(x, y, 110, 30, r, g, b);

    // Lower chassis
    rdrawRectangle(x, y, 110, 8, r * 0.85f, g * 0.85f, b * 0.85f);

    // Roof
    rdrawRectangle(x + 2, y + 30, 106, 2, r * 0.75f, g * 0.75f, b * 0.75f);

    // Front windshield (now on left)
    glColor3f(0.25f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 30);
    glVertex2f(x + 20, y + 30);
    glVertex2f(x + 20, y + 10);
    glVertex2f(x + 2, y + 10);
    glEnd();

    // Side windows (multiple for bus)
    for (int i = 0; i < 5; i++) {
        rdrawRectangle(x + 10 + i * 15, y + 15, 12, 12, 0.3f, 0.3f, 0.35f);
        rdrawRectangle(x + 11 + i * 15, y + 16, 10, 10, 0.2f, 0.3f, 0.5f);
    }

    // Door (now on right side for left-facing bus)
    rdrawRectangle(x + 30, y + 8, 10, 20, r * 0.9f, g * 0.9f, b * 0.9f);
    rdrawRectangle(x + 31, y + 9, 8, 18, 0.2f, 0.3f, 0.5f);

    // Wheels
    rdrawCircle(x + 20, y + 2, 8, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 20, y + 2, 6, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 20, y + 2, 3, 0.25f, 0.25f, 0.28f);

    rdrawCircle(x + 90, y + 2, 8, 0.12f, 0.12f, 0.12f);
    rdrawCircle(x + 90, y + 2, 6, 0.45f, 0.45f, 0.48f);
    rdrawCircle(x + 90, y + 2, 3, 0.25f, 0.25f, 0.28f);

    // ─────────────── NIGHT MODE LIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Soft outer layer (small size)
       glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 2, y + 15);
            glVertex2f(x - 50, y - 5);
            glVertex2f(x - 45, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        rdrawCircle(x + 2, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        rdrawCircle(x + 2, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow (now on right side)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        rdrawCircle(x + 108, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        rdrawCircle(x + 108, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        rdrawRectangle(x + 108, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        rdrawRectangle(x + 108, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        rdrawCircle(x + 2, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        rdrawCircle(x + 2, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        rdrawRectangle(x + 108, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        rdrawRectangle(x + 108, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille (now on left side)
    rdrawRectangle(x + 2, y + 10, 2, 10, 0.15f, 0.15f, 0.18f);
    glColor3f(0.25f, 0.25f, 0.28f);
    glLineWidth(1.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 2, y + 12 + i * 1.5f);
        glVertex2f(x + 4, y + 12 + i * 1.5f);
        glEnd();
    }

    // License plate
    rdrawRectangle(x + 2, y + 4, 2, 4, 1.0f, 1.0f, 1.0f);
}

/* TRUCK - Cargo/Delivery */
void rdrawTruck(float x, float y, float r, float g, float b) {
    // Truck shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    rdrawRectangle(x + 3, y - 4, 84, 4, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Cargo container (box)
    rdrawRectangle(x, y, 65, 30, r, g, b);

    // Container details (stripes)
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    glLineWidth(2.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 15 + (i * 15), y);
        glVertex2f(x + 15 + (i * 15), y + 30);
        glEnd();
    }
    glLineWidth(1.0f);

    // Cabin
    rdrawRectangle(x + 65, y, 25, 28, r * 0.9f, g * 0.9f, b * 0.9f);

    // Cabin roof
    rdrawRectangle(x + 68, y + 28, 19, 6, r * 0.7f, g * 0.7f, b * 0.7f);

    // Windshield
    rdrawRectangle(x + 70, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);
    rdrawRectangle(x + 79, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);

    // Side window
    rdrawRectangle(x + 67, y + 20, 2, 8, 0.3f, 0.4f, 0.6f);

    // Wheels
    rdrawCircle(x + 12, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 12, y, 5, 0.3f, 0.3f, 0.3f);
    rdrawCircle(x + 30, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 30, y, 5, 0.3f, 0.3f, 0.3f);
    rdrawCircle(x + 75, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 75, y, 5, 0.3f, 0.3f, 0.3f);

    // ─────────────── NIGHT MODE HEADLIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp headlight beam
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 90, y + 15);           // Headlight center (front)
            glVertex2f(x + 112, y + 5);           // Left spread
            glVertex2f(x + 122, y + 35);          // Right spread (slightly higher)
        glEnd(); */

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 90, y + 15);
            glVertex2f(x + 132, y - 5);
            glVertex2f(x + 137, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        rdrawCircle(x + 90, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        rdrawCircle(x + 90, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        rdrawCircle(x + 2, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        rdrawCircle(x + 2, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        rdrawRectangle(x + 0, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        rdrawRectangle(x + 0, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        rdrawCircle(x + 90, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        rdrawCircle(x + 90, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        rdrawRectangle(x + 0, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        rdrawRectangle(x + 0, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Headlights base (daytime)
    rdrawCircle(x + 90, y + 15, 4, 1.0f, 1.0f, 0.8f);

    // Grill
    rdrawRectangle(x + 88, y + 10, 2, 8, 0.2f, 0.2f, 0.2f);
}

/* TRUCK FACING LEFT */
void rdrawTruckLeft(float x, float y, float r, float g, float b) {
    // Truck shadow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    rdrawRectangle(x + 3, y - 4, 84, 4, 0.0f, 0.0f, 0.0f);
    glDisable(GL_BLEND);

    // Cabin (now on left)
    rdrawRectangle(x, y, 25, 28, r * 0.9f, g * 0.9f, b * 0.9f);

    // Cabin roof
    rdrawRectangle(x + 3, y + 28, 19, 6, r * 0.7f, g * 0.7f, b * 0.7f);

    // Windshield
    rdrawRectangle(x + 5, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);
    rdrawRectangle(x + 14, y + 20, 8, 10, 0.3f, 0.4f, 0.6f);

    // Side window
    rdrawRectangle(x + 22, y + 20, 2, 8, 0.3f, 0.4f, 0.6f);

    // Cargo container
    rdrawRectangle(x + 25, y, 65, 30, r, g, b);

    // Container stripes
    glColor3f(r * 0.7f, g * 0.7f, b * 0.7f);
    glLineWidth(2.0f);
    for (int i = 0; i < 4; i++) {
        glBegin(GL_LINES);
        glVertex2f(x + 35 + (i * 15), y);
        glVertex2f(x + 35 + (i * 15), y + 30);
        glEnd();
    }
    glLineWidth(1.0f);

    // Wheels
    rdrawCircle(x + 15, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 15, y, 5, 0.3f, 0.3f, 0.3f);
    rdrawCircle(x + 48, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 48, y, 5, 0.3f, 0.3f, 0.3f);
    rdrawCircle(x + 66, y, 7, 0.15f, 0.15f, 0.15f);
    rdrawCircle(x + 66, y, 5, 0.3f, 0.3f, 0.3f);

    // ─────────────── NIGHT MODE LIGHTS ───────────────
    if (isNight) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        // Small & sharp headlight beam (mirrored for right-to-left)
       /* glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.98f, 0.9f, 0.55f);
            glVertex2f(x + 2, y + 20);             // Headlight center (front/left)
            glVertex2f(x - 30, y + 5);             // Left spread (backward)
            glVertex2f(x - 20, y + 35);            // Right spread (slightly higher)
        glEnd(); */

        // Soft outer layer (small size)
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.20f);
            glVertex2f(x + 2, y + 15);
            glVertex2f(x - 50, y - 5);
            glVertex2f(x - 45, y + 45);
        glEnd();

        glDisable(GL_BLEND);

        // Tiny bright core (LED look)
        rdrawCircle(x + 2, y + 12, 3.0f, 1.0f, 1.0f, 0.98f);
        rdrawCircle(x + 2, y + 18, 3.0f, 1.0f, 1.0f, 0.98f);

        // Taillights with small red glow (now on right side)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glColor4f(1.0f, 0.35f, 0.25f, 0.5f);
        rdrawCircle(x + 90, y + 12, 7.0f, 1.0f, 0.35f, 0.25f);
        rdrawCircle(x + 90, y + 18, 7.0f, 1.0f, 0.35f, 0.25f);
        glDisable(GL_BLEND);

        rdrawRectangle(x + 90, y + 10, 4, 5, 1.0f, 0.25f, 0.15f);
        rdrawRectangle(x + 90, y + 16, 4, 5, 1.0f, 0.25f, 0.15f);
    } else {
        // Daytime: normal small lights
        rdrawCircle(x + 2, y + 12, 2.5f, 1.0f, 1.0f, 0.95f);
        rdrawCircle(x + 2, y + 18, 2.5f, 1.0f, 1.0f, 0.95f);

        rdrawRectangle(x + 90, y + 10, 2, 3, 0.95f, 0.08f, 0.08f);
        rdrawRectangle(x + 90, y + 16, 2, 3, 0.95f, 0.08f, 0.08f);
    }

    // Front grille (now on left side)
    rdrawRectangle(x + 2, y + 10, 2, 8, 0.2f, 0.2f, 0.2f);
}
/* ================= OPENGL ================= */


void initDhaka() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 400, 0, 400);
    glMatrixMode(GL_MODELVIEW);
}
// Create display lists ONCE during initialization
void createDisplayLists() {
    int baselineY = 432;
    bool originalNightState = isNight;  // Save current state

    // ========== DAY BUILDINGS DISPLAY LIST ==========
    buildingsDisplayList = glGenLists(1);
    glNewList(buildingsDisplayList, GL_COMPILE);

    isNight = false;  // Force day mode for this list

    glViewport(-10, 430, 380, 240);
    rdrawBuilding1();

    glViewport(85, baselineY, 240, 228);
    rdrawPerfectBuilding();

    glViewport(130, 445, 450, 450);
    rdrawBuilding3();

    glViewport(240, baselineY, 450, 350);
    rdrawBuilding4();

    glViewport(330, 420, 270, 280);
    rdrawSchool();

    glViewport(410, 410, 320, 400);
    rdrawHospital();

    glViewport(510, 422, 350, 290);
    rdrawPoliceStation();

    glViewport(630, 425, 320, 350);
    rdrawShoppingMall();

    glViewport(728, 422, 300, 250);
    rdrawRestaurant();

    glViewport(815, 415, 340, 450);
    rdrawApartment();

    glViewport(910, 428, 340, 230);
    rdrawLibrary();

    glViewport(1010, 428, 350, 480);
    rdrawBuilding4();

    glViewport(1055, 450, 350, 280);
    rdrawBuilding3();

    glViewport(1145, 415, 260, 450);
    rdrawModernGlass();

    glViewport(1210, 422, 380, 350);
    rdrawLuxuryHotel();

    glViewport(1325, 420, 300, 360);
    rdrawEcoGreenBuilding();

    glViewport(1410, 405, 350, 390);
    rdrawPoliceStation();

    glEndList();

    // ========== NIGHT BUILDINGS DISPLAY LIST ==========
    buildingsNightDisplayList = glGenLists(1);
    glNewList(buildingsNightDisplayList, GL_COMPILE);

    isNight = true;  // Force night mode for this list

    glViewport(-10, 430, 380, 240);
    rdrawBuilding1();

    glViewport(85, baselineY, 240, 228);
    rdrawPerfectBuilding();

    glViewport(130, 445, 450, 450);
    rdrawBuilding3();

    glViewport(240, baselineY, 450, 350);
    rdrawBuilding4();

    glViewport(330, 420, 270, 280);
    rdrawSchool();

    glViewport(410, 410, 320, 400);
    rdrawHospital();

    glViewport(510, 422, 350, 290);
    rdrawPoliceStation();

    glViewport(630, 425, 320, 350);
    rdrawShoppingMall();

    glViewport(728, 422, 300, 250);
    rdrawRestaurant();

    glViewport(815, 415, 340, 450);
    rdrawApartment();

    glViewport(910, 428, 340, 230);
    rdrawLibrary();

    glViewport(1010, 428, 350, 480);
    rdrawBuilding4();

    glViewport(1055, 450, 350, 280);
    rdrawBuilding3();

    glViewport(1145, 415, 260, 450);
    rdrawModernGlass();

    glViewport(1210, 422, 380, 350);
    rdrawLuxuryHotel();

    glViewport(1325, 420, 300, 360);
    rdrawEcoGreenBuilding();

    glViewport(1410, 405, 350, 390);
    rdrawPoliceStation();

    glEndList();

    // Restore original night state
    isNight = originalNightState;

    // Trees (unchanged)
    treesDisplayList = glGenLists(1);
    glNewList(treesDisplayList, GL_COMPILE);
    for (int x = 100; x < 1600; x += 300) {
        rdrawPineTree(x - 40, 400);
        rdrawPineTree(x + 35, 400);
    }
    glEndList();

    displayListsCreated = true;
}


void displayDhaka() {


    // Inside the day sky drawing part (after clouds preferably

    rdrawRoadAndFootpath();


    int baselineY = 420;


    // Create display lists on first run
    if (!displayListsCreated) {
        createDisplayLists();
    }

    // Choose correct building display list based on day/night
    if (isNight) {
        glCallList(buildingsNightDisplayList);
    } else {
        glCallList(buildingsDisplayList);
    }


  //  glCallList(buildingsDisplayList);

   /* DRAW ALL BUILDINGS */
  /*  glViewport(-25, 430, 180, 140);
    drawBuilding1();

    glViewport(85, baselineY, 140, 228);
    drawPerfectBuilding();

    glViewport(175,445, 150, 350);
    drawBuilding3();

    glViewport(272, baselineY, 150, 350);
    drawBuilding4();

    glViewport(333, baselineY, 170, 180);
    drawSchool();

    glViewport(466 ,400, 140, 325);
    drawHospital();

    glViewport(575, baselineY, 150, 190);
    drawPoliceStation();

    glViewport(695, baselineY, 110, 250);
    drawShoppingMall();

    glViewport(782, baselineY, 100, 150);
    drawRestaurant();

    glViewport(851, 400, 140, 350);
    drawApartment();

    glViewport(942, 410, 140, 230);
    drawLibrary();

    glViewport(1042, baselineY, 150, 380);
    drawBuilding4();

    glViewport(1070, 450, 200, 150);
    drawBuilding3();

    glViewport(1183,402, 160, 350);
    drawModernGlass();

    glViewport(1288,420, 180, 250);
    drawLuxuryHotel();

    glViewport(1425,420, 130, 260);
    drawEcoGreenBuilding();



    /* DRAW PEOPLE AND STREET FURNITURE */
    glViewport(0, 0, 1600, 900);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 900);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* PEOPLE WALKING RIGHT (left to right) */
    rdrawWalkingPerson(personX, 410, 0.8f, 0.2f, 0.2f);        // Red shirt
    rdrawWalkingPerson(personX + 300, 410, 0.2f, 0.4f, 0.8f);  // Blue shirt
    rdrawWalkingPerson(personX + 600, 410, 0.3f, 0.7f, 0.3f);  // Green shirt
    rdrawWalkingPerson(personX + 900, 410, 0.9f, 0.7f, 0.1f);  // Yellow shirt

    /* PEOPLE WALKING LEFT (right to left) */
    rdrawWalkingPersonLeft(personX2, 415, 0.9f, 0.4f, 0.6f);        // Pink shirt
    rdrawWalkingPersonLeft(personX2 - 350, 415, 0.5f, 0.2f, 0.7f);  // Purple shirt
    rdrawWalkingPersonLeft(personX2 - 700, 415, 0.2f, 0.8f, 0.8f);  // Cyan shirt
    rdrawWalkingPersonLeft(personX2 - 1050, 415, 1.0f, 0.5f, 0.0f); // Orange shirt


     // ROAD 1 (Main Road - y = 340) - Upper Lane (moving RIGHT)
rdrawCar(car1X, 360, 0.9f, 0.1f, 0.1f);        // Red car
rdrawBus(bus1X, 360, 0.2f, 0.6f, 0.9f);        // Blue bus
rdrawTruck(truck1X, 360, 0.3f, 0.7f, 0.3f);    // Green truck






    // === SCALED VEHICLES START HERE ===

    // ROAD 1 - Upper Lane (moving RIGHT - y ≈ 360)
    glPushMatrix();
    glTranslatef(car1X + 25, 360 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCar(0, 0, 0.9f, 0.1f, 0.1f);  // Red car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus1X + 50, 360 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    rdrawBus(0, 0, 0.2f, 0.6f, 0.9f);  // Blue bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck1X + 45, 360 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    rdrawTruck(0, 0, 0.3f, 0.7f, 0.3f);  // Green truck
    glPopMatrix();

    // ROAD 1 - Lower Lane (moving LEFT - y ≈ 320)
    glPushMatrix();
    glTranslatef(car2X + 25, 320 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCarLeft(0, 0, 1.0f, 0.8f, 0.0f);  // Yellow car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus2X + 50, 320 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    rdrawBusLeft(0, 0, 0.9f, 0.5f, 0.2f);  // Orange bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck2X + 45, 320 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    rdrawTruckLeft(0, 0, 0.6f, 0.3f, 0.8f);  // Purple truck
    glPopMatrix();

    // ROAD 2 - Upper Lane (moving RIGHT - y ≈ 230)
    glPushMatrix();
    glTranslatef(car3X + 25, 230 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCar(0, 0, 0.2f, 0.8f, 0.9f);  // Cyan car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(truck3X + 45, 230 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    rdrawTruck(0, 0, 0.9f, 0.9f, 0.2f);  // Yellow truck
    glPopMatrix();

    // ROAD 2 - Lower Lane (moving LEFT - y ≈ 165)
    glPushMatrix();
    glTranslatef(bus3X + 50, 165 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    rdrawBusLeft(0, 0, 0.8f, 0.2f, 0.4f);  // Pink bus
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car4X + 25, 165 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCarLeft(0, 0, 0.4f, 0.9f, 0.4f);  // Light green car
    glPopMatrix();

    // ROAD 3 - Upper Lane (moving RIGHT - y ≈ 140)
    glPushMatrix();
    glTranslatef(car5X + 25, 140 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCar(0, 0, 1.0f, 0.6f, 0.0f);  // Orange car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus4X + 50, 140 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    rdrawBus(0, 0, 0.5f, 0.3f, 0.8f);  // Purple bus
    glPopMatrix();

    // ROAD 3 - Lower Lane (moving LEFT - y ≈ 70-65)
    glPushMatrix();
    glTranslatef(truck4X + 45, 70 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    rdrawTruckLeft(0, 0, 0.2f, 0.5f, 0.7f);  // Blue truck
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car6X + 25, 65 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCarLeft(0, 0, 0.9f, 0.3f, 0.6f);  // Pink car
    glPopMatrix();

    // === SCALED VEHICLES END HERE ===




 rdrawSecondRoad();

   rdrawMetroRail();
    rdrawThirdRoad();




    rdrawWalkingPerson(personX_road2, 30, 0.6f, 0.3f, 1.0f);
   rdrawWalkingPerson(personX_road2 + 400, 30, 0.9f, 0.6f, 0.2f);
   rdrawWalkingPersonLeft(personX2_road2, 30, 0.3f, 0.8f, 0.6f);
   rdrawWalkingPersonLeft(personX2_road2 - 450, 30, 0.8f, 0.2f, 0.4f);



    // ROAD 3 - Upper Lane (moving RIGHT - y ≈ 140)
    glPushMatrix();
    glTranslatef(car5X + 25, 140 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCar(0, 0, 1.0f, 0.6f, 0.0f);  // Orange car
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bus4X + 50, 140 + 17.5, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-50, -17.5, 0);
    rdrawBus(0, 0, 0.5f, 0.3f, 0.8f);  // Purple bus
    glPopMatrix();

    // ROAD 3 - Lower Lane (moving LEFT - y ≈ 70 for truck, 65 for car)
    glPushMatrix();
    glTranslatef(truck4X + 45, 70 + 15 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-45, -15, 0);
    rdrawTruckLeft(0, 0, 0.2f, 0.5f, 0.7f);  // Blue truck
    glPopMatrix();

    glPushMatrix();
    glTranslatef(car6X + 25, 65 + 15 + 15, 0);
    glScalef(vehicleScale, vehicleScale, 1.0f);
    glTranslatef(-25, -15, 0);
    rdrawCarLeft(0, 0, 0.9f, 0.3f, 0.6f);  // Pink car
    glPopMatrix();


    /* STREET LIGHTS AND TREES (in front) */
    for (int x = 100; x < 1600; x += 300) {
    rdrawStreetLight(x, 400);
    rdrawPineTree(x - 40, 400);
    rdrawPineTree(x + 35, 400);
}
  // After sky gradient and clouds...
rdrawRain();   // ← Add this line


    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    //glutSwapBuffers();
}
void rcleanup() {
    if (displayListsCreated) {
        glDeleteLists(buildingsDisplayList, 1);
        glDeleteLists(buildingsNightDisplayList, 1);
        glDeleteLists(treesDisplayList, 1);
    }
}
void rtimer(int value) {
    // People walking RIGHT (left to right)
    personX += 1.2;  // Speed of walking right

    // People walking LEFT (right to left)
    personX2 -= 0.5;  // Speed of walking left (slightly slower for variety)

    // Animate legs
    legAnimation += 0.08;

    // Reset position when people go off screen
    if (personX > 1650) {
        personX = -50;  // Reset to left side
    }

    if (personX2 < -50) {
        personX2 = 1650;  // Reset to right side
    }

    personX_road2 += 1.5;
   personX2_road2 -= 1.5;

   if (personX_road2 > 1650) {
       personX_road2 = -50;
   }
   if (personX2_road2 < -50) {
       personX2_road2 = 1650;
   }
       // Metro train logic with station stop
    if (trainState == 0) { // Normal moving
        metroTrainX += 4;
        // Check if entering station
        if (metroTrainX >= STATION_ENTER && metroTrainX < STATION_ENTER + 20) {
            trainState = 1; // Start stopping
        }
    } else if (trainState == 1) { // Decelerate and stop
        metroTrainX += 2; // Slow down gradually
        if (metroTrainX >= STATION_STOP_POS) {
            metroTrainX = STATION_STOP_POS; // Lock position
            trainState = 2;
            stopTimer = 0.0f;
        }
    } else if (trainState == 2) { // Stopped at station
        stopTimer += 0.03f; // ~30ms per frame
        if (stopTimer >= 3.0f) { // Waited 3 seconds
            trainState = 0; // Resume normal movement
        }
    }

    // Loop back when leaving screen
    if (metroTrainX > 1650) {
        metroTrainX = -320;
        trainState = 0; // Reset state for next lap
        stopTimer = 0.0f;
    }
// Road 1 - Moving RIGHT (speed: 4-5)
car1X += 3.8;
bus1X += 2.8;
truck1X += 2.5;

// Road 1 - Moving LEFT
car2X -= 3.8;
bus2X -= 2.8;
truck2X -= 2.5;

// Road 2 - Moving RIGHT
car3X += 4.5;
truck3X += 3;

// Road 2 - Moving LEFT
bus3X -= 3.5;
car4X -= 4;

// Road 3 - Moving RIGHT
car5X += 3.8;
bus4X += 3;

// Road 3 - Moving LEFT
truck4X -= 2.5;
car6X -= 3.5;

/* ========== RESET VEHICLES WHEN OFF-SCREEN ========== */

// Road 1 RIGHT-moving vehicles
if (car1X > 1650) car1X = -60;
if (bus1X > 1650) bus1X = -110;
if (truck1X > 1650) truck1X = -100;

// Road 1 LEFT-moving vehicles
if (car2X < -60) car2X = 1650;
if (bus2X < -110) bus2X = 1650;
if (truck2X < -100) truck2X = 1650;

// Road 2 RIGHT-moving vehicles
if (car3X > 1650) car3X = -60;
if (truck3X > 1650) truck3X = -100;

// Road 2 LEFT-moving vehicles
if (bus3X < -110) bus3X = 1650;
if (car4X < -60) car4X = 1650;

// Road 3 RIGHT-moving vehicles
if (car5X > 1650) car5X = -60;
if (bus4X > 1650) bus4X = -110;

// Road 3 LEFT-moving vehicles
if (truck4X < -100) truck4X = 1650;
if (car6X < -60) car6X = 1650;

    glutPostRedisplay();
    glutTimerFunc(30, rtimer, 0);
    // Move clouds slowly from left to right
cloudOffset += 0.8f;  // Adjust speed: smaller = slower, larger = faster

// Optional: reset if gets too large (prevents floating point issues)
// Clouds - only move during day
// Clouds - only move during day

nightTimer += 0.016f;  // ~1/60 per frame
    if (!isNight) {
        cloudOffset += 0.4f;          // adjust speed if needed
        if (cloudOffset > 2000) {
            cloudOffset -= 2000;
        }
    }

// Day/Night cycle

if (nightTimer >= DAY_NIGHT_CYCLE) {
    nightTimer = 0.0f;
    isNight = !isNight;  // Toggle between day and night
}
if (isRaining) {
        rainOffset += 1.0f;           // controls falling speed
        if (rainOffset > 100) rainOffset -= 100;
    }
}



/*===========================Partho=============================
                             partho

================================================================*/




// Animation Variables
GLfloat cloud_x1=0, cloud_x2=0, cloud_x3=0, cloud_x4=0;
GLfloat boat_x = 0;
GLfloat wave_offset = 0;
GLfloat bird_x1 = 300, bird_y1 = 400;
GLfloat bird_x2 = 500, bird_y2 = 420;
GLfloat sun_x = 350, sun_y = 500;
GLfloat animation_speed = 1.0;
bool animate = false;
bool isDayMode = true;
const double PI = 3.141592653589793;

// Rain Variables
//bool isRaining = false;
//bool rainSoundPlaying = false;
//GLfloat rainOffset = 0;
//GLfloat rainSpeed = 2.0;
//GLfloat rainDensity = 0.5;

void initBeach()
{
    glClearColor(0.529, 0.808, 0.922, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 1000.0, 0.0, 600.0);
}

///============================================================================================================///
///                                          UTILITY FUNCTIONS
///============================================================================================================///

void circle(GLdouble rad)
{
    GLint points = 50;
    GLdouble delTheta = (2.0 * PI) / (GLdouble)points;
    GLdouble theta = 0.0;

    glBegin(GL_POLYGON);
    {
        for(int i = 0; i <=50; i++, theta += delTheta )
        {
            glVertex2f(rad * cos(theta),rad * sin(theta));
        }
    }
    glEnd();
}


///============================================================================================================///
///                                          INDIVIDUAL OBJECT FUNCTIONS
///============================================================================================================///

///*** Sky Background ***///
void pdrawSky(){
    glBegin(GL_POLYGON);
    if(isDayMode){
        glColor3f(0.35, 0.65, 0.95);
        glVertex2f(0, 600);
        glVertex2f(1000, 600);
        glColor3f(0.75, 0.88, 0.98);
        glVertex2f(1000, 300);
        glVertex2f(0, 300);
    } else {
        glColor3f(0.05, 0.05, 0.2);
        glVertex2f(0, 600);
        glVertex2f(1000, 600);
        glColor3f(0.1, 0.1, 0.3);
        glVertex2f(1000, 300);
        glVertex2f(0, 300);
    }
    glEnd();
}

///*** First Hill (Leftmost, Darker) ***///
void pdrawHill1(){
    glBegin(GL_POLYGON);
    glColor3f(0.25, 0.45, 0.35);
    glVertex2f(0, 340);
    glVertex2f(80, 380);
    glVertex2f(140, 400);
    glVertex2f(200, 390);
    glVertex2f(260, 370);
    glVertex2f(320, 340);
    glEnd();
}

///*** Second Hill (Middle-Left, Medium) ***///
void pdrawHill2(){
    glBegin(GL_POLYGON);
    glColor3f(0.30, 0.52, 0.40);
    glVertex2f(280, 340);
    glVertex2f(350, 350);
    glVertex2f(400, 390);
    glVertex2f(480, 410);
    glVertex2f(540, 400);
    glVertex2f(600, 365);
    glVertex2f(640, 340);
    glEnd();
}

///*** Third Hill (Center-Right, Tallest) ***///
void pdrawHill3(){
    glBegin(GL_POLYGON);
    glColor3f(0.28, 0.48, 0.38);
    glVertex2f(620, 340);
    glVertex2f(640, 350);
    glVertex2f(700, 390);
    glVertex2f(750, 410);
    glVertex2f(800, 400);
    glVertex2f(920, 340);
    glEnd();
}

///*** Fourth Hill (Rightmost, Lighter) ***///
void pdrawHill4(){
    glBegin(GL_POLYGON);
    glColor3f(0.35, 0.58, 0.45);
    glVertex2f(900, 340);
    glVertex2f(960, 400);
    glVertex2f(1000, 380);
    glVertex2f(1100, 350);
    glVertex2f(1300, 340);
    glEnd();
}

///*** Hill Highlight ***///
void pdrawHillHighlight(){
    glBegin(GL_POLYGON);
    glColor3f(0.32, 0.55, 0.45);
    glVertex2f(700, 360);
    glVertex2f(730, 380);
    glVertex2f(750, 390);
    glVertex2f(770, 385);
    glVertex2f(740, 365);
    glEnd();
}

///*** Soil Line Below Mountains ***///
void pdrawSoilLine(){
    glColor3f(0.72, 0.58, 0.36);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(0, 340);
    glVertex2f(1000, 340);
    glEnd();
}

///*** Ocean (Gradient) - Full coverage from hills to bottom ***///
void pdrawOcean(){
    glBegin(GL_POLYGON);
    if(isDayMode){
        glColor3f(0.2, 0.5, 0.7);
        glVertex2f(0, 340);
        glVertex2f(1000, 340);
        glColor3f(0.0, 0.35, 0.55);
        glVertex2f(1000, 0);
        glVertex2f(0, 0);
    } else {
        // Darker ocean at night
        glColor3f(0.12, 0.32, 0.48);
        glVertex2f(0, 340);
        glVertex2f(1000, 340);
        glColor3f(0.0, 0.08, 0.15);
        glVertex2f(1000, 0);
        glVertex2f(0, 0);
    }
    glEnd();
}

///*** Beach Sand - Semi-transparent overlay ***///
void pdrawBeach(){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_POLYGON);
    if(isDayMode){
        glColor4f(0.95, 0.90, 0.65, 0.85);
        glVertex2f(0, 140);
        glVertex2f(1000, 140);
        glColor4f(0.88, 0.82, 0.55, 0.85);
        glVertex2f(1000, 0);
        glVertex2f(0, 0);
    } else {
        // Dark beach at night
        glColor4f(0.35, 0.32, 0.25, 0.85);
        glVertex2f(0, 140);
        glVertex2f(1000, 140);
        glColor4f(0.25, 0.22, 0.18, 0.85);
        glVertex2f(1000, 0);
        glVertex2f(0, 0);
    }
    glEnd();

    glDisable(GL_BLEND);
}

///*** Sun Outer Glow ***///
void pdrawSunGlow(){
    glColor4f(1.0, 0.9, 0.3, 0.3);
    glPushMatrix();
    circle(65);
    glPopMatrix();

    glColor4f(1.0, 0.85, 0.2, 0.5);
    glPushMatrix();
    circle(52);
    glPopMatrix();
}

///*** Sun Body ***///
void pdrawSunBody(){
    glColor3f(1.0, 0.95, 0.2);
    glPushMatrix();
    circle(40);
    glPopMatrix();
}

///*** Sun Rays ***///
void pdrawSunRays(){
    glColor3f(1.0, 0.85, 0.0);
    for(int r=0; r<16; r++){
        glPushMatrix();
        glRotatef(r*22.5, 0, 0, 1);
        glTranslatef(50, 0, 0);
        glBegin(GL_TRIANGLES);
        glVertex2f(-8, -3);
        glVertex2f(25, 0);
        glVertex2f(-8, 3);
        glEnd();
        glPopMatrix();
    }
}

///*** Complete Sun Model ***///
void pdrawSun(){
    pdrawSunGlow();
    pdrawSunBody();
    pdrawSunRays();
}

///*** Moon ***///
void pdrawMoon(){
    // Moon body (white/light gray)
    glColor3f(0.95, 0.95, 0.98);
    glPushMatrix();
    circle(40);
    glPopMatrix();

    // Moon craters (darker spots)
    glColor3f(0.75, 0.75, 0.78);
    glPushMatrix();
    glTranslatef(15, 10, 0);
    circle(8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-10, 5, 0);
    circle(6);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-5, -15, 0);
    circle(7);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8, -8, 0);
    circle(5);
    glPopMatrix();
}

///*** Cloud ***///
void pdrawCloud(){
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();
    glTranslatef(-15,5,0);
    circle(18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,0);
    circle(22);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,8,0);
    circle(25);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(40,5,0);
    circle(20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(55,0,0);
    circle(18);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(20,-8,0);
    circle(20);
    glPopMatrix();
}

///*** Boat Hull ***///
void pdrawBoatHull(){
    glBegin(GL_POLYGON);
    glColor3f(0.08, 0.15, 0.25);
    glVertex2f(-80, 0);
    glVertex2f(80, 0);
    glVertex2f(60, -35);
    glVertex2f(-60, -35);
    glEnd();
}

///*** Boat White Stripe ***///
void pdrawBoatStripe(){
    glBegin(GL_POLYGON);
    glColor3f(0.95, 0.97, 1.0);
    glVertex2f(-75, -5);
    glVertex2f(75, -5);
    glVertex2f(55, -30);
    glVertex2f(-55, -30);
    glEnd();
}

///*** Boat Left Sail ***///
void pdrawBoatLeftSail(){
    glBegin(GL_TRIANGLES);
    glColor4f(0.98, 0.98, 1.0, 0.9);
    glVertex2f(0, 100);
    glVertex2f(-70, 10);
    glVertex2f(0, 10);
    glEnd();
}

///*** Boat Right Sail ***///
void pdrawBoatRightSail(){
    glBegin(GL_TRIANGLES);
    glColor4f(0.92, 0.96, 1.0, 0.85);
    glVertex2f(0, 100);
    glVertex2f(70, 10);
    glVertex2f(0, 10);
    glEnd();
}

///*** Boat Navigation Light ***///
void pdrawBoatLight(){
    glColor3f(0.9, 0.1, 0.1);
    glPushMatrix();
    glTranslatef(0, 100, 0);
    circle(4);
    glPopMatrix();
}

///*** Complete Boat Model ***///
void pdrawBoat(){
    pdrawBoatHull();
    pdrawBoatStripe();
    pdrawBoatLeftSail();
    pdrawBoatRightSail();
    pdrawBoatLight();
}

///*** Palm Tree Trunk ***///
void pdrawPalmTrunk(){
    for(int seg=0; seg<10; seg++){
        glBegin(GL_POLYGON);
        if(seg % 2 == 0)
            glColor3f(0.5, 0.3, 0.1);
        else
            glColor3f(0.45, 0.25, 0.08);

        float y1 = seg * 11;
        float y2 = (seg + 1) * 11;
        glVertex2f(-12 + seg*0.5, y1);
        glVertex2f(12 - seg*0.5, y1);
        glVertex2f(10 - (seg+1)*0.5, y2);
        glVertex2f(-10 + (seg+1)*0.5, y2);
        glEnd();
    }
}

///*** Palm Coconuts ***///
void pdrawPalmCoconuts(){
    glColor3f(0.35, 0.2, 0.05);
    for(int c=0; c<5; c++){
        glPushMatrix();
        glTranslatef(-8 + c*4, 105, 0);
        circle(5);
        glPopMatrix();
    }
}

///*** Palm Leaves ***///
void pdrawPalmLeaves(){
    for(int l=0; l<8; l++){
        glPushMatrix();
        glTranslatef(0, 110, 0);
        glRotatef(l*45, 0, 0, 1);

        glBegin(GL_POLYGON);
        glColor3f(0.1, 0.5, 0.1);
        glVertex2f(0, 0);
        glVertex2f(8, 15);
        glColor3f(0.2, 0.7, 0.2);
        glVertex2f(45, 60);
        glVertex2f(15, 25);
        glEnd();

        glBegin(GL_POLYGON);
        glColor3f(0.15, 0.6, 0.15);
        glVertex2f(0, 0);
        glVertex2f(-6, 20);
        glColor3f(0.25, 0.75, 0.25);
        glVertex2f(-35, 50);
        glVertex2f(-10, 20);
        glEnd();

        glPopMatrix();
    }
}

///*** Complete Palm Tree ***///
void pdrawPalmTree(){
    pdrawPalmTrunk();
    pdrawPalmCoconuts();
    pdrawPalmLeaves();
}

///*** Bird (Seagull) ***///
void pdrawBird(){
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(2);

    glBegin(GL_LINE_STRIP);
    glVertex2f(-8, 2);
    glVertex2f(0, 0);
    glVertex2f(8, 2);
    glEnd();

    glPushMatrix();
    circle(1.5);
    glPopMatrix();
}

///*** Starfish Arms ***///
void pdrawStarfishArms(){
    for(int arm=0; arm<5; arm++){
        glPushMatrix();
        glRotatef(arm*72, 0, 0, 1);

        glBegin(GL_TRIANGLES);
        glVertex2f(-3, 0);
        glVertex2f(3, 0);
        glVertex2f(0, 15);
        glEnd();

        glPopMatrix();
    }
}

///*** Starfish Center ***///
void pdrawStarfishCenter(){
    glColor3f(0.9, 0.4, 0.2);
    circle(4);
}

///*** Complete Starfish ***///
void pdrawStarfish(){
    glColor3f(1.0, 0.5, 0.3);
    pdrawStarfishArms();
    pdrawStarfishCenter();
}

///*** Rain Effect ***///
void pdrawRain() {
    if (!isRaining) {
        // Stop rain sound when rain stops
        if (rainSoundPlaying) {
           //sndPlaySound("waves.wav", SND_ASYNC | SND_LOOP);  // Play waves sound back
            rainSoundPlaying = false;
        }
        return;
    }
    // Start rain sound when rain begins
    if (!rainSoundPlaying) {
       // sndPlaySound("rain.wav", SND_ASYNC | SND_LOOP);
        rainSoundPlaying = true;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // === 1. Natural rain opacity ===
    float baseAlpha = 0.12f + rainDensity * 0.28f;
    // === 2. High drop count for realistic density ===
    int rainCount = (int)(3500 * rainDensity);
    for (int i = 0; i < rainCount; i++) {
        // === 3. Consistent random distribution per drop ===
        unsigned int seed = i * 48271 + (int)(rainOffset * 50);
        float x = (float)((seed * 2654435761u) % 2000) - 100.0f;
        float startY = (float)((seed * 1103515245u) % 1400) - 200.0f;
        // === 4. Different fall speeds create depth illusion ===
        float depthSpeed = 0.7f + (float)((seed * 196314165u) % 100) / 125.0f;
        float animY = fmod(startY + rainOffset * rainSpeed * depthSpeed, 1500.0f) - 200.0f;
        // === 5. Varying drop lengths (realistic motion blur) ===
        float length = 35.0f + (float)((seed * 48271u) % 90);
        // === 6. STRAIGHT DOWN - purely vertical ===
        float x1 = x;
        float y1 = animY;
        float x2 = x;
        float y2 = y1 - length;
        // === 7. Depth-based rendering ===
        float depthFactor = 0.4f + (float)((seed * 1664525u) % 100) / 166.0f;
        float alpha = baseAlpha * depthFactor;
        float thickness = 0.6f + depthFactor * 1.6f;
        glLineWidth(thickness);
        // === 8. Realistic rain color ===
        glColor4f(0.82f + depthFactor * 0.16f,
                  0.88f + depthFactor * 0.10f,
                  0.96f + depthFactor * 0.04f,
                  alpha);
        glBegin(GL_LINES);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
        glEnd();
    }
    glDisable(GL_BLEND);
}

///============================================================================================================///
///                                          SCENE COMPOSITION
///============================================================================================================///

void pdisplay()
{


    /// Background Elements
    pdrawSky();
    pdrawHill1();
    pdrawHill2();
    pdrawHill3();
    pdrawHill4();
    pdrawHillHighlight();
    pdrawSoilLine();
    pdrawOcean();
    pdrawBeach();

    /// Sun or Moon (depending on day/night mode)
    glPushMatrix();
    glTranslatef(sun_x, sun_y, 0);
    if(isDayMode){
        pdrawSun();
    } else {
        pdrawMoon();
    }
    glPopMatrix();

    /// Birds
    glPushMatrix();
    glTranslatef(bird_x1, bird_y1, 0);
    glScalef(1.5, 1.5, 1);
    pdrawBird();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(bird_x2, bird_y2, 0);
    glScalef(1.2, 1.2, 1);
    pdrawBird();
    glPopMatrix();

    /// Clouds (only show in day mode)
    if(isDayMode){
        glPushMatrix();
        glTranslatef(cloud_x1 + 150, 500, 0);
        glScalef(1.2, 1.2, 1);
        pdrawCloud();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(cloud_x2 + 450, 470, 0);
        glScalef(0.9, 0.9, 1);
        pdrawCloud();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(cloud_x4 + 50, 480, 0);
        pdrawCloud();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(cloud_x3 + 750, 520, 0);
        pdrawCloud();
        glPopMatrix();
    }

    /// Boat
    glPushMatrix();
    glTranslatef(boat_x + 600, 230, 0);
    glScalef(0.9, 0.9, 1);
    pdrawBoat();
    glPopMatrix();

    /// Palm Trees
    glPushMatrix();
    glTranslatef(120, 100, 0);
    glScalef(1.3, 1.3, 1);
    pdrawPalmTree();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(880, 90, 0);
    glScalef(1.3, 1.3, 1);
    pdrawPalmTree();
    glPopMatrix();

    /// Starfish
    glPushMatrix();
    glTranslatef(250, 40, 0);
    glScalef(1.2, 1.2, 1);
    glRotatef(25, 0, 0, 1);
    pdrawStarfish();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(650, 35, 0);
    glRotatef(-15, 0, 0, 1);
    pdrawStarfish();
    glPopMatrix();

    /// Rain Effect (draw on top of everything)
    pdrawRain();

   // glFlush();
}


///========================///
///*** Animation ***///
///========================///

void ptimer(){
    // Beach animation update
    if(animate){
        boat_x -= 0.90 * animation_speed;
        if(boat_x < -800) boat_x = 600;

        cloud_x1 += 0.05 * animation_speed;
        cloud_x2 += 0.01 * animation_speed;
        cloud_x3 += 0.06 * animation_speed;
        cloud_x4 += 0.01 * animation_speed;

        if(cloud_x1 > 1100) cloud_x1 = -250;
        if(cloud_x2 > 1100) cloud_x2 = -300;
        if(cloud_x3 > 1100) cloud_x3 = -350;
        if(cloud_x4 > 1100) cloud_x4 = -400;

        bird_x1 += 0.9 * animation_speed;
        bird_y1 += 0.09 * sin(bird_x1 * 0.05);
        if(bird_x1 > 1100) {
            bird_x1 = -50;
            bird_y1 = 400 + (rand() % 50);
        }

        bird_x2 += 0.15 * animation_speed;
        bird_y2 += 0.04 * sin(bird_x2 * 0.04);
        if(bird_x2 > 1100) {
            bird_x2 = -30;
            bird_y2 = 420 + (rand() % 40);
        }
    }

    // Rain animation (runs independently)
    if(isRaining){
        rainOffset += 1.0;
    }
}

///*** Keyboard Controls ***///
void pkeyboard(unsigned char key, int x, int y){
    switch(key){
        case 27: // ESC key
            exit(0);
            break;
        case ' ': // Space - Pause/Resume
            animate = !animate;
            break;
        case 'd':
        case 'D': // Day mode
            isDayMode = true;
            glutPostRedisplay();
            break;
        case 'n':
        case 'N': // Night mode
            isDayMode = false;
            glutPostRedisplay();
            break;
        case 'r':
        case 'R': // Toggle Rain
            isRaining = !isRaining;
            glutPostRedisplay();
            break;
        case 'm':
        case '=': // Speed up
            animation_speed += 1.2;
            if(animation_speed > 3.0) animation_speed = 3.0;
            break;
        case '-':
        case '_': // Slow down
            animation_speed -= 0.2;
            if(animation_speed < 0.2) animation_speed = 0.2;
            break;
        case 's':
        case 'S': // Reset
            boat_x = 0;
            cloud_x1 = 0;
            cloud_x2 = 0;
            cloud_x3 = 0;
            cloud_x4 = 0;
            sun_x = 350;
            sun_y = 500;
            animation_speed = 1.0;
            isRaining = false;
            rainOffset = 0;
            glutPostRedisplay();
            break;
    }
}

///*** Special Keys (Arrow Keys) ***///
void pspecialKeys(int key, int x, int y){
    switch(key){
        case GLUT_KEY_LEFT:
            sun_x -= 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_RIGHT:
            sun_x += 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            sun_y += 10;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            sun_y -= 10;
            glutPostRedisplay();
            break;
    }
}
void pmouse(int key, int state, int x, int y){
    if (key == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        animate = true;
        glutPostRedisplay();  // Force redraw
    }
    if (key == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        animate = false;
    }
}


/* ==============================abir=======================================*/
void drawCircle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= num_segments; i++)
    {
        float theta = 2.0f * 3.1415926f * i / num_segments;
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}
void displayVillageScene3()
{
    //glClear(GL_COLOR_BUFFER_BIT);
    // SKY

    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.8f, 1.0f);  // Light blue at top
    glVertex2f(0, 440);
    glVertex2f(1200, 440);
    glColor3f(0.2f, 0.4f, 0.8f);  // Dark blue at bottom
    glVertex2f(1200, 750);
    glVertex2f(0, 750);
    glEnd();

    // SUN
    glColor3f(1.0f, 1.0f, 0.0f);
    drawCircle(1050, 650, 45, 80);

    // CLOUDS
    glPushMatrix();
    glTranslatef(cloudPosX, 0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawCircle(225, 625, 42, 70);
    drawCircle(270, 625, 56, 70);
    drawCircle(330, 625, 42, 70);


    drawCircle(150, 550, 30, 70);
    drawCircle(190, 550, 40, 70);
    drawCircle(230, 550, 30, 70);


    drawCircle(750, 700, 35, 70);
    drawCircle(790, 700, 45, 70);
    drawCircle(840, 700, 35, 70);


    drawCircle(900, 650, 35, 70);
    drawCircle(945, 650, 50, 70);
    drawCircle(1005, 650, 35, 70);

    glPopMatrix();


    // FIELD

    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.8f, 0.5f);
    glVertex2f(0, 187);//187
    glVertex2f(1200, 187);
    glColor3f(0.3f, 0.6f, 0.3f);  // Medium green
    glVertex2f(1200, 440);//375
    glVertex2f(0, 440);
    glEnd();


    // BUSHES (behind fence)
    glColor3f(0.0f, 0.5f, 0.0f);  // dark green
    drawCircle(920, 460, 35, 50);
    drawCircle(960, 470, 60, 50);
    drawCircle(1000, 480, 55, 50);
    drawCircle(1040, 470, 60, 50);
    drawCircle(1080, 465, 55, 50);
    drawCircle(1120, 470, 60, 50);
    drawCircle(1160, 480, 55, 50);

    glColor3f(0.2f, 0.4f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex2f(1200,400);
    glVertex2f(1200,440);
    glVertex2f(900,440);
    glVertex2f(900,400);
    glEnd();

    // FENCE (right side)

    glColor3f(0.6f, 0.3f, 0.0f); // brown color
    glLineWidth(6.0f);           // thick fence rails
    glBegin(GL_LINES);
    // Horizontal rails
    glVertex2f(900, 400);
    glVertex2f(1200, 400);
    glVertex2f(900, 440);
    glVertex2f(1200, 440);
    glVertex2f(900, 460);
    glVertex2f(1200, 460);
    // Vertical posts
    for (int x = 900; x <= 1200; x += 50)
    {
        glVertex2f(x, 400);
        glVertex2f(x, 460);
    }
    glEnd();



    // HAY inside the cart

    glColor3f(0.93f, 0.79f, 0.25f); // golden yellow hay color
    glBegin(GL_POLYGON);
    glVertex2f(885, 420); // left top near cart
    glVertex2f(910, 450);
    glVertex2f(940, 460);
    glVertex2f(970, 455);
    glVertex2f(1000, 470);
    glVertex2f(1040, 460);
    glVertex2f(1080, 465);
    glVertex2f(1110, 450);
    glVertex2f(1145, 420); // right end of cart
    glVertex2f(880, 420);  // close back to start
    glEnd();

// Optional: hay edges (lines to look like straws sticking out)
    glColor3f(0.8f, 0.6f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(900, 450);
    glVertex2f(890, 470);
    glVertex2f(950, 460);
    glVertex2f(945, 480);
    glVertex2f(1000, 470);
    glVertex2f(1005, 490);
    glVertex2f(1080, 465);
    glVertex2f(1095, 485);
    glEnd();
//hay square
    glColor3f(0.8f, 0.6f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(1145, 350); // left top near cart
    glVertex2f(1145, 420);
    glVertex2f(885, 420);
    glVertex2f(885,350);

    glEnd();


    // BULLOCK CART (wooden style, slim wheel, line body)


    // Cart horizontal bars (long wide lines)
    glLineWidth(12.0f);
    glColor3f(0.36f, 0.25f, 0.20f); // dark brown
    glBegin(GL_LINES);
    glVertex2f(880, 420);//
    glVertex2f(1180, 420); // top bar
    glVertex2f(880, 385);
    glVertex2f(1150, 385); // middle bar
    glVertex2f(880, 350);
    glVertex2f(1150, 350); // bottom bar
    glEnd();

    // Vertical posts of the cart
    glLineWidth(8.0f);
    glBegin(GL_LINES);
    for (int x = 900; x <= 1120; x += 50)
    {
        glVertex2f(x, 350);
        glVertex2f(x, 420);
    }
    glEnd();

    // Slim wooden wheel (outer rim only)
    glLineWidth(10.0f);
    glColor3f(0.36f, 0.25f, 0.20f); // dark brown
    glBegin(GL_LINE_LOOP);
    for (int angle = 0; angle < 360; angle++)
    {
        float x = 950 + 35 * cos(angle * M_PI / 180.0);
        float y = 340 + 35 * sin(angle * M_PI / 180.0);
        glVertex2f(x, y);
    }
    glEnd();

    // Wheel spokes
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    for (int angle = 0; angle < 360; angle += 30)
    {
        float x = 950 + 35 * cos(angle * M_PI / 180.0);
        float y = 340 + 35 * sin(angle * M_PI / 180.0);
        glVertex2f(950, 340);
        glVertex2f(x, y);
    }
    glEnd();

    // Cart handles (front sticks to tie bulls)
    glLineWidth(10.0f);
    glBegin(GL_LINES);
    glVertex2f(1110, 305);
    glVertex2f(1120, 350);

    glVertex2f(1140, 305);
    glVertex2f(1120, 350);
    glEnd();



    // RIVER gradient: deep blue (bottom) to lighter blue (top)
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.3f, 0.7f);  // Darker blue at bottom
    glVertex2f(0, 0);
    glVertex2f(1200, 0);
    glColor3f(0.3f, 0.6f, 1.0f);  // Lighter blue at top
    glVertex2f(1200, 300);
    glVertex2f(0, 300);
    glEnd();



    // TREE 1
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(30,425);
    glVertex2f(30,562.5);
    glVertex2f(60,562.5);
    glVertex2f(60,425);
    glEnd();
    //glColor3f(0.0f, 0.6f, 0.0f);
    glColor3f(0.0f, 0.27f, 0.13f);
    drawCircle(30,587.5, 56, 70);
    drawCircle(120,587.5, 50, 70);
    drawCircle(75,650, 70, 70);
    drawCircle(15,675,60, 70);



// HOUSE 2


// top front part
    glBegin(GL_POLYGON);
    glColor3f(0.7f, 0.1f, 0.1f);
    glVertex2f(660,515);
    glColor3f(0.9f, 0.3f, 0.3f);
    glVertex2f(600,575);
    glVertex2f(375,575);
    glVertex2f(375,515);
    glEnd();

// upper house
    glBegin(GL_POLYGON);
    glColor3f(0.9f, 0.5f, 0.5f);
    glVertex2f(495,575);
    glColor3f(1.0f, 0.7f, 0.6f);
    glVertex2f(495,625);
    glVertex2f(330,625);
    glVertex2f(375,575);
    glEnd();

    // Side lines for upper house
    glColor3f(0.1f, 0.1f, 0.1f);  // Dark color
    glLineWidth(5.0f);             // Slightly wide

    glBegin(GL_LINES);
// Vertical line (right side of upper house)
    glVertex2f(495+80, 575);
    glVertex2f(495+80, 610);

// Horizontal line (top side of upper house)
    glVertex2f(495+80, 610);
    glVertex2f(330+165, 610);

// vertical line (left side of upper house)
    glVertex2f(330+165, 575);
    glVertex2f(330+165, 610);

    glVertex2f(330+175, 575);
    glVertex2f(330+175, 610);

    glVertex2f(330+185, 575);
    glVertex2f(330+185, 610);

    glVertex2f(330+195, 575);
    glVertex2f(330+195, 610);


    glVertex2f(330+205, 575);
    glVertex2f(330+205, 610);

    glVertex2f(330+215, 575);
    glVertex2f(330+215, 610);

    glVertex2f(330+225, 575);
    glVertex2f(330+225, 610);

    glVertex2f(330+235, 575);
    glVertex2f(330+235, 610);

    glVertex2f(330+245, 575);
    glVertex2f(330+245, 610);


    glEnd();




// roof
    glBegin(GL_POLYGON);
    glColor3f(0.6f, 0.0f, 0.0f);
    glVertex2f(510,625);
    glColor3f(0.9f, 0.2f, 0.2f);
    glVertex2f(480,662.5);
    glVertex2f(315,662.5);
    glVertex2f(300,640);
    glVertex2f(315,640);
    glVertex2f(330,625);
    glEnd();

// house body
    glBegin(GL_POLYGON);
    glColor3f(0.9f, 0.6f, 0.4f);
    glVertex2f(640,450);
    glVertex2f(640,515);
    glColor3f(1.0f, 0.85f, 0.65f);
    glVertex2f(375,515);
    glVertex2f(375,450);
    glEnd();

// house entry
    glBegin(GL_POLYGON);
    glColor3f(0.4f, 0.2f, 0.1f);
    glVertex2f(660,437.5);
    glVertex2f(660,450);
    glVertex2f(375,450);
    glVertex2f(375,437.5);
    glEnd();

// door
    glBegin(GL_POLYGON);
    glColor3f(0.3f, 0.15f, 0.05f);
    glVertex2f(555,450);
    glVertex2f(555,500);
    glVertex2f(465,500);
    glVertex2f(465,450);
    glEnd();
// door split
    glColor3f(0.0f,0.0f,0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(510,450);
    glVertex2f(510,500);
    glEnd();
// handles
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(500,475);
    glVertex2f(520,475);
    glEnd();

// window 1
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(615,462.5);
    glVertex2f(615,487.5);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(585,487.5);
    glVertex2f(585,462.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(615,462.5);
    glVertex2f(615,487.5);
    glVertex2f(585,487.5);
    glVertex2f(585,462.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(600,462.5);
    glVertex2f(600,487.5);
    glEnd();

// window 2
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(435,462.5);
    glVertex2f(435,487.5);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(405,487.5);
    glVertex2f(405,462.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(435,462.5);
    glVertex2f(435,487.5);
    glVertex2f(405,487.5);
    glVertex2f(405,462.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(420,462.5);
    glVertex2f(420,487.5);
    glEnd();

// upper window 1
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(465,587.5);
    glVertex2f(465,612.5);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(435,612.5);
    glVertex2f(435,587.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(465,587.5);
    glVertex2f(465,612.5);
    glVertex2f(435,612.5);
    glVertex2f(435,587.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(450,587.5);
    glVertex2f(450,612.5);
    glEnd();

// upper window 2
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(390,587.5);
    glVertex2f(390,612.5);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(360,612.5);
    glVertex2f(360,587.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(390,587.5);
    glVertex2f(390,612.5);
    glVertex2f(360,612.5);
    glVertex2f(360,587.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(375,587.5);
    glVertex2f(375,612.5);
    glEnd();



// HOUSE 1

    glBegin(GL_POLYGON);
    glColor3f(0.9f, 0.5f, 0.3f);
    glVertex2f(375, 437.5);
    glVertex2f(375,575);
    glColor3f(0.7f, 0.3f, 0.2f);
    glVertex2f(315,637.5);
    glVertex2f(195,637.5);
    glVertex2f(150,575);
    glVertex2f(75,575);
    glVertex2f(75,437.5);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.6f);
    glVertex2f(375, 437.5);
    glVertex2f(375,562.5);
    glVertex2f(315,625);
    glColor3f(0.85f, 0.55f, 0.3f);
    glVertex2f(195,625);
    glVertex2f(150,562.5);
    glVertex2f(90,562.5);
    glVertex2f(90,437.5);
    glEnd();



// Door
    glBegin(GL_POLYGON);
    glColor3f(0.3f, 0.15f, 0.05f);
    glVertex2f(315, 437.5);
    glVertex2f(315,512.5);
    glVertex2f(270,512.5);
    glVertex2f(270,437.5);
    glEnd();
// door split
    glColor3f(0.0f,0.0f,0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(292.5,437.5);
    glVertex2f(292.5,512.5);
    glEnd();
// handles
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(285,475);
    glVertex2f(300,475);
    glEnd();

// Window 1
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(165, 462.5);
    glVertex2f(165,515);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(120,515);
    glVertex2f(120,462.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(165,462.5);
    glVertex2f(165,515);
    glVertex2f(120,515);
    glVertex2f(120,462.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(142.5,462.5);
    glVertex2f(142.5,515);
    glEnd();

// Window 2
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(240, 462.5);
    glVertex2f(240,515);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(195,515);
    glVertex2f(195,462.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(240,462.5);
    glVertex2f(240,515);
    glVertex2f(195,515);
    glVertex2f(195,462.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(217.5,462.5);
    glVertex2f(217.5,515);
    glEnd();

// Window 3
    glBegin(GL_POLYGON);
    glColor3f(0.55f,0.27f,0.07f);
    glVertex2f(300, 562.5);
    glVertex2f(300,607.5);
    glColor3f(0.65f,0.32f,0.10f);
    glVertex2f(225,607.5);
    glVertex2f(225,562.5);
    glEnd();
// frame
    glColor3f(0.0f,0.0f,0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(300,562.5);
    glVertex2f(300,607.5);
    glVertex2f(225,607.5);
    glVertex2f(225,562.5);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(262.5,562.5);
    glVertex2f(262.5,607.5);
    glEnd();
    // Side lines for upper house
    glColor3f(0.1f, 0.1f, 0.1f);  // Dark color
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    glVertex2f(375, 437.5);
    glVertex2f(375,565);
    glEnd();


// house entry
    glBegin(GL_POLYGON);
    glColor3f(0.5f,0.2f,0.1f);
    glVertex2f(390,425);
    glVertex2f(390,440);
    glVertex2f(55,440);
    glVertex2f(55,425);
    glEnd();





    // TREE 2

    glColor3f(0.0f, 0.27f, 0.13f);
    drawCircle(785,590.5, 50, 70);//1
    drawCircle(75+645,640, 60, 70);//2

    drawCircle(725,585,60, 70);//3
    drawCircle(585,590.5, 50, 70);//1.1
    drawCircle(640,570.5, 50, 70);//1.2

    drawCircle(800,650.5,60, 70);//4
    drawCircle(630,650.5, 70, 70);//5
    drawCircle(750,680, 70, 70);//6

    drawCircle(670,695,60, 70);//7
    //glColor3f(1.0f, 0.0f, 0.0f);
    drawCircle(580,660.5,60, 70);//8

    drawCircle(590,680.5,50, 70);//8



    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_QUADS);
    glVertex2f(675,425);
    glVertex2f(675,562.5);
    glVertex2f(705,562.5);
    glVertex2f(705,425);
    glEnd();

    //root
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_POLYGON);
    glVertex2f(675,425);//a
    glVertex2f(630,405);//b
    glVertex2f(675,410);//c
    glVertex2f(720,390);//d
    glVertex2f(705,410);//e
    glVertex2f(740,415);//f
    glVertex2f(705,425);//g
    glEnd();

// grass
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.35f, 0.0f);   // dark green (bottom)
    glVertex2f(645,392.5);//h

    glColor3f(0.0f, 0.55f, 0.0f);   // softer green (top)
    glVertex2f(625,430);//i

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(655,392.5);//k

    glColor3f(0.0f, 0.55f, 0.0f);   // soft
    glVertex2f(650,440);//j

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(665,392.5);//l

    glColor3f(0.0f, 0.55f, 0.0f);   // soft
    glVertex2f(690,430);//m

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(675,392.5);//n
    glEnd();

    //tree branch
    glColor3f(0.55f, 0.27f, 0.07f);
    glBegin(GL_POLYGON);
    glVertex2f(730,470+50);//d
    glVertex2f(730,455+50);//c
    glVertex2f(705,430+50);//a
    glVertex2f(705,460+50);//b
    glEnd();


// grass2
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.35f, 0.0f);   // dark green (bottom)
    glVertex2f(645+70,392.5+10);//h

    glColor3f(0.0f, 0.55f, 0.0f);   // softer green (top)
    glVertex2f(625+70,430+10);//i

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(655+70,392.5+10);//k

    glColor3f(0.0f, 0.55f, 0.0f);   // soft
    glVertex2f(650+70,440+10);//j

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(665+70,392.5+10);//l

    glColor3f(0.0f, 0.55f, 0.0f);   // soft
    glVertex2f(690+70,430+10);//m

    glColor3f(0.0f, 0.35f, 0.0f);   // dark
    glVertex2f(675+70,392.5+10);//n
    glEnd();





    //tree branch 2
    glColor3f(0.36f, 0.16f, 0.09f);
    drawCircle(730,513.5,8, 70);
    glColor3f(0.91f, 0.76f, 0.65f);
    drawCircle(730,513.5,4, 70);



    //tree branch
    glColor3f(0.55f, 0.27f, 0.07f);

    glBegin(GL_POLYGON);
    glVertex2f(705,562.5);//d
    glVertex2f(730,592.5);//c
    glVertex2f(715,592.5);//a
    glVertex2f(690,575);//b
    glVertex2f(700,625);//d
    glVertex2f(690,635);//c
    glVertex2f(675,560);//a
    glVertex2f(615,620);//b
    glVertex2f(605,630);//a
    glVertex2f(675,562.5);//b
    glEnd();





    //Hay hill
    // HAY HILL (in front of first tree)
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.9f, 0.3f);   // bright yellow (top of hill)
    glVertex2f(0, 420);            // left bottom
    glColor3f(0.95f, 0.75f, 0.1f); // golden (bottom side)
    glVertex2f(160, 420);          // right bottom
    glColor3f(1.0f, 0.85f, 0.2f);  // yellow-gold (top curve)
    glVertex2f(150, 480);          // right top
    glVertex2f(140, 500);          // right top
    glVertex2f(120, 520);           // left top
    glVertex2f(100, 540);           // left top
    glVertex2f(80, 560);           // left top
    glVertex2f(60, 540);           // left top
    glVertex2f(40, 520);           // left top
    glVertex2f(20, 500);
    glVertex2f(10, 480);
    //glEnd();

    // Add some hay texture lines (to make it look real)
    glColor3f(0.85f, 0.65f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(30, 440);
    glVertex2f(50, 470);
    glVertex2f(70, 430);
    glVertex2f(90, 465);
    glVertex2f(110, 435);
    glVertex2f(130, 468);
    glEnd();



    //river 1


    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.3f, 0.7f);  // bottom
    glVertex2f(809,282);//n
    glVertex2f(807,327);//m
    glColor3f(0.3f, 0.6f, 1.0f);  // top
    glVertex2f(462,305);//f
    glVertex2f(465,274);//g
    glEnd();

    glColor3f(0.0f, 0.4f, 0.9f);
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.3f, 0.7f);  // bottom
    glVertex2f(807,327);//m
    glVertex2f(900,440);//k
    glColor3f(0.3f, 0.6f, 1.0f);  // top
    glVertex2f(490,274);//j
    glVertex2f(685,337);//h
    glEnd();

    //soil
    glColor3f(0.823f, 0.705f, 0.549f);
    glBegin(GL_POLYGON);
    glVertex2f(400,265);//g
    glVertex2f(400,300);//f
    glVertex2f(0,300);//d
    glVertex2f(0,265);//e
    glEnd();

    glColor3f(0.823f, 0.705f, 0.549f);
    glBegin(GL_POLYGON);
    glVertex2f(625,300);//i
    glVertex2f(625,330);//h
    glVertex2f(400,300);//f
    glVertex2f(400,265);//g
    glEnd();

    glColor3f(0.823f, 0.705f, 0.549f);
    glBegin(GL_POLYGON);
    glVertex2f(625,300);//i
    glVertex2f(890,430);//g
    glVertex2f(900,440);//k
    glVertex2f(625,330);//h

    glEnd();
    glColor3f(0.823f, 0.705f, 0.549f);
    glBegin(GL_POLYGON);
    glVertex2f(1200,265);//p
    glVertex2f(1200,300);//o
    glVertex2f(840,300);//s
    glVertex2f(807,305);//m
    glVertex2f(807,265);//n
    glEnd();


    // BOAT

    glPushMatrix();                  // Save current coordinate system
    glTranslatef(boatPosX, 0.0f, 0); // Move boat along x-axis

    //SAIL (Right side )
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex2f(630, 125);   // Bottom-left
    glVertex2f(630, 560);   // Much taller mast height
    glVertex2f(880, 300);   // Wider outward point
    glVertex2f(800, 125);   // Bottom-right
    glEnd();

    //SAIL (Left side)
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_TRIANGLES);
    glVertex2f(630, 125);   // Bottom-right
    glVertex2f(630, 560);   // Same tall height as mainsail
    glVertex2f(400, 125);   // Much farther left for width
    glEnd();

    // Stick
    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(625, 125);
    glVertex2f(635, 125);
    glVertex2f(635, 580);   // Taller mast
    glVertex2f(625, 580);
    glEnd();

    //back body
    glColor3f(0.0f, 0.2f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(855, 100);
    glVertex2f(910, 125);
    glVertex2f(345, 125);
    glVertex2f(390, 100);//365
    glEnd();


    //Human
    // Head (circle)
    glColor3f(0.9f, 0.8f, 0.6f);
    drawCircle(400, 270, 30, 100);

    //hair
    // Hair (polygon on top of head)
    glColor3f(0.1f, 0.1f, 0.1f); // Dark black hair
    glBegin(GL_POLYGON);
    glVertex2f(370, 280);  // left side of head
    glVertex2f(430, 280);  // right side of head
    glVertex2f(440, 300);  // top right
    glVertex2f(400, 320);  // top middle
    glVertex2f(360, 300);  // top left
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f); // Dark black hair
    glBegin(GL_POLYGON);
    glVertex2f(400, 250);  //1
    glVertex2f(400, 280);  // 2
    glVertex2f(370, 280);  //3
    glVertex2f(370, 250);  // 4

    glEnd();


    // Eyes (small black circles)
    glColor3f(0.0f, 0.0f, 0.0f); // Black
    drawCircle(390, 265, 3, 50); // Left eye
    drawCircle(410, 265, 3, 50); // Right eye

    // Eyebrows (thin black rectangles/lines above eyes)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON); // Left eyebrow
    glVertex2f(383, 273);
    glVertex2f(397, 273);
    glVertex2f(397, 276);
    glVertex2f(383, 276);
    glEnd();

    glBegin(GL_POLYGON); // Right eyebrow
    glVertex2f(403, 273);
    glVertex2f(417, 273);
    glVertex2f(417, 276);
    glVertex2f(403, 276);
    glEnd();

    // Lips (smile shape)


    // Lips (just one line)
    glColor3f(0.8f, 0.2f, 0.2f); // Red lips
    glBegin(GL_LINES);
    glVertex2f(420, 250); // left corner
    glVertex2f(410, 250); // right corner
    glEnd();



    // Body (rectangle)
    glColor3f(0.2f, 0.2f, 0.8f);
    glBegin(GL_POLYGON);
    glVertex2f(430, 165);
    glVertex2f(430, 240);
    glVertex2f(370, 240);
    glVertex2f(370, 165);
    glEnd();

    // Arm
    glColor3f(0.9f, 0.8f, 0.6f);
    glBegin(GL_POLYGON);
    glVertex2f(445, 220-40);
    glVertex2f(445, 200-30);
    glVertex2f(380, 200);
    glVertex2f(380, 215);
    glEnd();

    // Leg
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_POLYGON);
    glVertex2f(490, 100);//1
    glVertex2f(470, 150);//2
    glVertex2f(470-30, 170);//3
    glVertex2f(430-30, 170);//4
    glVertex2f(400-30, 165);//5
    glVertex2f(430-70, 130);
    glVertex2f(410-50, 100);//6
    glEnd();

    // Oar (long brown rectangle)
    glColor3f(0.5f, 0.25f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(455, 200);
    glVertex2f(470, 200);
    glVertex2f(350, 70);
    glVertex2f(365, 70);
    glEnd();

// Oar Blade (flat part touching water)
    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_POLYGON);
    glVertex2f(330, 60);
    glVertex2f(380, 60);
    glVertex2f(380, 90);
    glVertex2f(330, 90);
    glEnd();




    //boat part
    glColor3f(0.5f, 0.2f, 0.1f);
    glBegin(GL_POLYGON);
    glVertex2f(870, 62.5);
    glVertex2f(945, 150);
    glVertex2f(840, 110);
    glVertex2f(340, 110);//365
    glVertex2f(390, 62.5);//960

    glEnd();

    glColor3f(0.5f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLES);
    glVertex2f(430,95);
    glVertex2f(300, 150);
    glVertex2f(350, 100);
    glEnd();

    glPopMatrix();


    //flower

// flower 2
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.9f); // soft pink
    glVertex2f(75+40,100-40);
    glVertex2f(150+40,155-40);
    glColor3f(1.0f, 1.0f, 1.0f); // white
    glVertex2f(150+40,200-40);
    glVertex2f(75+40,165-40);
    glEnd();

// flower 3
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.9f);
    glVertex2f(75-15,63);
    glVertex2f(50,120);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0,125);
    glVertex2f(30,75);
    glEnd();

// flower 4
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.9f);
    glVertex2f(75-15,38);
    glVertex2f(50,63);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0,40);
    glVertex2f(30,50);
    glEnd();

// flower 1.0
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.9f);
    glVertex2f(75,100);
    glVertex2f(150,188-35);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(150,250-50);
    glVertex2f(75,200-35);
    glEnd();

// flower 3 (near 255,38)
    glBegin(GL_POLYGON);
    glColor3f(1.0f, 0.8f, 0.9f);
    glVertex2f(255,38);
    glVertex2f(225,63);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(165,75);
    glVertex2f(195,40);
    glEnd();

// grass 1 (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(90,0);
    glVertex2f(150,125);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(150,115);
    glVertex2f(120,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(150,125);
    glVertex2f(210,50);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(150,95);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    glVertex2f(120,0);
    glVertex2f(150,125);
    glEnd();

// grass 2 (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(90+15,0);
    glVertex2f(150+15,125);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(150+15+15,115);
    glVertex2f(120+15,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(150+15,125);
    glVertex2f(210+15,50);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(150+15,95);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(0.5f);
    glBegin(GL_LINES);
    glVertex2f(90+25,0);
    glVertex2f(150+15,125);
    glEnd();

// grass 3 (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(90,0+35);
    glVertex2f(150,125+35);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(150,115+35);
    glVertex2f(120,0+35);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(150,125+35);
    glVertex2f(210,50+35);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(150,95+35);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(100,0+35);
    glVertex2f(150,115+35);
    glEnd();

// grass 4 (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(90,0);
    glVertex2f(150-60,125);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(120,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(100,0);
    glVertex2f(150-60,125);
    glEnd();

// grass 5 (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(90,0);
    glVertex2f(150-90,125);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(140,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(100,0);
    glVertex2f(150-90,125);
    glEnd();

// grass 6 left (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(70,0);
    glVertex2f(0,115);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(50,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(10,85);
    glVertex2f(0,125);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(0,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(78,0);
    glVertex2f(0,115);
    glEnd();

// grass 7 (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(90+15,0);
    glVertex2f(0+15,125);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(50+15,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(90+20,0);
    glVertex2f(0+15,120);
    glEnd();

// grass 7 small (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(45,0);
    glVertex2f(10,25);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(30,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(35,0);
    glVertex2f(10,25);
    glEnd();

// grass 8 (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(55,0);
    glVertex2f(0,55);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(40,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(50,0);
    glVertex2f(0,55);
    glEnd();

// grass 9 left (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(70,0);
    glVertex2f(0,115-35);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(50,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(10,85-20);
    glVertex2f(0,125-40);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(0,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(10,85-20);
    glVertex2f(0,125-40);
    glEnd();

// grass 10 left (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(120,0);
    glVertex2f(50,170);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(85,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(55,150);
    glVertex2f(0,130);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(50,170);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(55,150);
    glVertex2f(0,130);
    glEnd();

// grass 11 left (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(120-15,0);
    glVertex2f(50-15,170);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(85-15,0);
    glEnd();

    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(55-15,150-15);
    glVertex2f(0,130-15);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(50-15,170-15);
    glEnd();

// grass 12 left (Type A)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.6f, 0.0f);
    glVertex2f(150,0);
    glVertex2f(220,130);
    glColor3f(0.0f, 0.8f, 0.0f);
    glVertex2f(150-20,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(150,0);
    glVertex2f(220,130);
    glEnd();

// grass 13 left (Type B)
    glBegin(GL_POLYGON);
    glColor3f(0.0f, 0.5f, 0.0f);
    glVertex2f(180,0);
    glVertex2f(230,80);
    glColor3f(0.1f, 0.7f, 0.1f);
    glVertex2f(180-20,0);
    glEnd();

// line
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(180,0);
    glVertex2f(230,80);
    glEnd();
    //glFlush();
}
void updateCloud(int value)
{
    if (moveCloud)
    {
        cloudPosX += 2.0f;   // cloud speed
        if (cloudPosX > 1200) cloudPosX = -300; // reset clouds when out of screen
    }
    glutPostRedisplay();
   // glutTimerFunc(30, updateCloud, 0);
}

void initVillage()
{
    glClearColor(0.7f, 0.9f, 1.0f, 1.0f);  // background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 1000, 0, 600);           // same for both
}
void updateBoat(int value)
{
    boatPosX += 4.0f;
    if (boatPosX > 1200)
    {
        boatPosX = -200;
    }
    glutPostRedisplay();
    //glutTimerFunc(30, updateBoat, 0);
}


void vkeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'd':  // move clouds right
        moveCloud = true;
        break;
    case 's':  // stop clouds
        moveCloud = false;
        break;
    case 'a':  // step clouds left
        cloudPosX -= 10;
        break;
    }
    //glutPostRedisplay();
}




//================================Abir end=============================//

/*==========================Main function================================== */

int currentScene = 0;  // global, only once

void init() {
    if (currentScene == 0) initDhaka();
    else initBeach();
}

void globalKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case '1':                 // Dhaka
        currentScene = 0;
        initDhaka();          // scene-specific states only
        glutPostRedisplay();
        break;

    case '2':                 // Beach
        currentScene = 1;
        initBeach();
        glutPostRedisplay();
        break;

    case '3':                 // Village / Boat scene
        currentScene = 2;
        initVillage();        // create this: reset boatPosX, cloudPosX, etc.
        glutPostRedisplay();
        break;

    default:
        // Route other keys to the active scene’s own keyboard
        if (currentScene == 0)
            rkeyboard(key, x, y);      // Dhaka keys (day/night, rain, etc.)
        else if (currentScene == 1)
            pkeyboard(key, x, y);      // Beach keys
        else if (currentScene == 2)
            vkeyboard(key, x, y);      // Village/boat keys (d/s/a)
        break;
    }
}



void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (currentScene == 0)
        gluOrtho2D(0, 1000, 0, 600);   // Dhaka
    else if (currentScene == 1)
        gluOrtho2D(0, 1000, 0, 600);   // Beach (same size)
    else
        gluOrtho2D(0, 1200, 0, 750);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (currentScene == 0)
        displayDhaka();
    else if (currentScene == 1)
        pdisplay();          // beach
    else if (currentScene == 2)
        displayVillageScene3();

    glutSwapBuffers();
}



// Fix the globalIdle function
void globalIdle() {
    if (currentScene == 0) {
        // Dhaka animation - rain update only (rtimer handles the rest)
        if (isRaining) {
            rainOffset += 1.0f;
            if (rainOffset > 100) rainOffset -= 100;
        }
    } else {
        // Beach animation update
        ptimer();  // This will update all beach animations
    }
    glutPostRedisplay();
}

/// Add this new timer function that handles both scenes
void globalTimer(int value) {
    if (currentScene == 0) {
        // Dhaka scene updates (existing rtimer logic)
        personX += 1.2;
        personX2 -= 0.5;
        legAnimation += 0.08;

        if (personX > 1650) personX = -50;
        if (personX2 < -50) personX2 = 1650;

        personX_road2 += 1.5;
        personX2_road2 -= 1.5;
        if (personX_road2 > 1650) personX_road2 = -50;
        if (personX2_road2 < -50) personX2_road2 = 1650;

        // Metro train logic
        if (trainState == 0) {
            metroTrainX += 4;
            if (metroTrainX >= STATION_ENTER && metroTrainX < STATION_ENTER + 20) {
                trainState = 1;
            }
        } else if (trainState == 1) {
            metroTrainX += 2;
            if (metroTrainX >= STATION_STOP_POS) {
                metroTrainX = STATION_STOP_POS;
                trainState = 2;
                stopTimer = 0.0f;
            }
        } else if (trainState == 2) {
            stopTimer += 0.03f;
            if (stopTimer >= 3.0f) {
                trainState = 0;
            }
        }
        if (metroTrainX > 1650) {
            metroTrainX = -320;
            trainState = 0;
            stopTimer = 0.0f;
        }

        // Vehicle updates
        car1X += 3.8; bus1X += 2.8; truck1X += 2.5;
        car2X -= 3.8; bus2X -= 2.8; truck2X -= 2.5;
        car3X += 4.5; truck3X += 3;
        bus3X -= 3.5; car4X -= 4;
        car5X += 3.8; bus4X += 3;
        truck4X -= 2.5; car6X -= 3.5;

        if (car1X > 1650) car1X = -60;
        if (bus1X > 1650) bus1X = -110;
        if (truck1X > 1650) truck1X = -100;
        if (car2X < -60) car2X = 1650;
        if (bus2X < -110) bus2X = 1650;
        if (truck2X < -100) truck2X = 1650;
        if (car3X > 1650) car3X = -60;
        if (truck3X > 1650) truck3X = -100;
        if (bus3X < -110) bus3X = 1650;
        if (car4X < -60) car4X = 1650;
        if (car5X > 1650) car5X = -60;
        if (bus4X > 1650) bus4X = -110;
        if (truck4X < -100) truck4X = 1650;
        if (car6X < -60) car6X = 1650;

        nightTimer += 0.016f;
        if (!isNight) {
            cloudOffset += 0.4f;
            if (cloudOffset > 2000) cloudOffset -= 2000;
        }
        if (nightTimer >= DAY_NIGHT_CYCLE) {
            nightTimer = 0.0f;
            isNight = !isNight;
        }
        if (isRaining) {
            rainOffset += 1.0f;
            if (rainOffset > 100) rainOffset -= 100;
        }
    } else if (currentScene == 1) {
        // Beach scene updates (ptimer logic)
        ptimer();
    }
    else if (currentScene == 2) {
        // Scene 3 (village/boat) updates
        updateBoat(0);       // or inline boatPosX logic
        updateCloud(0);      // or your cloudPosX logic
        // make sure these functions do NOT call glutTimerFunc again
        // and only change positions
    }

    glutPostRedisplay();
    glutTimerFunc(30, globalTimer, 0);  // Call itself again
}
void terminal() {
    cout << "========================================" << endl;
    cout << " PORTRAIT OF BANGLADESH - OpenGL Scenes " << endl;
    cout << "========================================" << endl;
    cout << "Created by: Rashedul Alam" << endl;
    cout << "Scenes: Dhaka City, Beach, Village Boat" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "GLOBAL CONTROLS:" << endl;
    cout << "  1       -> Dhaka City Scene" << endl;
    cout << "  2       -> Beach Scene" << endl;
    cout << "  3       -> Village & Boat Scene" << endl;
    cout << "  ESC     -> Exit Program" << endl;
    cout << endl;
    cout << "DHAKA SCENE:" << endl;
    cout << "  N / n   -> Night Mode" << endl;
    cout << "  D / d   -> Day Mode" << endl;
    cout << "  R / r   -> Toggle Rain ON/OFF" << endl;
    cout << "  Auto    -> Day/Night cycle (~20s)" << endl;
    cout << endl;
    cout << "BEACH SCENE:" << endl;
    cout << "  Scene-specific keys (waves, sun, etc.)" << endl;
    cout << endl;
    cout << "VILLAGE & BOAT SCENE:" << endl;
    cout << "  D       -> Move clouds right" << endl;
    cout << "  S       -> Stop clouds" << endl;
    cout << "  A       -> Step clouds left" << endl;
    cout << endl;
    cout << "FEATURES:" << endl;
    cout << "  - Animated metro train with station" << endl;
    cout << "  - Moving vehicles on 3 roads" << endl;
    cout << "  - Walking people with umbrellas in rain" << endl;
    cout << "  - Beach environment with animation" << endl;
    cout << "  - Village river, boat and bullock cart" << endl;
    cout << "  - Dynamic day/night and rain effects" << endl;
    cout << "========================================" << endl;
    cout << "Program Started. Enjoy the simulation!" << endl;
    cout << "========================================" << endl;
    cout << endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(1000, 600);

    terminal();
    glutCreateWindow("Portrait of Bangladesh");


    initDhaka();
    glutDisplayFunc(display);
    glutKeyboardFunc(globalKeyboard);
    glutSpecialFunc(pspecialKeys);
    glutMouseFunc(pmouse);
    glutTimerFunc(0, globalTimer, 0);

    glutMainLoop();

    return 0;
}


