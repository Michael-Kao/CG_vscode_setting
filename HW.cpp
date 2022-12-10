/*---------------------------------------------------------------------------------
 * This file contains a program, drawing & animating a wind-mill.
 * This program is used to teach LCS and scene graph concepts.
 *
 * by S. K. Ueng, Nov.  2006
 */
#include <GL/glut.h>
#include <bits/stdc++.h>

using namespace std;

enum MODE
{
    STOP,
    WALK,
    JUMP,
    GESTURE
};

enum WEAPON
{
    NONE,
    SWORD,
    LANCE
};

WEAPON weapon = NONE;
MODE mode = STOP;

/*-----Define a unit box--------*/
/* Vertices of the box */
float points[][3] = {{0.0, 0.0, 0.0}, {1.5, 0.0, 0.0}, {1.5, 1.5, 0.0}, {0.0, 1.5, 0.0}, {0.0, 0.0, 2.5}, {1.5, 0.0, 2.5}, {1.5, 1.5, 2.5}, {0.0, 1.5, 2.5}};
/* face of box, each face composing of 4 vertices */
int face[][4] = {{0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3}};
/* indices of the box faces */
int cube[6] = {0, 1, 2, 3, 4, 5};

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
GLUquadricObj *sphere = NULL, *cylind = NULL;

/*-----Define window size----*/
int width = 800, height = 800;

// Global variables recording rotation angles of H-arm and blades.
float arm_ang = 45.0, bld_ang = 10.0;

double Pi = 3.1415926;
float view_x = 1.0, view_y = 1.0, view_z = 1.0;
float test_x = 0.0, test_y = 0.0, test_z = 0.0;
int ges_fore = 0, gesture = 0, spin = 0, ang_x = 0, ang_y = 0, ang_z = 0, r_x = 0, r_y = 0, r_z = 0, Lfa_ang = 0, Rfa_ang = 0, La_ang = 0, Ra_ang = 0, r_calf = 0, l_calf = 0, r_leg = 0, l_leg = 0, angle_speed = 1, counter = 0, turn_ang = 0;
bool ges = false, D = false, one_side_move = false, jump = false, stop = true, crouch = false, run = false, hold = false, right_hand = true;
float run_speed = 1.0;
float crouch_down = 0.0, calf_down = 0.0, leg_down = 0.0; // no use
int jump_s = 0;

/*-----Translation and rotations of eye coordinate system---*/
float eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double Eye[3] = {0.0, 0.0, 30.0}, Focus[3] = {0.0, 0.0, 0.0},
	   Vup[3] = {0.0, 1.0, 0.0};

float u[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
float eye[3];
float cv, sv; /* cos(5.0) and sin(5.0) */
float my_near = 10, my_far = 20.0;
float ntop, nleft, nright, nbottom;
float nzoom[4][4] = {{-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}, {-40.0, 40.0, -40.0, 40}};
int zoom_mode = 1;

/* ----Drawing syle----*/
int style = 0;

void draw_sword();
void draw_lance();
void draw_stuff();

/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void myinit()
{
    glClearColor(0.0, 0.0, 0.0, 1.0); /*set the background color BLACK */
    /*Clear the Depth & Color Buffers */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, 499, 499);

    /*-----Set a parallel projection mode-----*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-40.0, 40.0, -40, 40, 0.0, 120.0);

    glEnable(GL_DEPTH_TEST); /*Enable depth buffer for shading computing */

    if (cylind == NULL)
    { /* allocate a quadric object, if necessary */
        cylind = gluNewQuadric();
        gluQuadricDrawStyle(cylind, GLU_FILL);
        gluQuadricNormals(cylind, GLU_SMOOTH);
    }

    if (sphere == NULL)
    {
        sphere = gluNewQuadric();
        gluQuadricDrawStyle(sphere, GLU_FILL);
    }

    cv = cos(5.0 * Pi / 180.0);
    sv = sin(5.0 * Pi / 180.0);

    eye[0] = Eye[0];
    eye[1] = Eye[1];
    eye[2] = Eye[2];

    float h_near, w_near;
    h_near = 2 * tan(90.0 / 2 * Pi / 180.0) * my_near;
    w_near = h_near * width / height;
    nleft = -w_near / 2;
    nright = w_near / 2;
    nbottom = -h_near / 2;
    ntop = h_near / 2;


    glFlush(); /*Enforce window system display the results*/
}

void draw_viewvolume2(){
    // glTranslatef(eyeDx, eyeDy, eyeDz);
    float fleft, fright, ftop, fbottom, nl, nr, nt, nb;
    // nl = nleft + eye[0];
    // nr = nright + eye[0];
    // nt = ntop + eye[1];
    // nb = nbottom + eye[1];
    // fleft = nleft * (my_far / my_near) + eye[0];
    // fright = nright * (my_far / my_near) + eye[0];
    // ftop = ntop * (my_far / my_near) + eye[1];
    // fbottom = nbottom * (my_far / my_near) + eye[1];

    nl = nleft;
    nr = nright;
    nt = ntop ;
    nb = nbottom ;
    fleft = nleft * (my_far / my_near) ;
    fright = nright * (my_far / my_near) ;
    ftop = ntop * (my_far / my_near) ;
    fbottom = nbottom * (my_far / my_near) ;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nl, nt, - my_near);
    glVertex3f(nr, nt, - my_near);
    glEnd();
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nl, nb, - my_near);
    glVertex3f(nl, nt, - my_near);
    glEnd();
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nr, nb, - my_near);
    glVertex3f(nl, nb, - my_near);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(nr, nb, - my_near);
    glVertex3f(nr, nt, - my_near);
    glEnd();
    

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fleft, ftop, - my_far);
    glVertex3f(fright, ftop, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fleft, fbottom, - my_far);
    glVertex3f(fleft, ftop, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fright, fbottom, - my_far);
    glVertex3f(fleft, fbottom, - my_far);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(fright, fbottom, - my_far);
    glVertex3f(fright, ftop, - my_far);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


}

/*-------------------------------------------------------
 * Make viewing matrix
 */
void make_view(int x)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (x)
	{
	case 4: /* Perspective */

		/* In this sample program, eye position and Xe, Ye, Ze are computed
		   by ourselves. Therefore, use them directly; no trabsform is
		   applied upon eye coordinate system
		   */
		gluLookAt(eye[0], eye[1], eye[2],
				  eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
				  u[1][0], u[1][1], u[1][2]);
		break;

	case 1: /* X direction parallel viewing */
		gluLookAt(30.0, 0.0, 0.0, Focus[0], Focus[1], Focus[2], 0.0, 1.0, 0.0);
		break;
	case 2: /* Y direction parallel viewing */
		gluLookAt(0.0, 30.0, 0.0, Focus[0], Focus[1], Focus[2], 1.0, 0.0, 0.0);
		break;
	case 3:
		gluLookAt(0.0, 0.0, 30.0, Focus[0], Focus[1], Focus[2], 0.0, 1.0, 0.0);
		break;
	}
}

/*------------------------------------------------------
 * Procedure to make projection matrix
 */
void make_projection(int x)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (x == 4)
	{
        // if(zoom_mode){
        //     gluPerspective(90.0, (double)width / (double)height, my_near, 50.0);
        // }
        // else{
            // nleft += eye[0];
            // nright += eye[0];
            // ntop += eye[1];
            // nbottom += eye[1];
            glFrustum(nleft + eye[0], nright + eye[0], nbottom + eye[1], ntop + eye[1], 10, 20);
        // }
	}
	else
	{
        if (width > height)
            glOrtho(nzoom[x][0], nzoom[x][1], nzoom[x][2] * (float)height / (float)width,
                    nzoom[x][3] * (float)height / (float)width,
                    -0.0, 100.0);
        else
            glOrtho(nzoom[x][0] * (float)width / (float)height,
                    nzoom[x][1] * (float)width / (float)height, nzoom[x][2], nzoom[x][3],
                    -0.0, 100.0);
    }
	glMatrixMode(GL_MODELVIEW);
}

/*---------------------------------------------------------
 * Procedure to draw view volume, eye position, focus ,...
 * for perspective projection
 */
void draw_view()
{

	glMatrixMode(GL_MODELVIEW);

	/*----Draw Eye position-----*/
	glPushMatrix();
	glTranslatef(eye[0], eye[1], eye[2]);
	glColor3f(0.0, 1.0, 0.0);
	glutWireSphere(1.0, 10, 10);
	glPopMatrix();

	/*----Draw eye coord. axes -----*/
	glColor3f(1.0, 1.0, 0.0); /* Draw Xe */ //yellow
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[0][0], eye[1] + 20.0 * u[0][1], eye[2] + 20.0 * u[0][2]);
	glEnd();

	glColor3f(1.0, 0.0, 1.0); /* Draw Ye */ // purple
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[1][0], eye[1] + 20.0 * u[1][1], eye[2] + 20.0 * u[1][2]);
	glEnd();

	glColor3f(0.0, 1.0, 1.0); /* Draw Ze */  // cyan
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0 * u[2][0], eye[1] + 20.0 * u[2][1], eye[2] + 20.0 * u[2][2]);
	glEnd();

    glPushMatrix();
    glTranslatef(-eyeDx, -eyeDy, eye[2]);
    glRotatef(-eyeAngx, 1.0, 0.0, 0.0);
    glRotatef(eyeAngy, 0.0, 1.0, 0.0);
    glRotatef(-eyeAngz, 0.0, 0.0, 1.0);
    draw_viewvolume2();
    glPopMatrix();
}

/*-------------------------------------------------------
 * Procedure to draw three axes and the orign
 */
void draw_axes()
{

    /*----Draw a white sphere to represent the original-----*/
    glColor3f(0.9, 0.9, 0.9);

    gluSphere(sphere, 2.0, /* radius=2.0 */
              12,          /* composing of 12 slices*/
              12);         /* composing of 8 stacks */

    /*----Draw three axes in colors, yellow, meginta, and cyan--*/
    /* Draw Z axis  */
    glColor3f(0.0, 0.95, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
                10.0,             /* height of the cylinder */
                12,               /* use 12-side polygon approximating circle*/
                3);               /* Divide it into 3 sections */

    /* Draw Y axis */
    glPushMatrix();
    glRotatef(-90.0, 1.0, 0.0, 0.0); /*Rotate about x by -90', z becomes y */
    glColor3f(0.95, 0.0, 0.95);
    gluCylinder(cylind, 0.5, 0.5, /* radius of top and bottom circle */
                10.0,             /* height of the cylinder */
                12,               /* use 12-side polygon approximating circle*/
                3);               /* Divide it into 3 sections */
    glPopMatrix();

    /* Draw X axis */
    glColor3f(0.95, 0.95, 0.0);
    glPushMatrix();
    glRotatef(90.0, 0.0, 1.0, 0.0); /*Rotate about y  by 90', x becomes z */
    gluCylinder(cylind, 0.5, 0.5,   /* radius of top and bottom circle */
                10.0,               /* height of the cylinder */
                12,                 /* use 12-side polygon approximating circle*/
                3);                 /* Divide it into 3 sections */
    glPopMatrix();
    /*-- Restore the original modelview matrix --*/
    glPopMatrix();
}

/*--------------------------------------------------------
 * Procedure to draw a cube. The geometrical data of the cube
 * are defined above.
 */
void draw_ret()
{
    int i;

    glColor3f(1.0, 0.0, 1.0); /* Set the current color */
    for (i = 0; i < 6; i++)
    {
        glBegin(GL_POLYGON); /* Draw the face */
        glVertex3fv(points[face[i][0]]);
        glVertex3fv(points[face[i][1]]);
        glVertex3fv(points[face[i][2]]);
        glVertex3fv(points[face[i][3]]);
        glEnd();
    }
}

/*------------------------------------------------------------
 * Procedure to draw a floor of black-and-white tiles.
 */
void draw_floor()
{
    int i, j;

    for (i = -100; i < 100; i++)
        for (j = -100; j < 100; j++)
        {
            // if ((i + j) % 2 == 0) glColor3f(0.9, 0.9, 0.9);
            /*else*/ 
            glColor3f(0.3, 0.1, 0.1);
            // glColor3f(0.0, 0.0, 0.0);
            glBegin(GL_POLYGON);
            glVertex3f(i, 0.0, j);
            glVertex3f(i, 0.0, j + 1);
            glVertex3f(i + 1, 0.0, j + 1);
            glVertex3f(i + 1, 0.0, j);
            glEnd();
        }
}

// change degree to radian
double DEG2RAD(int deg)
{
    double x;
    x = deg * Pi / 180;
    return x;
}

// caculate how height we reduce by crouching //not using now
void caldown_func()
{
    float leg = 2.5, calf = 6.0;
    double l_deg, c_deg, tmp_l, tmp_c;
    l_deg = DEG2RAD(r_leg * (-1));
    c_deg = DEG2RAD(r_calf);
    tmp_l = leg * sin(l_deg);
    tmp_c = calf * sin(c_deg);
    cout << "sin_leg = " << sin(l_deg) << " sin_calf = " << sin(c_deg) << endl;
    if (tmp_l > -1 && tmp_c > -1)
    {
        leg_down = leg - tmp_l;
        calf_down = calf - tmp_c;
        crouch_down = (leg_down + calf_down) * (-1);
        cout << "leg = " << leg_down << " calf = " << calf_down << " total = " << crouch_down << endl;
    }
}

// Draw robot
void draw_robo()
{
    // draw_floor();
    glPushMatrix();
    // draw obstacle
    glTranslatef(-5.0, 0.0, 36.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(10.0, 6.0, 6.0);
    glColor3f(0.0, 1.0, 0.0);
    draw_ret();
    glPopMatrix();

    glTranslatef(test_x, test_y, test_z);

    glPushMatrix();
    glRotatef(turn_ang, 0.0, 1.0, 0.0);

    glTranslatef(0.0, 5.0, 0.0);

    // draw body
    glPushMatrix(); //(0, 5, 0)
    glTranslatef(0.0, 5.0, 0.0);
    glRotatef(-90, 1.0, 0.0, 0.0); // 把z軸座標轉到-y方向
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glColor3f(0.0, 1.0, 0.0); /* Gray colored */
    gluCylinder(cylind, 4, 4, // 沿著z軸畫圓柱
                6,
                12,
                3);
    glPopMatrix(); // 回到(0, 5, 0)

    // draw right leg
    glPushMatrix();
    glTranslatef(-2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(r_leg, 1.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 0.0); // Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);
    // draw right knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); // Red color
    gluSphere(sphere, 1.5,    /* radius=2.0 */
              12,             /* composing of 12 slices*/
              12);            /* composing of 12 stacks */
    glPopMatrix();

    glRotatef(r_calf, 1.0, 0.0, 0.0);

    // draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run)
    {
        glRotatef(30, 1.0, 0.0, 0.0);
    }
    glColor3f(0.0, 0.0, 1.0); // Blue color
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);
    glPopMatrix();
    glPopMatrix();

    // draw left leg
    glPushMatrix();
    glTranslatef(2.0, 5.0, 0.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(-30, 1.0, 0.0, 0.0);
    }
    glRotatef(l_leg, 1.0, 0.0, 0.0);
    glColor3f(2.0, 1.0, 0.0); // Yello color
    gluCylinder(cylind, 1.5, 1.5, 2.5, 12, 3);
    // draw left knee
    glTranslatef(0.0, 0.5, 3.0);
    glPushMatrix();
    glColor3f(1.0, 0.0, 0.0); // Red color
    gluSphere(sphere, 1.5,    /* radius=2.0 */
              12,             /* composing of 12 slices*/
              12);            /* composing of 12 stacks */
    glPopMatrix();

    glRotatef(l_calf, 1.0, 0.0, 0.0);

    // draw calf
    glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    if (run)
    {
        glRotatef(30, 1.0, 0.0, 0.0);
    }
    glColor3f(0.0, 0.0, 1.0); // Blue color
    gluCylinder(cylind, 1.5, 1.5, 6.0, 12, 3);
    glPopMatrix();
    glPopMatrix();

    // draw head
    glPushMatrix();
    glTranslatef(0.0, 13.0, 0.0);

    if (run)
    {
        glTranslatef(0.0, 0.0, 1.0);
        glRotatef(10, 1.0, 0.0, 0.0);
    }

    glColor3f(1.0, 1.0, 1.0);
    glutSolidCube(6);

    glTranslatef(-1.5, 0.0, 3.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);

    glTranslatef(3.0, 0.0, 0.0);
    glutSolidSphere(1.0, 6, 6);

    glPopMatrix();

    // draw left shoulder
    glPushMatrix();
    glTranslatef(4.9, 10.0, 0.0);
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }
    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);

    glRotatef(La_ang, 1.0, 0.0, 0.0);
    glRotatef(gesture, 0.0, 0.0, 1.0);

    // draw left upper arm
    glPushMatrix();
    glTranslatef(-0.5, -1.4, -0.5);
    glRotatef(90, 1.0, 0.0, 0.0);
    draw_ret();
    // draw left elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);
    glPopMatrix();

    glRotatef(Lfa_ang, 1.0, 0.0, 0.0);
    glRotatef(ges_fore, 0.0, 0.0, 1.0);

    // draw left forearm
    glPushMatrix();
    glTranslatef(-0.6, -0.4, 0.125);
    if (run)
    {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }
    draw_ret();

    if (hold && !right_hand)
    {
        glTranslatef(0.75, 0.0, 2.5);
        if (weapon == SWORD)
            draw_sword();
        if (weapon == LANCE)
            draw_lance();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();

    // draw right shoulder
    glPushMatrix();
    glTranslatef(-4.9, 10.0, 0.0);
    if (run)
    {
        glRotatef(10, 1.0, 0.0, 0.0);
    }
    glColor3f(0.32, 0.32, 0.32);
    glutSolidCube(3);

    glRotatef(Ra_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * gesture, 0.0, 0.0, 1.0);

    // draw right upper arm
    glPushMatrix();
    glTranslatef(-1.4, -1.3, -0.8);
    glRotatef(90, 1.0, 0.0, 0.0);
    draw_ret();

    // draw right elbow
    glTranslatef(0.5, 0.4, 3.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glColor3f(0.0, 1.0, 1.0);
    gluSphere(sphere, 0.5, 12, 12);
    glPopMatrix();

    // rotate forearm
    glRotatef(Rfa_ang, 1.0, 0.0, 0.0);
    glRotatef((-1) * ges_fore, 0.0, 1.0, 0.0);

    // draw right forearm
    glPushMatrix();
    glTranslatef(-0.5, -0.4, 0.125);
    if (run)
    {
        glTranslatef(0.0, 0.0, 0.375);
        glRotatef(-60, 1.0, 0.0, 0.0);
    }
    draw_ret();

    if (hold && right_hand)
    {
        glTranslatef(0.75, 0.0, 2.5);
        if (weapon == SWORD)
            draw_sword();
        if (weapon == LANCE)
            draw_lance();
    }

    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

// draw sword
void draw_sword()
{

    if (!hold || weapon != SWORD)
    {
        glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(2.0, 2.0, 2.0);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);

    gluCylinder(cylind, 0.5, 0.5, // 沿著z軸畫圓柱
                2,
                12,
                3);

    glPushMatrix();
    glTranslatef(0.0, 0.0, 2.5);
    glScalef(3.0, 1.0, 1.0);
    glutWireCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 5.5);
    glScalef(2.0, 0.1, 5.0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, 8.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(/*sqrt(3) - 1.0*/ 0.0, 0.0, 1.0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

// draw lance
void draw_lance()
{

    if (!hold || weapon != LANCE)
    {
        glRotatef(spin, 0.0, 1.0, 0.0);
        glScalef(1.5, 1.5, 1.5);
    }

    glPushMatrix();
    glRotatef(-90, 1.0, 0.0, 0.0);
    glColor3f(0.5, 0.0, 1.0);
    gluCylinder(cylind, 0.5, 0.5, 10.0, 12, 3);

    glTranslatef(0.0, 0.0, 10.0);

    glPushMatrix();
    glColor3f(1.0, 0.5, 0.0);
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0, -1.0, 0.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex3f(-1.0, -1.0, 0.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 3.0);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

void draw_stuff(){
    draw_axes();
    glPushMatrix();
    glTranslatef(-30.0, 0.0, 0.0);
    draw_sword();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30.0, 0.0, 10.0);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(-10.0, 0.0, 0.0);
    glVertex3f(10.0, 0.0, 0.0);
    glEnd();
    glTranslatef(10.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, -20.0);
    glVertex3f(0.0, 0.0, 20.0);
    glEnd();
    glTranslatef(0.0, 0.0, -20.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glTranslatef(0.0, 0.0, 40.0);
    glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(-20.0, 0.0, 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-30.0, 0.0, 20.0);
    draw_lance();
    glPopMatrix();


}
/*draw scene*/
void draw_scene(){
    draw_stuff();

    glPushMatrix();
    draw_robo();
    glPopMatrix();
}
/*-------------------------------------------------------
 * Display and Idle callback func. This func. draws three
 * cubes at proper places to simulate a solar system.
 */
void display()
{
    /*Clear previous frame and the depth buffer */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*----Define the current eye position and the eye-coordinate system---*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); /* Initialize modelview matrix */

    /*-------Draw the base of the windmill which is a cube----*/
    switch (style)
    {
    case 0:
        make_view(4);
        make_projection(4);
        glViewport(width / 2, 0, width / 2, height / 2);
        draw_scene();

        make_view(1);
        make_projection(1);
        glViewport(0, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(1);
        draw_view();

        make_view(2);
        make_projection(2);
        glViewport(width / 2, height / 2, width / 2, height / 2);
        draw_scene();
        make_view(2);
        draw_view();

        make_view(3);
        make_projection(3);
        glViewport(0, 0, width / 2, height / 2);
        draw_scene();
        make_view(3);
        draw_view();
        break;
    case 1:
        make_view(1);
        make_projection(1);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(1);
        draw_view();
        break;
    case 2:
        make_view(2);
        make_projection(2);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(2);
        draw_view();
        break;
    case 3:
        make_view(3);
        make_projection(3);
        glViewport(0, 0, width, height);
        draw_scene();
        make_view(3);
        draw_view();
        break;
    case 4:
        make_view(4);
        make_projection(4);
        glViewport(0, 0, width, height);
        draw_scene();
        break;
    }
    /*-------Swap the back buffer to the front --------*/
    glutSwapBuffers();
    glFlush(); /*--Display the results----*/
}

void zoom(unsigned char key){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch(style){
        case 0:
            if(key == 'k'){
                float factor = 1.2;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            else{
                float factor = 5.0 / 6.0;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            break;
        case 1:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[1][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[1][i] *= factor;
                }
            }
            break;
        case 2:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[2][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[2][i] *= factor;
                }
            }
            break;
        case 3:
            if(key == 'k'){
                float factor = 1.2;
                for (int i = 0; i < 4; i++){
                    nzoom[3][i] *= factor;
                }
            }
            else{
                float factor = 5.0 / 6.0;
                for (int i = 0; i < 4; i++){
                    nzoom[3][i] *= factor;
                }
            }
            break;
        case 4:
            if(key == 'k'){
                float factor = 1.2;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            else{
                float factor = 5.0 / 6.0;
                nleft *= factor;
                nright *= factor;
                ntop *= factor;
                nbottom *= factor;
            }
            break;
    }
    glMatrixMode(GL_MODELVIEW);
}

/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{
    width = w;
    height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (double)w / (double)h, 1.5, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int ix, int iy)
{
    if (key == ' ')
    {
        mode = JUMP;
        jump = true;
        if (jump_s < 5)
        {
            jump_s++;
        }
    }
    if (key == 27)
        exit(0);
    if (key == '.')
    {
        if (!crouch)
        {
            test_y = -2.3;
            r_leg = l_leg = -45;
            r_calf = l_calf = 90;
            crouch = true;
        }
        else
        {
            test_y = 0.0;
            r_leg = l_leg = 0;
            r_calf = l_calf = 0;
            crouch = false;
        }
    }
    if (key == 'p')
    {
        hold = false;
        weapon = NONE;
    }
    if (key == 'g')
    {
        mode = GESTURE;
        ges = true;
    }
    if (key == 'o')
        right_hand = !right_hand;
    if (key == 'w')
        mode = WALK;
    if (key == 's')
        mode = STOP;
    if (key == 'r')
    {
        run = true;
        if (run_speed < 3.0)
        {
            run_speed += 0.4;
        }
    }
    // if (key == 'd' || key == 'D')
    // {
    //     D = (D == true) ? false : true;
    //     cout << "status: " << D << endl;
    // }
    // if (D)
    // {
    //     if (key == 'q')
    //     {
    //         if ((La_ang - 1) < -180)
    //             return;
    //         La_ang = (La_ang - 1) % 360;
    //     }
    //     if (key == 'e')
    //     {
    //         if ((Ra_ang - 1) < -180)
    //             return;
    //         Ra_ang = (Ra_ang - 1) % 360;
    //     }
    //     if (key == 'z')
    //     {
    //         if ((Lfa_ang - 1) < -120)
    //             return;
    //         Lfa_ang = (Lfa_ang - 1) % 360;
    //     }
    //     if (key == 'c')
    //     {
    //         if ((Rfa_ang - 1) < -120)
    //             return;
    //         Rfa_ang = (Rfa_ang - 1) % 360;
    //     }
    //     if (key == '1')
    //         test_x -= 0.1;
    //     if (key == '2')
    //         test_y -= 0.1;
    //     if (key == '3')
    //         test_z -= 0.1;
    //     if (key == '4')
    //         ang_x = (ang_x - 1) % 360;
    //     if (key == '5')
    //         ang_y = (ang_y - 1) % 360;
    //     if (key == '6')
    //         ang_z = (ang_z - 1) % 360;
    //     if (key == '7')
    //         r_x = (r_x - 5) % 360;
    //     if (key == '8')
    //         r_y = (r_y - 5) % 360;
    //     if (key == '9')
    //         r_z = (r_z - 5) % 360;
    // }
    // else
    // {
    //     if (key == 'q')
    //     {
    //         if ((La_ang + 1) > 30)
    //             return;
    //         La_ang = (La_ang + 1) % 360;
    //     }
    //     if (key == 'e')
    //     {
    //         if ((Ra_ang + 1) > 30)
    //             return;
    //         Ra_ang = (Ra_ang + 1) % 360;
    //     }
    //     if (key == 'z')
    //     {
    //         if ((Lfa_ang + 1) > 0)
    //             return;
    //         Lfa_ang = (Lfa_ang + 1) % 360;
    //     }
    //     if (key == 'c')
    //     {
    //         if ((Rfa_ang + 1) > 0)
    //             return;
    //         Rfa_ang = (Rfa_ang + 1) % 360;
    //     }
        // if(key == '0')
        //     style = 0;
        // if (key == '1')
        //     style = 1;
        // if (key == '2')
        //     style = 2;
        // if (key == '3')
        //     style = 3;
        // if (key == '4')
        //     style = 4;
        // if (key == '5')
        //     ang_y = (ang_y + 1) % 360;
        // if (key == '6')
        //     ang_z = (ang_z + 1) % 360;
        // if (key == '7')
        //     r_x = (r_x + 5) % 360;
        // if (key == '8')
        //     r_y = (r_y + 5) % 360;
        // if (key == '9')
        //     r_z = (r_z + 5) % 360;
    // }
    if (key == '0')
        style = 0;
    if (key == '1')
        style = 1;
    if (key == '2')
        style = 2;
    if (key == '3')
        style = 3;
    if (key == '4')
        style = 4;
    // Transform the EYE coordinate system
    int i;
    float x[3], y[3], z[3];

    if (key == 'u')
	{
		eyeDy += 0.5; /* move up */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[1][i];
	}
	else if (key == 'd')
	{
		eyeDy += -0.5; /* move down */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[1][i];
	}
	else if (key == 'W')
	{
		eyeDx += -0.5; /* move left */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[0][i];
	}
	else if (key == 'e')
	{
		eyeDx += 0.5; /* move right */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[0][i];
	}
	else if (key == 'n')
	{
		eyeDz += 0.5; /* zoom in */
		for (i = 0; i < 3; i++)
			eye[i] -= 0.5 * u[2][i];
	}
	else if (key == 'f')
	{
		eyeDz += -0.5; /* zoom out */
		for (i = 0; i < 3; i++)
			eye[i] += 0.5 * u[2][i];
	}
	else if (key == 'p')
	{ /* pitching 對X軸旋轉 */ 
		eyeAngx += 5.0;
		if (eyeAngx > 360.0)
			eyeAngx -= 360.0;
		y[0] = u[1][0] * cv - u[2][0] * sv;
		y[1] = u[1][1] * cv - u[2][1] * sv;
		y[2] = u[1][2] * cv - u[2][2] * sv;

		z[0] = u[2][0] * cv + u[1][0] * sv;
		z[1] = u[2][1] * cv + u[1][1] * sv;
		z[2] = u[2][2] * cv + u[1][2] * sv;

		for (i = 0; i < 3; i++)
		{
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'P')
	{
		eyeAngx += -5.0;
		if (eyeAngx < 0.0)
			eyeAngx += 360.0;
		y[0] = u[1][0] * cv + u[2][0] * sv;
		y[1] = u[1][1] * cv + u[2][1] * sv;
		y[2] = u[1][2] * cv + u[2][2] * sv;

		z[0] = u[2][0] * cv - u[1][0] * sv;
		z[1] = u[2][1] * cv - u[1][1] * sv;
		z[2] = u[2][2] * cv - u[1][2] * sv;

		for (i = 0; i < 3; i++)
		{
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'h')
	{ /* heading 對Y軸旋轉 */
		eyeAngy += 5.0;
		if (eyeAngy > 360.0)
			eyeAngy -= 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] - sv * u[2][i];
			z[i] = sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'H')
	{
		eyeAngy += -5.0;
		if (eyeAngy < 0.0)
			eyeAngy += 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] + sv * u[2][i];
			z[i] = -sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'r')
	{ /* rolling 對z軸旋轉 */
		eyeAngz += 5.0;
		if (eyeAngz > 360.0)
			eyeAngz -= 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] - sv * u[1][i];
			y[i] = sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == 'R')
	{
		eyeAngz += -5.0;
		if (eyeAngz < 0.0)
			eyeAngz += 360.0;
		for (i = 0; i < 3; i++)
		{
			x[i] = cv * u[0][i] + sv * u[1][i];
			y[i] = -sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++)
		{
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
    else if(key == '+'){
        if (zoom_mode)
            zoom_mode = 0;
        else
            zoom_mode = 1;
    }
    else if(key == 'k' || key == 'l')
    {
        zoom(key);
    }
    display();
}

// Check boundary condition
bool CheckBoundary()
{
    cout << "(" << test_x << "," << test_y << "," << test_z << ")\n";
    float distance = 0.0;
    if (!crouch && !run)
    {
        distance = 0.6;
    }
    if (crouch && !run)
    {
        distance = 0.3;
    }
    if (!crouch && run)
    {
        distance = 0.6 * run_speed;
    }

    if (turn_ang == 0)
    {
        if ((test_z + distance > 23.4 && test_z < 39.6) && (test_x >= -8.6 && test_x <= 13.6))
            return false;
        if ((test_z + distance > -10.0 && test_z < 10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z + distance > 10.0 && test_z < 30.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 90 || turn_ang == -270)
    {
        if ((test_x + distance > -8.6 && test_x < 13.6) && (test_z >= 23.4 && test_z <= 39.6))
            return false;
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= -10 && test_z <= 10))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x + distance > -40.0 && test_x < -20.0) && (test_z >= 10 && test_z <= 30))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == 180 || turn_ang == -180)
    {
        if ((test_z - distance < 39.6 && test_z > 23.4) && (test_x >= -8.6 && test_x <= 13.6))
            return false;
        if ((test_z - distance < 10.0 && test_z > -10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_z - distance < 30.0 && test_z > 10.0) && (test_x >= -40.0 && test_x <= -20.0))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }
    if (turn_ang == -90 || turn_ang == 270)
    {
        if ((test_x - distance < 13.6 && test_x > -8.6) && (test_z >= 23.4 && test_z <= 39.6))
            return false;
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= -10 && test_z <= 10))
        {
            weapon = SWORD;
            hold = true;
            return true;
        }
        if ((test_x - distance < -20.0 && test_x > -40.0) && (test_z >= 10 && test_z <= 30))
        {
            weapon = LANCE;
            hold = true;
            return true;
        }
    }

    return true;
}

// stop func
void Stop()
{
    if (counter % angle_speed == 0 && !crouch)
    {
        if (La_ang < 0)
        {
            La_ang += 1;
        }
        if (r_leg < 0 /*&& La_ang % 2 == 0*/)
        {
            r_leg += 1;
        }
        if (Ra_ang < 0)
        {
            Ra_ang += 1;
        }
        if (l_leg < 0 /*&& Ra_ang % 2 == 0*/)
        {
            l_leg += 1;
        }
        if ((La_ang == r_leg && r_leg == Ra_ang) && (Ra_ang == l_leg && l_leg == 0))
            stop = true;
    }
    else if (counter % angle_speed == 0 && crouch)
    {
        if (r_calf < 90)
        {
            r_calf++;
        }
        if (l_calf < 90)
        {
            l_calf++;
            if (l_calf == 90)
                stop = true;
        }
    }
    counter--;
    if (counter == 0)
        stop = true;
}

// moveforward func
void Move()
{
    float distance = 0.0;
    if (!crouch && !run)
    {
        distance = 0.6;
    }
    if (crouch && !run)
    {
        distance = 0.3;
    }
    if (!crouch && run)
    {
        distance = 0.6 * run_speed;
    }
    if (turn_ang == 0 && CheckBoundary())
        test_z += distance;
    if ((turn_ang == 90 || turn_ang == -270) && CheckBoundary())
        test_x += distance;
    if ((turn_ang == 180 || turn_ang == -180) && CheckBoundary())
        test_z -= distance;
    if ((turn_ang == -90 || turn_ang == 270) && CheckBoundary())
        test_x -= distance;
}

void movement()
{
    spin = (spin + 5) % 360;
    if (mode == STOP)
    {
        if (!stop)
        {
            run = false;
            Stop();
        }
    }
    else if (mode == WALK)
    {
        counter++;
        stop = false;
        if ((counter % angle_speed == 0 && !crouch) && !run)
        {
            if (one_side_move)
            {
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (l_leg > -15 && Ra_ang % 2 == 0)
                {
                    l_leg -= 1;
                }
                if (La_ang < 0)
                {
                    La_ang += 1;
                }
                if (r_leg < 0 && La_ang % 2 == 0)
                {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (r_leg > -15 && La_ang % 2 == 0)
                {
                    r_leg -= 1;
                }
                if (Ra_ang < 0)
                {
                    Ra_ang += 1;
                }
                if (l_leg < 0 && Ra_ang % 2 == 0)
                {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if ((counter % angle_speed == 0 && crouch) && !run)
        {
            if (one_side_move)
            {
                if (l_calf > 45)
                {
                    l_calf -= 1;
                }
                if (r_calf < 90)
                {
                    r_calf += 1;
                }
                if (La_ang < 0)
                {
                    La_ang += 1;
                }
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (l_calf == 45)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (r_calf > 45)
                {
                    r_calf -= 1;
                }
                if (l_calf < 90)
                {
                    l_calf += 1;
                }
                if (Ra_ang < 0)
                {
                    Ra_ang += 1;
                }
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (r_calf == 45)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
        else if (counter % angle_speed == 0 && run)
        {
            if (one_side_move)
            {
                if (Ra_ang > -30)
                {
                    Ra_ang -= 1;
                }
                if (La_ang < 15)
                {
                    La_ang += 1;
                }
                if (l_leg > -15)
                {
                    l_leg -= 1;
                }
                if (r_leg < 15)
                {
                    r_leg += 1;
                }
                if (Ra_ang == -30 && l_leg == -15)
                {
                    Move();
                    one_side_move = false;
                }
            }
            else
            {
                if (La_ang > -30)
                {
                    La_ang -= 1;
                }
                if (Ra_ang < 15)
                {
                    Ra_ang += 1;
                }
                if (r_leg > -15)
                {
                    r_leg -= 1;
                }
                if (l_leg < 15)
                {
                    l_leg += 1;
                }
                if (La_ang == -30 && r_leg == -15)
                {
                    Move();
                    one_side_move = true;
                }
            }
        }
    }
    else if (mode == JUMP)
    {
        if (test_y < 1.0 * jump_s && jump)
        {
            test_y += 0.1;
        }
        else
        {
            jump = false;
            jump_s = 0;
        }
        if (!jump && test_y > 0.0)
        {
            test_y -= 0.1;
        }
    }
    else if (mode == GESTURE)
    {
        Stop();
        if (gesture < 120)
        {
            gesture++;
        }
        else if (ges_fore < 90)
        {
            ges_fore++;
        }
        else
        {
            ges = false;
        }
        if (!ges)
        {
            if (ges_fore > 0)
            {
                ges_fore--;
            }
            else if (gesture > 0)
            {
                gesture--;
            }
        }
    }
    glutPostRedisplay();
}

/*---------------------------------------------
 * Idle event callback function, increase blade and arm
 * rotation angles.
 */
void idle_func()
{
    bld_ang += 5.0;
    if (bld_ang > 360.0)
    {
        bld_ang -= 360.0;
        arm_ang += 3.0;
        if (arm_ang > 360.0)
            arm_ang -= 360.0;
    }
    display(); /* show the scene again */
}

void mouse(int button, int state, int x, int y)
{
    // Wheel reports as button 3(scroll up) and button 4(scroll down)
    if ((button == 3) || (button == 4)) // It's a wheel event
    {
        // Each wheel event reports like a button click, GLUT_DOWN then GLUT_UP
        if (state == GLUT_UP)
            return; // Disregard redundant GLUT_UP events
        printf("Scroll %s At %d %d\n", (button == 3) ? "Up" : "Down", x, y);
        if (button == 3)
        {
            view_x += 0.1;
            view_y += 0.1;
            view_z += 0.1;
        }
        else
        {
            if (view_x == 1.0)
                return;
            view_x -= 0.1;
            view_y -= 0.1;
            view_z -= 0.1;
        }
    }
    else
    { // normal button event
        printf("Button %s At %d %d\n", (state == GLUT_DOWN) ? "Down" : "Up", x, y);
    }
}

void motion(int x, int y)
{
}

void specialKey(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        turn_ang = (turn_ang + 90) % 360;
        break;
    case GLUT_KEY_RIGHT:
        turn_ang = (turn_ang - 90) % 360;
        break;
    }
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
int main(int argc, char **argv)
{
    /*-----Initialize the GLUT environment-------*/
    glutInit(&argc, argv);

    /*-----Depth buffer is used, be careful !!!----*/
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("robot");

    myinit(); /*---Initialize other state varibales----*/

    /*----Associate callback func's whith events------*/
    glutDisplayFunc(display);
    // glutIdleFunc(idle_func);    /* Add idle event */
    glutReshapeFunc(my_reshape);
    glutKeyboardFunc(my_keyboard);
    glutMouseFunc(mouse);
    glutIdleFunc(movement);
    glutSpecialFunc(specialKey);

    glutMainLoop();
    return 1;
}

/*
    視野直視角色
    glRotatef(20.0, 0.0, 1.0, 0.0);
    glRotatef(-50, 1.0, 0.0, 0.0);

    Debug Obj
    glTranslatef(test_x, 0.0, 0.0);
    glTranslatef(0.0, test_y, 0.0);
    glTranslatef(0.0, 0.0, test_z);

    glRotatef(r_x, 1.0, 0.0, 0.0);
    glRotatef(r_y, 0.0, 1.0, 0.0);
    glRotatef(r_z, 0.0, 0.0, 1.0);
*/