#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include<GL/gl.h>
#include<GL/glut.h>

#define N 4 // number of control points
#define D 2 // number of dimensions
#define T 100 // number of u subintervals

static GLfloat ctrlPoints[N][3] = {{100, 100, 0}, {200, 350, 0}, {400,300, 0}, {480,100,0}};
GLUnurbsObj *theNurb;


int typeMode = 1;	 // 1 = hermite cubic (with specific endpoint tangents)
					 // 2 = bezier cubic (Bernstein) 

int ww = 1000;
int wh = 800;
int MOVENAME = -1;
int pickRadius = 50;

void printControlPoints()
{
	printf("\nControl Points:\n");

	for (int i = 0; i < N; i++)
	{
		printf("  ( ");
		for (int j = 0; j < D; j++)
		{
		 printf("%g ",ctrlPoints[i][j]);
		}
		printf(")\n");
	}
}

void display2DControlPolyline()
{
	glLineWidth(2.0);
	glColor3f(1.0f, 0.0f, 0.0f);
  
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < N; i++)
	{
      glVertex2i(ctrlPoints[i][0],ctrlPoints[i][1]);
	}
    glEnd();
    glFlush();
}

void display2DControlPoints()
{
	glPointSize(3.0);
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_POINTS);
	for (int i = 0; i < N; i++)
	{
      glVertex2i(ctrlPoints[i][0],ctrlPoints[i][1]);
	}
    glEnd();
    glFlush();
}

void interpolate()
{
	printf("\nCalculating Interpolating Form...\n");

	glLineWidth(4.0);
	glColor3f(1.0f, 1.0f, 1.0f);

	GLfloat bu[N];
  
	glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt/(GLfloat)T;
		bu[0] = (-9.0/2.0)*(u-(1.0/3.0))*(u-(2.0/3.0))*(u-1.0);
		bu[1] = (27.0/2.0)*u*(u-(2.0/3.0))*(u-1.0);
		bu[2] = (-27.0/2.0)*u*(u-(1.0/3.0))*(u-1.0);
		bu[3] = (9.0/2.0)*u*(u-(1.0/3.0))*(u-(2.0/3.0));

		GLfloat x = 0.0;
		GLfloat y = 0.0;

		for (int i = 0; i < N; i++)
		{
			x += bu[i]*ctrlPoints[i][0];
			y += bu[i]*ctrlPoints[i][1];
		}
		printf("x=%g  y=%g\n", x, y);
		glVertex2i(x,y);
	}

	glEnd();
    glFlush();
}

void hermite()
{
	printf("\nCalculating Hermite Form with specific endpoint tangents...\n");

	glLineWidth(4.0);
	glColor3f(1.0f, 1.0f, 1.0f);

	GLfloat fu[N];
  
	glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt/(GLfloat)T;
		GLfloat u2 = u*u;
		GLfloat u3 = u2*u;
		fu[0] = 2.0*u3 - 3.0*u2 + 1.0;
		fu[1] = -2.0*u3 + 3.0*u2;
		fu[2] = u3 - 2.0*u2 + u;
		fu[3] = u3 - u2;

		GLfloat x = 0.0;
		GLfloat y = 0.0;
	
		// p0 = ctrlPoints[0]
		x += fu[0]*ctrlPoints[0][0];
		y += fu[0]*ctrlPoints[0][1];

		// p1 = ctrlPoints[3]
		x += fu[1]*ctrlPoints[3][0];
		y += fu[1]*ctrlPoints[3][1];

	    //  tangent at p0 = ctrlPoints[1]-ctrlPoints[0]
		x += fu[2]*(3.0*(ctrlPoints[1][0]-ctrlPoints[0][0]));
		y += fu[2]*(3.0*(ctrlPoints[1][1]-ctrlPoints[0][1]));

		//  tangent at p1 = ctrlPoints[3]-ctrlPoints[2]
		x += fu[3]*(6.0*(ctrlPoints[3][0]-ctrlPoints[2][0]));
		y += fu[3]*(6.0*(ctrlPoints[3][1]-ctrlPoints[2][1]));
		
		printf("x=%g  y=%g\n", x, y);
		glVertex2i(x,y);
	}

	glEnd();
    glFlush();
}

GLfloat bernstein(GLfloat u, int i, int n)
{
	GLfloat choose[N] = {1, 3, 3, 1};
	GLfloat nChoosei = choose[i];
	GLfloat ui = pow(u,i);
	GLfloat oneMinusu = pow(1.0-u, n-i);

	return (nChoosei*ui*oneMinusu);
}

void bezier()
{
	printf("\nCalculating Bezier Form using Bernstein polynomials...\n");

	glLineWidth(4.0);
	glColor3f(1.0f, 1.0f, 1.0f);
  
	glBegin(GL_LINE_STRIP);

	for (int uInt = 0; uInt <= T; uInt++)
	{
		GLfloat u = uInt/(GLfloat)T;

		GLfloat x = 0.0;
		GLfloat y = 0.0;

		for (int i = 0; i < N; i++)
		{
			GLfloat b = bernstein(u,i,N-1);
			x += b*ctrlPoints[i][0];
			y += b*ctrlPoints[i][1];
		}
		printf("x=%g  y=%g\n", x, y);
		glVertex2i(x,y);
	}

	glEnd();
    glFlush();
}

void drawCurve()
{
	switch (typeMode){
		case 1:
			hermite();
			break;
		case 2:
			bezier();
			break;
		default:
			bezier();
			// exit(0);
	}
	
}

void myDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT);
  display2DControlPolyline();
  display2DControlPoints();
  drawCurve();
  glFlush();
}


void init()
{
   glClearColor(0.0, 0.0, 1.0, 0.0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, ww, 0.0, wh);
}

// mouse function
void myPick(int button, int state, int xPosition, int yPosition)
{
	// left mouse button down
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLuint newX = xPosition;
		GLuint newY = wh - yPosition;
		printf("Pick location: x = %d   y = %d\n", newX, newY);

	// determine which control point is picked

	int choiceFound = 0;

	for (int i = 0; i < N && !choiceFound; i++)
	{
		// Use globally defined pickRadius

		if ((abs(ctrlPoints[i][0] - newX) <= pickRadius) &&
		    (abs(ctrlPoints[i][1] - newY) <= pickRadius))
		{
			MOVENAME = i;
			choiceFound = 1;
			printf("Control point %d was picked.\n",MOVENAME);
		}
	}
	}

	// left mouse button up
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		MOVENAME = -1;
	}
	glutPostRedisplay();
}

// mouse motion function
void myMouseMove(int xPosition, int yPosition)
{
	if (MOVENAME > -1)
	{
	 GLuint newX = xPosition;
	 GLuint newY = wh - yPosition;
	 printf("New control point %d location: x = %d   y = %d\n", MOVENAME, newX, newY);

	 ctrlPoints[MOVENAME][0] = newX;
	 ctrlPoints[MOVENAME][1] = newY;
	
	glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	ww = w;
	wh = h;
}
void myKey(unsigned char key,int x,int y) {

    if(key=='h') {
        glFlush();
        // CyrusBeck();
        typeMode=1;
        // bezier();
        glutPostRedisplay();
    }
    if(key=='b'){
    	glFlush();
    	typeMode=2;
    	// hermite();
    	glutPostRedisplay();
    }
}


int main(int argc, char **argv)
{
   printControlPoints();

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(ww, wh);
   glutInitWindowPosition(0,0);
   glutCreateWindow("91.580 2D Curve Evaluation");
   glutDisplayFunc(myDisplay);
   glutMouseFunc(myPick);
   glutMotionFunc(myMouseMove);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(myKey);

   init();

//    createMenu();

	glutMainLoop();
	return 0;
}
