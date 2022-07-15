/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  11/23/2019

Description:    This program 3D simulates a UAV half-time show using openGL and MPI.
*/

#include <iostream>
#include "mpi.h"
#include <chrono>
#include <thread>
#include <GL/glut.h>
#include "ECE_Bitmap.h"
#include <cmath>
using namespace std;

#define WINDOW_TITLE "Buzzy Bowl"
// window location
#define DEFAULT_X 10
#define DEFAULT_Y 10
// window size
#define DEFAULT_WIDTH 400
#define DEFAULT_HEIGHT 400
// football field dimension in meters
#define FIELD_LENGTH 91.44
#define FIELD_WIDTH 48.77
// UAV bounding box
#define BOUNDING_BOX 2.0
// how frequently render scene in milleseconds
#define TIMER_DELAY 100
// duration of simulation in seconds
#define TIMER_LIMIT 60
// for comparing two floats 
#define EPS 0.005
// mass of UAV
#define MASS 1
// parameter for the virtual sphere
#define SPHERE_X 0.0
#define SPHERE_Y 0.0
#define SPHERE_Z 50.0
#define SPHERE_R 10.0

// setup MPI buffers
// send location and velocity vector in each direction
int numTasks;
const int numel = 6; // x, y, z, vx, vy, vz
int rcvsize;
double* rcvbuffer;
double sendbuffer[numel];

// determine the number of times we call the timer function
int max_steps = TIMER_LIMIT * 1000 / TIMER_DELAY;

// control UAV color
int uav_b = 255.0;
int uav_b_incr = -1.0;

// rank 0 timer counter
int main_ts = 0;

// camera params
float eye_x = 0.0, eye_y = -70, eye_z = 70.0;
float center_x = 0.0, center_y = 30.0, center_z = 0.0;
// amount of eye movement along z-axis 
float deltaMove = 0.0;
// amount of rotation around z-axis
float deltaRotate = 0.0;

// toggle lighting status
bool light0_status = true;
bool light1_status = true;
// lighting specifications
GLfloat light0_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light1_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light1_specular[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat light1_position[] = { 5.0, 5.0, 8.0 };
GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat mat_shininess[] = { 50.0 };

// handle reshaping the window
void changeSize(int w, int h)
{
    float ratio = ((float)w) / ((float)h); // window aspect ratio
    glMatrixMode(GL_PROJECTION); // projection matrix is active
    glLoadIdentity(); // reset the projection
    gluPerspective(60.0, ratio, 0.1, 1000.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

// draw the football field as part of rendering the screen
void displayFootballField()
{
    // load bitmap texture
    BMP inBitmap;
    GLuint texture[1];

    glDepthFunc(GL_LESS);

    inBitmap.read("ff.bmp");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, texture);

    // Setup texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);
    glEnable(GL_TEXTURE_2D);

    // the football field bimap has big margin, we need to place UAVs at the corners properly
    float tex_length = FIELD_LENGTH * 2000 / 1580;
    float tex_width = FIELD_WIDTH * 965 / 855;

    // Draw the field using the bitmap texture
    glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0);
            glVertex3f(-tex_length/2.0, -tex_width/2.0, 0.0);
            glTexCoord2f(0, 1);
            glVertex3f(-tex_length/2.0, tex_width/2.0, 0.0);
            glTexCoord2f(1, 1);
            glVertex3f(tex_length/2.0, tex_width/2.0, 0.0);
            glTexCoord2f(1, 0);
            glVertex3f(tex_length/2.0, -tex_width/2.0, 0.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    // draw wire sphere- for debugging
    glColor3f(128.0, 128.0, 128.0);
    glPushMatrix();
        glTranslatef(SPHERE_X, SPHERE_Y, SPHERE_Z);
        glutWireSphere( SPHERE_R, 20, 20 );
    glPopMatrix();
}

// draw the UAVs based on new location
void drawUAVs()
{
    int idx;
    
    for ( int ii = 1; ii < numTasks; ii++ )
    {
        idx = ii * numel;

        // change color value in every step
        uav_b_incr = ( uav_b >= 255.0 ) ? -1.0 : ( uav_b <= 128.0 ? 1.0 : uav_b_incr );
        uav_b += uav_b_incr;

        glColor3f(0, 0, uav_b/255.0);
        glPushMatrix();
            glTranslatef(rcvbuffer[idx], rcvbuffer[idx+1], rcvbuffer[idx+2]);
            glutSolidCone(BOUNDING_BOX/2, BOUNDING_BOX, 20, 20);
        glPopMatrix();
    }
}

// render scene callback fundtion
void renderScene()
{
    // Clear color and depth buffers
    glClearColor(0.0, 0.7, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset transformations
    glLoadIdentity();

    // toggle ambient and diffuse light
    if ( light0_status ) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);
    if ( light1_status ) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
    
    // set material properties
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    gluLookAt(eye_x, eye_y, eye_z, 
              center_x, center_y, center_z,
              0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    // handle rotation
    glPushMatrix();
        glRotatef(deltaRotate, 0.0, 0.0, 1.0);

        // draw everything
        displayFootballField();
        drawUAVs();
    glPopMatrix();

    // make it all visible
    glutSwapBuffers();
}

// check if any UAV collided with another in the previous step, and swaps velocity if so
void collisionStatus()
{
    int idxa, idxb;

    for ( int ii = 1; ii < numTasks; ii++ )
    {
        idxa = ii * numel;
        
        // get UAV location
        double xa = rcvbuffer[idxa];
        double ya = rcvbuffer[idxa+1];
        double za = rcvbuffer[idxa+2];

        // compare will all other UAV location
        for ( int i = 1; i <= numTasks; i++ )
        {
            // can't collide with itself
            if ( i == ii )
            {
                continue;
            }

            // get other UAV index
            idxb = i * numel;

            // get the other UAV location
            double xb = rcvbuffer[idxb];
            double yb = rcvbuffer[idxb+1];
            double zb = rcvbuffer[idxb+2];

            // distance calculation, collision logic
            double dist = sqrt( pow( xa-xb, 2 ) + pow( ya-yb, 2 ) + pow( za-zb, 2 ) );
            if ( dist < 1.0/100.0 )
            {
                // swap velocity
                double temp1 = rcvbuffer[idxa+3];
                double temp2 = rcvbuffer[idxa+4];
                double temp3 = rcvbuffer[idxa+5];
                rcvbuffer[idxa+3] = rcvbuffer[idxb+3];
                rcvbuffer[idxa+4] = rcvbuffer[idxb+4];
                rcvbuffer[idxa+5] = rcvbuffer[idxb+5];
                rcvbuffer[idxb+5] = temp1;
                rcvbuffer[idxb+5] = temp2;
                rcvbuffer[idxb+5] = temp3;
            }
        }
    }
}

// timer callback function
void timerFunction( int id )
{
    // redraw on every timer pulse
    glutPostRedisplay();

    if ( main_ts < max_steps )
    {
        // check for collision before next step
        MPI_Allgather(sendbuffer, numel, MPI_DOUBLE, rcvbuffer, numel, MPI_DOUBLE, MPI_COMM_WORLD);
        collisionStatus();
        glutTimerFunc(TIMER_DELAY, timerFunction, 0);
    }
    else
    {
        cout << "Simulation finished, press ESC to exit\n";
    }
}

// key press callback function
// - q or ESC to quit
// - u or U to move camera up
// - d or D to move camera down
// - r or R to rotate field
// - 0 to toggle LIGHT_0
// - 1 to toggle LIGHT_1
void processNormalKeys( unsigned char key, int xx, int yy )
{
    switch( key )
    {
        case 27:
        case 'q':
        case 'Q':
            // quit
            MPI_Abort(MPI_COMM_WORLD, 0);
            break;
        case 'u':
        case 'U':
            // move eye up along z-axis
            eye_z += 0.25;
            break;
        case 'd':
        case 'D':
            // move eye down along z-axis
            eye_z -= 0.25;
            break;
        case 'r':
        case 'R':
            // rotate world
            deltaRotate += 10.0;
            break;
        case '0':
            // toggle ambient light
            light0_status = !light0_status;
            break;
        case '1':
            // toggle diffuse light
            light1_status = !light1_status;
            break;
    }
}

// key up callback function
void processKeyUp( unsigned char key, int xx, int yy )
{
    switch( key ) 
    {
        case 'u':
        case 'U':
        case 'd':
        case 'D':
            deltaMove = 0.0;
            break;
    }
}

// initialize OpenGL and register callback functions. Called once from rank 0.
void mainOpenGL( int argc, char**argv )
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(DEFAULT_X, DEFAULT_Y);
    glutInitWindowSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

    glutCreateWindow( WINDOW_TITLE );
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    // set material properties
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // register callbacks
    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutTimerFunc(100, timerFunction, 0);
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(processKeyUp);
    glutMainLoop();
}

// handle UAV kinematics. Called once after every MPI gather
void CalcualteUAVsLocation( int rank )
{
    int idx = rank * numel;

    // get init position of the UAV based on rank
    double x0 = -FIELD_LENGTH/2.0 + ( ( rank % 5 ) ) * FIELD_LENGTH/4.0;
    double y0 = -FIELD_WIDTH/2.0 + ( ( rank % 3 ) ) * FIELD_WIDTH/2.0;
    double z0 = 0.0;

    // previous location and velocity
    double xa = rcvbuffer[idx];
    double ya = rcvbuffer[idx+1];
    double za = rcvbuffer[idx+2];
    double vxa = rcvbuffer[idx+3];
    double vya = rcvbuffer[idx+4];
    double vza = rcvbuffer[idx+5];

    // how far from 0,0,50?
    double dist = sqrt( pow( xa-SPHERE_X, 2 ) + pow( ya-SPHERE_Y, 2 ) + pow( za-SPHERE_Z, 2 ) );

    if ( dist > 10.0 )
    {
        double td = ( TIMER_DELAY / 1000.0 );
        // approach 0,0,50 at 2m/s
        // is current speed less than 2ms?
        double abs_v = sqrt( pow( vxa, 2 ) + pow( vya, 2 ) + pow( vza, 2 ) );
        if ( fabs( abs_v - 2.0 ) > EPS )
        {
            // x-axis
            // angle calculation
            // vector along the UAV is x0-xa,y0-ya,z0-za
            // unit vector along x-axis is 1,0,0
            // x . y = xa ya + xb yb + xc yc... but two of the terms are zero
            double dotProd_x = SPHERE_X - x0;
            double abs_va = EPS + sqrt( pow( x0 - SPHERE_X, 2 ) + pow( y0 - SPHERE_Y, 2 ) + pow( z0 - SPHERE_Z, 2 ) );
            double abs_vb = sqrt( pow( 1, 2 ) );
            // cos(theta) = x . y / |x||y|
            double cosTheta_x = dotProd_x / ( abs_va * abs_vb );

            // target force and acceleration calculation
            double target_vx = 2 * cosTheta_x;
            // a = ( v - v0 ) / t 
            double ax = ( target_vx - vxa ) / td;
            // F = ma
            double Fx = MASS * ax;

            // y-axis
            // angle calculation
            // unit vector along x-axis is 0,1,0
            double dotProd_y = SPHERE_Y - y0;
            double cosTheta_y = dotProd_y / ( abs_va * abs_vb );

            // target force and acceleration calculation
            double target_vy = 2 * cosTheta_y;
            double ay = ( target_vy - vya ) / td;
            double Fy = MASS * ay;

            // z-axis
            // angle calculation
            // unit vector along z-axis is 0,0,1
            double dotProd_z = SPHERE_Z - z0;
            double cosTheta_z = dotProd_z / ( abs_va * abs_vb );

            // target force and acceleration calculation
            double target_vz = 2 * cosTheta_z;
            double az = ( target_vz - vza ) / td;
            // take gravity into consideration
            double Fz =  10.0 + MASS * az;

            // new location
            // x = x0 + vx0 t + 0.5 a t^2
            double new_x = xa + target_vx * td + 0.5 * ax * pow( td, 2 );
            double new_y = ya + target_vy * td + 0.5 * ay * pow( td, 2 );
            double new_z = za + target_vz * td + 0.5 * az * pow( td, 2 );

            sendbuffer[0] = new_x;
            sendbuffer[1] = new_y;
            sendbuffer[2] = new_z;
            sendbuffer[3] = target_vx;
            sendbuffer[4] = target_vy;
            sendbuffer[5] = target_vz;
        }
        else
        {
            // otherwise just get new location
            double target_vx = sendbuffer[3];
            double target_vy = sendbuffer[4];
            double target_vz = sendbuffer[5];
            // x = x0 + vt
            double new_x = sendbuffer[0] + target_vx * td;
            double new_y = sendbuffer[1] + target_vy * td;
            double new_z = sendbuffer[2] + target_vz * td;

            sendbuffer[0] = new_x;
            sendbuffer[1] = new_y;
            sendbuffer[2] = new_z;
            sendbuffer[3] = target_vx;
            sendbuffer[4] = target_vy;
            sendbuffer[5] = target_vz;
        }
    }
    else {
        double td = ( TIMER_DELAY / 1000.0 );
        // virtual sphere logic
        // limit new speed to 2m/s ~ 5m/s
        double new_speed_limit = 0.03;
        // choose new xyz 
        double target_x = xa - SPHERE_X;
        double target_y = ya - SPHERE_Y;
        double target_z = za - SPHERE_Z;
        double r = -new_speed_limit + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/( 2 * new_speed_limit)));
        r += r > 0 ? 0.2 : -0.2;
        target_x += r;
        r = -new_speed_limit + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/( 2 * new_speed_limit)));
        r += r > 0 ? 0.2 : -0.2;
        target_y += r;
        r = -new_speed_limit + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/( 2 * new_speed_limit)));
        r += r > 0 ? 0.2 : -0.2;
        target_z += r;
        
        // distance from 0,0,0
        double abs_va = sqrt( pow( target_x, 2 ) + pow( target_y, 2 ) + pow( target_z, 2 ) );
        // scale to the surface
        target_x = xa + ( target_x * 10/abs_va );
        target_y = ya + ( target_y * 10/abs_va );
        target_z = za + ( target_z * 10/abs_va );
        
        // find target acceleration
        // a = 2 * ( x - x0 - v0 t ) / t^2
        double ax = 2 * ( target_x - xa - vxa * td ) / pow( td, 2 );
        double ay = 2 * ( target_y - ya - vya * td ) / pow( td, 2 );
        double az = 2 * ( target_z - za - vza * td ) / pow( td, 2 );

        // find target force
        double Fx = MASS * ax;
        double Fy = MASS * ay;
        // take gravity into consideration
        double Fz =  10.0 + MASS * az;

        // find new velocity
        double target_vx = vxa + ax * td;
        double target_vy = vya + ay * td;
        double target_vz = vza + az * td;

        // find new location
        double new_x = xa + target_vx * td + 0.5 * ax * pow( td, 2 );
        double new_y = ya + target_vy * td + 0.5 * ay * pow( td, 2 );
        double new_z = za + target_vz * td + 0.5 * az * pow( td, 2 );

        sendbuffer[0] = new_x;
        sendbuffer[1] = new_y;
        sendbuffer[2] = new_z;
        sendbuffer[3] = target_vx;
        sendbuffer[4] = target_vy;
        sendbuffer[5] = target_vz;        
    }

}

// main function executed in all MPI processes, different functionality based on rank
int main(int argc, char** argv)
{
    int rank;
    
    int rc = MPI_Init( &argc, &argv );
    if ( rc != MPI_SUCCESS )
    {
        cout << "Error starting MPI program. Terminating.\n";
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    rcvsize = numTasks * numel; // (Main task + 15 UAVs) * numElements
    rcvbuffer = new double[rcvsize];
    
    // set init position of UAVs
    sendbuffer[0] = -FIELD_LENGTH/2.0 + ( ( rank % 5 ) ) * FIELD_LENGTH/4.0;
    sendbuffer[1] = -FIELD_WIDTH/2.0 + ( ( rank % 3 ) ) * FIELD_WIDTH/2.0;
    sendbuffer[2] = 0.0;
    sendbuffer[3] = 0.0;
    sendbuffer[4] = 0.0;
    sendbuffer[5] = 0.0;

    MPI_Allgather(sendbuffer, numel, MPI_DOUBLE, rcvbuffer, numel, MPI_DOUBLE, MPI_COMM_WORLD);
    
    if ( rank == 0 )
    {
        printf("\n\
-----------------------------------------------------------------------\n\
Buzzy Bowl project keyboard commands:\n\
- q or ESC to quit\n\
- u or U to move camera up\n\
- d or D to move camera down\n\
- r or R to rotate field\n\
- 0 to toggle LIGHT_0\n\
- 1 to toggle LIGHT_1\n\
-----------------------------------------------------------------------\n");
        mainOpenGL(argc, argv);
    }
    else 
    {
        // Sleep for 5 seconds
        this_thread::sleep_for( chrono::seconds(5) );

        for (int ii = 0; ii < max_steps ; ii++)
        {
            CalcualteUAVsLocation(rank);
            MPI_Allgather(sendbuffer, numel, MPI_DOUBLE, rcvbuffer, numel, MPI_DOUBLE, MPI_COMM_WORLD);
        }

        cout << "UAV " << rank << " flight complete\n";
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
