#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#define M_PI 3.1415928
#endif

#define USAGE	"[KEYBOARD]\n"\
				"ESC:\tQuit\n"\
				"RIGHT\tChange object\n"\
				"LEFT:\tChange object\n"\
				"UP:\tIncrease parallax\n"\
				"DOWN:\tDecrease parallax\n"\
				"r:\tReset\n"\
				"t:\tTexture ON/OFF\n"\
				"l:\tLight ON/OFF\n"\
				"w:\tFill/Wireframe\n"\
				"s:\tSmooth/Flat shade\n"\
				"b:\tBenchMark Mode ON/OFF\n"\
				"n:\tDisplay Rendered Frame Num ON/OFF\n"\
				"c:\tDisplay Frame Count ON/OFF\n"\
				"a:\tReset Frame Count\n"\
				"z:\tScale Up\n"\
				"Z:\tScale Down\n"\
				"SPACE:\tFullscreen ON/OFF\n"\
				"\n"\
				"[Mouse]\n"\
				"Left:\tMove\n"\
				"Middle:\tRotate\n"\
				"Right:\tScale\n"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <GL/glut.h>

#include "trackball.h"

#define CUBE			0
#define SPHERE			1
#define CONE			2
#define TORUS			3
#define DODECAHEDRON	4
#define OCTAHEDRON		5
#define TETRAHEDRON		6
#define ICOSAHEDRON		7
#define TEAPOT			8
#define NUM_OBJECTS		9

GLint object = TEAPOT;

GLboolean  stereo       = false;
GLboolean  showFPS      = false;
GLboolean  showFrameNum = true;
GLboolean  enableCount  = true;
GLboolean  showinfo     = false;
GLboolean  fullscreen   = false;

GLdouble   pan_x = 0.0;
GLdouble   pan_y = 0.0;
GLdouble   pan_z = 0.0;
GLdouble   scale = 1.0;
GLint      mouse_pos[2];
GLint      mouse_state = -1;
GLint      mouse_button = -1;

GLint      screen_width = 0;
GLint      screen_height = 0;
GLdouble   parallax = 0.1;
GLdouble   distance = 3.0;
GLdouble   zNear = 0.01;
GLdouble   zFar = 7.5;
GLdouble   fovy = 60.0;

char title[] = "OpenGL Test Application";
char texture_filename[512] = "spheremap.raw";
GLuint texture = 0;


GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat shininess[] = { 128.0 };

int bBenchMark = false;
GLdouble theta = 0.0;

int frameNum = 0;


void printCopyright()
{
	printf("%s\n", USAGE);
}

void toggleFullscreen()
{
	::glutHideWindow();

#ifdef WIN32
	static int x, y, w, h;
	if (fullscreen) {
		::glutPositionWindow(x, y);
		::glutReshapeWindow(w, h);
	} else {
		x = ::glutGet(GLUT_WINDOW_X);
		y = ::glutGet(GLUT_WINDOW_Y);
		w = ::glutGet(GLUT_WINDOW_WIDTH);
		h = ::glutGet(GLUT_WINDOW_HEIGHT);
		::glutPositionWindow(GetSystemMetrics(SM_XVIRTUALSCREEN), GetSystemMetrics(SM_YVIRTUALSCREEN));
		::glutReshapeWindow(GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
	}
#else
	static int x, y, w, h;
	if (fullscreen) {
		::glutPositionWindow(x, y);
		::glutReshapeWindow(w, h);
	} else {
		x = ::glutGet(GLUT_WINDOW_X);
		y = ::glutGet(GLUT_WINDOW_Y);
		w = ::glutGet(GLUT_WINDOW_WIDTH);
		h = ::glutGet(GLUT_WINDOW_HEIGHT);
		::glutFullScreen();
	}
#endif

	::glutShowWindow();
	::glutPostRedisplay();	
	fullscreen = !fullscreen;
}

#define TEXWIDTH  256
#define TEXHEIGHT 256
GLubyte pixels[TEXHEIGHT][TEXWIDTH][3];
void initTexture(void)
{
	FILE *fp = 0;
	if ((fp = ::fopen(texture_filename, "rb")) != NULL) {
		::fread(pixels, sizeof(pixels), 1, fp);
		::fclose(fp);
	} else {
		::fprintf(stderr, "WARNING: Can not read texture file, %s\n", texture_filename);
	}
}

void
initialize(void)
{
	tbInit(GLUT_MIDDLE_BUTTON);

	::glEnable(GL_LIGHTING);
	::glEnable(GL_LIGHT0);
	::glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	::glEnable(GL_DEPTH_TEST);

//	glEnable(GL_CULL_FACE);

	initTexture();
	::glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	::glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXWIDTH, TEXHEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	::glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	::glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	::glEnable(GL_TEXTURE_GEN_S);
	::glEnable(GL_TEXTURE_GEN_T);
	::glEnable(GL_TEXTURE_2D);

	::glColor3f(1, 1, 1);

	::glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	::glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void finalize()
{
	if (texture) {
		::glDeleteTextures(1, &texture);
	}
}

void drawText(float x, float y, char *str)
{
	::glMatrixMode(GL_PROJECTION);
	::glPushMatrix();
	::glLoadIdentity();

	::glMatrixMode(GL_MODELVIEW);
	::glPushMatrix();
	::glLoadIdentity();

	int bTextureEnabled = glIsEnabled(GL_TEXTURE_2D);
	::glDisable(GL_TEXTURE_2D);

	::glColor3f(1.0, 1.0, 1.0);
	::glDisable(GL_LIGHTING);
	::glRasterPos2f(x, y);
	for (char *c = str; *c != '\0'; c++) {
		::glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
	::glEnable(GL_LIGHTING);
	::glPopMatrix();

	if (bTextureEnabled) {
		glEnable(GL_TEXTURE_2D);
	}

	::glMatrixMode(GL_PROJECTION);
	::glPopMatrix();
	::glMatrixMode(GL_MODELVIEW);
}

void drawFPS()
{
	static int frame = 0;
	static int lastupdate = 0;
	static char str[32];

	frame++;
	int current = glutGet(GLUT_ELAPSED_TIME);
	if (current - lastupdate > 3000) {
		float fps = 1000.0f * frame / (current - lastupdate);
		lastupdate = current;
		frame = 0;
		sprintf(str, "%.2f fps", fps);
	}
	if (showFPS) {
		drawText(-0.95, -0.95, str);
	}
}

void drawFrameNum()
{

	char       frameNumStr[128];

	if (enableCount) frameNum++;
	sprintf(frameNumStr, "%d frames", frameNum);

	if (showFrameNum){
		drawText(-0.95, -0.88, frameNumStr);
	}

}

void
reshape(int width, int height)
{
	screen_width = width;
	screen_height = height;

	tbReshape(width, height);

	::glViewport(0, 0, width, height);

	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	::gluPerspective(fovy, (GLfloat)width/height, zNear, zFar);
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();
}


void drawObject(int n)
{
	::glScalef(scale, scale, scale);
	switch (n) {
		case CUBE:
			::glutSolidCube(1.5);
			break;
		case SPHERE:
			::glutSolidSphere(0.8, 64, 64);
			break;
		case CONE:
			::glPushMatrix();
			::glRotatef(-90.0, 1, 0, 0);
			::glutSolidCone(1.0, 1.0, 32, 16);
			::glPopMatrix();
			break;
		case TORUS:
			::glutSolidTorus(0.35, 0.75, 16, 32);
			break;
		case DODECAHEDRON:
			::glPushMatrix();
			::glScalef(0.5, 0.5, 0.5);
			::glutSolidDodecahedron();
			::glPopMatrix();
			break;
		case OCTAHEDRON:
			::glutSolidOctahedron();
			break;
		case TETRAHEDRON:
            ::glutSolidTetrahedron();
			break;
		case ICOSAHEDRON:
			::glutSolidIcosahedron();
			break;
		case TEAPOT:
			::glFrontFace(GL_CW);
			::glutSolidTeapot(1.0);
			::glFrontFace(GL_CCW);
			break;

	}
}


void
display(void)
{
	if (stereo) {
		GLdouble k = 0.5 * zNear / distance;
		GLdouble f = parallax * k;
		GLdouble h = zNear * tan(3.1415927 * fovy / 360.0);
		GLdouble w = h / screen_height * screen_width;
		GLfloat transform[4][4];

		// RIGHT
		glDrawBuffer(GL_BACK_RIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-w - f, w - f, -h, h, zNear, zFar);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslated(-parallax * 0.5, 0.0, -distance);
		
		glPushMatrix();
		if (bBenchMark) {
			glRotatef(theta, 1, 1, 1);
		} else {
			glTranslatef(pan_x, pan_y, pan_z);
			tbGetMatrix((GLfloat *)transform);
			tbMatrix();
		}
		drawObject(object);
		glPopMatrix();

		// LEFT
		glDrawBuffer(GL_BACK_LEFT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-w + f, w + f, -h, h, zNear, zFar);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslated(parallax * 0.5, 0.0, -distance);
		glPushMatrix();
		if (bBenchMark) {
			glRotatef(theta, 1, 1, 1);
		} else {
			glTranslatef(pan_x, pan_y, pan_z);
			tbSetMatrix((GLfloat *)transform);
			tbMatrix();
		}
		drawObject(object);	
		glPopMatrix();

	} else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslated(0.0, 0.0, -distance);		

		glPushMatrix();

		if (bBenchMark) {
			glRotatef(theta, 1, 1, 1);
		} else {
			glTranslatef(pan_x, pan_y, pan_z);
			tbMatrix();
		}

		drawObject(object);
		glPopMatrix();
	}
	drawFPS();
	drawFrameNum();

	glutSwapBuffers();
	glutPostRedisplay();

	if (bBenchMark) {
		theta += 1.0;
	}

}

void
keyboard(unsigned char key, int x, int y)
{
	int polygon_mode[2];
	int shade_model;
	switch (key) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
			object = key - '0';
			break;
		case 'r':
			pan_x = pan_y = pan_z = 0.0f;
			scale = 1.0f;
			object = TEAPOT;
			parallax = 0.1;
			tbInit(GLUT_MIDDLE_BUTTON);
			glutPostRedisplay();
			break;
		case 'q':
		case 27:
			finalize();
			exit(0);
			break;
		case 'f':
//			showFPS = !showFPS;
			break;
		case ' ':
			toggleFullscreen();
			break;
		case 'w':
			glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
			if (polygon_mode[0] == GL_FILL) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			} else {
	   		    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			break;
		case 't':
			if (glIsEnabled(GL_TEXTURE_2D)) {
				glDisable(GL_TEXTURE_2D);
				diffuse[0] = diffuse[1] = diffuse[2] = 0.75;

			} else {
				glEnable(GL_TEXTURE_2D);
				diffuse[0] = diffuse[1] = diffuse[2] = 1.0;
			}
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			break;
		case 'l':
			if (glIsEnabled(GL_LIGHTING)) {
				glDisable(GL_LIGHTING);
			} else {
				glEnable(GL_LIGHTING);
			}
			break;
		case 's':
			glGetIntegerv(GL_SHADE_MODEL, &shade_model);
			if (shade_model == GL_SMOOTH) {
				glShadeModel(GL_FLAT);
			} else {
				glShadeModel(GL_SMOOTH);
			}
			break;
		case 'b':
			bBenchMark = !bBenchMark;
			if (bBenchMark) {
				showFPS = true;
				theta = 0.0;
				scale = 1.0f;
			} else {
				showFPS = false;
			}
			break;
		case 'n':
			showFrameNum = !showFrameNum;
			break;
		case 'c':
			enableCount = !enableCount;
			break;
		case 'a':
			frameNum = 0;
			break;
		case 'z':
			if (scale < 0.5f) {
				scale = 0.0f;
			}
			scale += 0.5f;
			if (scale > 2.5f) {
				scale = 2.5f;
			}
			break;
		case 'Z':
			scale -= 0.5f;
			if (scale < 0.1f) {
				scale = 0.1f;
			}
			break;

	}

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	switch (key) {
		case GLUT_KEY_UP:
			parallax += 0.1;
			break;
		case GLUT_KEY_DOWN:
			parallax -= 0.1;
			if (parallax < 0) {
				parallax = 0.0;
			}
			break;
		case GLUT_KEY_LEFT:
			object--;
			if (object < 0) {
				object = NUM_OBJECTS - 1;
			}
			break;
		case GLUT_KEY_RIGHT:
			object++;
			if (object >= NUM_OBJECTS) {
				object = 0;
			}
			break;
	}
	glutPostRedisplay();
}

void
menu(int item)
{
	keyboard((unsigned char)item, 0, 0);
}

void
mouse(int button, int state, int x, int y)
{
	GLdouble model[4*4];
	GLdouble proj[4*4];
	GLint view[4];

	if (bBenchMark) {
		return;
	}

	tbMouse(button, state, x, y);

	mouse_state = state;
	mouse_button = button;

	mouse_pos[0] = x;
	mouse_pos[1] = y;

	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) {
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		gluProject((GLdouble)x, (GLdouble)y, 0.0,
			model, proj, view,
			&pan_x, &pan_y, &pan_z);
		gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
			model, proj, view,
			&pan_x, &pan_y, &pan_z);
		pan_y = -pan_y;
	}

	glutPostRedisplay();
}

void
motion(int x, int y)
{
	GLdouble model[4*4];
	GLdouble proj[4*4];
	GLint view[4];

	if (bBenchMark) {
		return;
	}

	tbMotion(x, y);

	if (mouse_state == GLUT_DOWN && mouse_button == GLUT_LEFT_BUTTON) {
		glGetDoublev(GL_MODELVIEW_MATRIX, model);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		glGetIntegerv(GL_VIEWPORT, view);
		gluProject((GLdouble)x, (GLdouble)y, 0.0,
			model, proj, view,
			&pan_x, &pan_y, &pan_z);
		gluUnProject((GLdouble)x, (GLdouble)y, pan_z,
			model, proj, view,
			&pan_x, &pan_y, &pan_z);
		pan_y = -pan_y;
	} else if (mouse_state == GLUT_DOWN && mouse_button == GLUT_RIGHT_BUTTON) {
		scale -= (float) (mouse_pos[1] - y) / 100.0f;
		if (scale < 0.1f) {
			scale = 0.1f;
		} else if (scale > 2.5f) {
			scale = 2.5f;
		}
	}
	mouse_pos[0] = x;
	mouse_pos[1] = y;

	glutPostRedisplay();
}

void idle()
{
	if (bBenchMark) {
		glutPostRedisplay();
	}
}

int
main(int argc, char** argv)
{
	printCopyright();

	if (argc > 1) {
		strcpy(texture_filename, argv[1]);
	}

	glutInitWindowSize(512, 512);
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STEREO);
	stereo = true;
	if (!glutGet(GLUT_DISPLAY_MODE_POSSIBLE)) {
		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
		stereo = false;
		fprintf(stderr, "INFO: Stereo is not supported.\n");
	} else {
		fprintf(stderr, "INFO: Stereo is supported.\n");
	}

	glutCreateWindow(title);

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(special);
//	glutIdleFunc(idle);
	initialize();

	glutMainLoop();
	return 0;
}

