#include <GLUT/glut.h>

GLfloat x_position = 0.0f;
GLfloat y_position = -0.8f;

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


void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        x_position -= 0.05f;
        glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        x_position += 0.05f;
        glutPostRedisplay();
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
    glutSpecialFunc(specialKeys);
    glutMainLoop();
    return 0;
}
