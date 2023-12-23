#include <GL/glut.h>
#include <cmath>
#include <cstdlib> // Untuk menggunakan fungsi rand()
#include <ctime>


// Ukuran jendela
const int window_width = 800;
const int window_height = 600;

// Variabel untuk mengontrol rotasi dan posisi bulan.
float rotationAngle = 0.0f;
float eyeX = 0.0f;
float eyeY = 0.0f;
float eyeZ = 5.0f;

// Variabel untuk kontrol skala objek
float scaleX = 1.0f;
float scaleY = 1.0f;
float scaleZ = 1.0f;

const float minScale = 0.1f;
const float maxScale = 2.0f;

// Posisi awal bulan
float moonX = 0.0;
float moonY = 0.0;
float moonZ = -5.0;

float rotationSpeed = 0.01f; // Deklarasikan variabel ini di awal program

// Fungsi untuk menggambar setengah bentuk bulan(bulat)
void drawHalfSphere(float radius, int slices, int stacks, bool isTop) {
    const float pi = 3.14159265359;
    int startStack = isTop ? stacks / 2 : 0;
    int endStack = isTop ? stacks : stacks / 2;

    for (int i = startStack; i <= endStack; ++i) {
        float lat0 = pi * (-0.5 + (float)(i - 1) / stacks);
        float z0 = radius * sin(lat0);
        float zr0 = radius * cos(lat0);

        float lat1 = pi * (-0.5 + (float)i / stacks);
        float z1 = radius * sin(lat1);
        float zr1 = radius * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; ++j) {
            float lng = 2 * pi * (float)(j - 1) / slices;
            float x = cos(lng);
            float y = sin(lng);

            // Warna biru muda
            glColor3ub(208, 234, 240);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);

            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

// Fungsi untuk menggambar silinder yang merupakan bagian dari badan roket
void drawCylinder(float radius, float height, int segments) {
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = (2.0f * 3.1415926f * float(i)) / float(segments);

        float x = radius * cos(theta);
        float y = radius * sin(theta);

        glVertex3f(x, y, 0.0);
        glVertex3f(x, y, height);
    }
    glEnd();
}


// Fungsi untuk menggambar roket
void drawRocket(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    // Badan roket (silinder) - Warna kuning
    glColor3f(1.0, 1.0, 0.0);
    drawCylinder(0.1, 0.6, 20);

    // Jendela kaca (silinder kecil) - Warna biru muda tanpa transparansi
    glColor3f(0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 0.3);
    drawCylinder(0.05, 0.05, 20);

    // Puncak roket (kerucut) - Warna abu-abu
    glColor4f(0.7, 0.7, 0.7, 1.0); 
    glTranslatef(0.0, 0.0, 0.3);
    glutSolidCone(0.1, 0.2, 20, 20);


    // Ekor roket (limas) - Warna kuning
    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(0.0, 0.0, -0.2);
    glutSolidCone(0.1, 0.1, 20, 20);

    // Sayap kiri (kubus) - Warna biru
    glColor3f(0.0, 0.0, 1.0);
    glPushMatrix();
    glTranslatef(0.05, 0.0, 0.15);
    glScalef(0.7, 0.05, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    // Sayap kanan (kubus) - Warna biru
    glPushMatrix();
    glTranslatef(-0.1, 0.0, 0.0);
    glScalef(0.7, 0.05, 0.1);
    glutSolidCube(1.0);
    glPopMatrix();

    glPopMatrix();
}



// Fungsi untuk untuk menggambar bentuk bulan secara lengkap.
void drawSphere(float radius, int slices, int stacks) {
    drawHalfSphere(radius, slices, stacks, true);
    drawHalfSphere(radius, slices, stacks, false);
}


// Fungsi untuk menggambar awan
void drawCloud(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    glColor3f(0.3, 0.3, 0.3); // Warna abu-abu gelap

    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.0);
    glutSolidSphere(0.5, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.5, 0.0, 0.0);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5, 0.0, 0.0);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 0.5);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, -0.5);
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    glPopMatrix();
}


// Fungsi untuk menggambar bintang-bintang
void drawStars(int numStars) {
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(2.0);
    glBegin(GL_POINTS);

    for (int i = 0; i < numStars; ++i) {
        float x = static_cast<float>(rand() % 50 - 25);
        float y = static_cast<float>(rand() % 50 - 25);
        float z = static_cast<float>(rand() % 20 - 10);

        glVertex3f(x, y, z);
    }

    glEnd();
}


// Fungsi untuk merender seluruh tampilan
void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matriks
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(window_width) / static_cast<double>(window_height), 1.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);

    glScalef(scaleX, scaleY, scaleZ);  // Terapkan scaling

    glClearColor(0.0, 0.0, 0.0, 1.0); // Warna latar belakang hitam

    glRotatef(rotationAngle, 0.0, 1.0, 0.0);

    glClear(GL_COLOR_BUFFER_BIT);


    // Draw stars before other elements
    drawStars(500); // Ubah 500 menjadi jumlah bintang yang Anda inginkan

    // Draw awan yang menutupi matahari di bagian depan
    drawCloud(2.5, 0.0, -2.5, 0.8);
    drawCloud(-3.0, 1.5, 0.8, 0.5);

    // Gambar matahari
    const float moonRadius = 1.0;
    const int moonSlices = 30;
    const int moonStacks = 30;
    drawSphere(moonRadius, moonSlices, moonStacks);
    drawStars(500);


    // Draw awan di belakang matahari
    drawCloud(1.5, 1.0, 0.0, 0.5);
    drawCloud(-1.0, 1.8, -1.0, 0.3);
    drawCloud(-2.0, 0.5, 1.0, 0.7);

    // Draw additional clouds
    drawCloud(2.0, 0.0, -2.0, 0.6);
    drawCloud(-2.5, 1.2, 0.5, 0.4);

    // Draw awan di bagian atas dan bawah
    drawCloud(1.0, 3.0, 0.0, 0.4);
    drawCloud(-1.5, -2.5, 0.0, 0.6);

    drawStars(500);

    // Draw clouds
    drawCloud(2.0, -2.0, -1.0, 0.5);
    drawCloud(-1.0, 2.5, 1.0, 0.4);
    drawCloud(0.5, -1.0, 1.5, 0.7);

    // Draw roket
    drawRocket(1.0, -1.0, -3.0, 1.8);
    // Draw additional satellite

    rotationAngle += 0.25;  // Sesuaikan kecepatan rotasi sesuai kebutuhan

    glutSwapBuffers();
}


// Callback fungsi untuk tombol keyboard standar
void mouseCallback(int button, int state, int x, int y) {
    static int lastX = x;
    static int lastY = y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Saat tombol kiri ditekan, atur faktor scaling berdasarkan pergerakan mouse
        scaleX += 0.1f;
        scaleY += 0.1f;
        scaleZ += 0.1f;

        // Batasi ke maksimum
        if (scaleX > maxScale) scaleX = maxScale;
        if (scaleY > maxScale) scaleY = maxScale;
        if (scaleZ > maxScale) scaleZ = maxScale;
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        // Saat tombol kanan ditekan, atur faktor scaling berdasarkan pergerakan mouse
        scaleX -= 0.1f;
        scaleY -= 0.1f;
        scaleZ -= 0.1f;

        // Batasi ke minimum
        if (scaleX < minScale) scaleX = minScale;
        if (scaleY < minScale) scaleY = minScale;
        if (scaleZ < minScale) scaleZ = minScale;
    }
    else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
        // Saat tombol tengah ditekan, atur faktor scaling berdasarkan pergerakan mouse
        scaleX += 0.1f * (x - lastX);
        scaleY -= 0.1f * (y - lastY);

        // Batasi ke maksimum dan minimum
        if (scaleX > maxScale) scaleX = maxScale;
        if (scaleY > maxScale) scaleY = maxScale;
        if (scaleZ > maxScale) scaleZ = maxScale;
        if (scaleX < minScale) scaleX = minScale;
        if (scaleY < minScale) scaleY = minScale;
        if (scaleZ < minScale) scaleZ = minScale;
    }

    lastX = x;
    lastY = y;

    glutPostRedisplay();
}

// Callback fungsi untuk tombol keyboard standar
void keyboardCallback(unsigned char key, int x, int y) {
    switch (key) {
    case 'a':
        rotationSpeed -= 0.01f;  
        break;
    case 'd':
        rotationSpeed += 0.01f;  
        break;
    }

    glutPostRedisplay();
}

void mouseMotionCallback(int x, int y) {
    // Atur faktor scaling berdasarkan pergerakan mouse
    scaleX += 0.01f * (x - window_width / 2);
    scaleY += 0.01f * (y - window_height / 2);

    glutPostRedisplay();
}


// Fungsi untuk meng-update rotasi dan memanggil glutPostRedisplay()
void update(int value) {
    rotationAngle += rotationSpeed;
    rotationSpeed -= 0.0001f;
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}


// Callback fungsi untuk tombol khusus keyboard
void specialKeys(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        eyeX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        eyeX += 0.1f;
        break;
    case GLUT_KEY_UP:
        eyeY += 0.1f;
        break;
    case GLUT_KEY_DOWN:
        eyeY -= 0.1f;
        break;
    }

    glutPostRedisplay();
}


//fungsi utama program
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Langit malam hari");

    //Mengaktifkan fitur OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Menetapkan fungsi-fungsi callback
    glutDisplayFunc(renderScene);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(25, update, 0);


    glutMouseFunc(mouseCallback);
    glutMotionFunc(mouseMotionCallback);
    glutKeyboardFunc(keyboardCallback);

    glutMainLoop();

    return 0;
}
