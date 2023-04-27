#include <GLUT/glut.h>

float x_position = 0.0f;
float y_position = -0.8f;


float triangle_speed = 0.5f; // pixels per second

int window_width = 640;
int window_height = 480;

void display()
{
    float scale = 10.0f;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
     glTranslatef(0.0f, 0.0f, 0.0f);
     glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f/scale, 1.0f/scale);
    glVertex2f(-0.6f/scale, -0.6f/scale);
    glVertex2f(0.0f/scale,  -0.3f/scale);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f/scale, 1.0f/scale);
    glVertex2f(0.6f/scale, -0.6f/scale);
    glVertex2f(0.0f/scale,  -0.3f/scale);
    glEnd();


    glTranslatef(x_position, y_position, 0.0f);
    
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex2f(0.0f/scale, 1.0f/scale);
    glVertex2f(-0.6f/scale, -0.6f/scale);
    glVertex2f(0.0f/scale,  -0.3f/scale);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0f/scale, 1.0f/scale);
    glVertex2f(0.6f/scale, -0.6f/scale);
    glVertex2f(0.0f/scale,  -0.3f/scale);


    glEnd();
   
    glFlush();
    glutPostRedisplay();

    glutSwapBuffers();
}

void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}


void update(int elapsed_time) {
    float delta_time = elapsed_time / 1000.0f; // convert to seconds
    
    float delta_position_x = triangle_speed * delta_time;
    
    
    // wrap triangle position around the screen
    if (x_position > window_width / 2.0f) {
        x_position = -window_width / 2.0f;
    }
    
    glutPostRedisplay();
    
    // call this function again after a delay
    glutTimerFunc(16, update, glutGet(GLUT_ELAPSED_TIME));
}


void specialKeys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        x_position -= triangle_speed * 10.0f;
        break;
    case GLUT_KEY_RIGHT:
        x_position += triangle_speed * 10.0f;
        break;
    }

}


void specialKeysUp(unsigned char key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        x_position -= triangle_speed * 10.0f;
        break;
    case GLUT_KEY_RIGHT:
        x_position += triangle_speed * 10.0f;
        break;
    }

}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(specialKeys);
    glutKeyboardUpFunc(specialKeysUp);
    glutTimerFunc(0, update, glutGet(GLUT_ELAPSED_TIME));
    glutMainLoop();
    return 0;
}
