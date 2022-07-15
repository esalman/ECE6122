/*
Author:         Mustafa Salman
Class:          ECE 6122
Last modified:  11/22/2019

Description:    This program creates a 3D chess set model using OpenGL following 
                specified rules.
*/

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <cmath>

// initializations
// initial camera position
float x = 4.0, y = -5.0, z = 10.0;
// camera center
float lx = 4.0, ly = 4.0, lz = 0.0;
// amount of eye movement along z-axis 
float deltaMove = 0.0;
// amount of rotation along z-axis
float deltaRotate = 0.0;

// light and dark piece colors
float lightr = 0.0/255.0, lightg = 255.0/255.0, lightb = 0.0/255.0;
float darkr = 255.0/255.0, darkg = 255.0/255.0, darkb = 0.0/255.0;

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

// initial piece (x,y) coordinates
float pawn_locations_x[] = {0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5};
float pawn_locations_y[] = {1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5};
float knight_locations_x[] = {1.5, 1.5, 6.5, 6.5};
float knight_locations_y[] = {0.5, 7.5, 0.5, 7.5};
float other_locations_x[] = {0.5, 0.5, 7.5, 7.5, 2.5, 2.5, 5.5, 5.5, 3.5, 3.5, 4.5, 4.5};
float other_locations_y[] = {0.5, 7.5, 0.5, 7.5, 0.5, 7.5, 0.5, 7.5, 0.5, 7.5, 0.5, 7.5};

// functions

// checks if the new coordinate is a valid move
// Input:
//     idx:    index of the piece, 0-15 for pawns and 0-3 for knights
//     x, y:   new coordinate
//     type_:  'k' for knight or 'p' for pawn
// Output:
//     (bool) whether new coordinate is valid or not
bool isValidMove( int idx, float x, float y, unsigned char type_ )
{
    // for comparing the difference between two floats
    float eps = 0.005;
    
    // outside the board?
    if ( x < 0 || x > 8 || y < 0 || y > 8 )
    {
        return false;
    }
    
    // is the new coordinate occupied by any another piece other than a knight/pawn
    for ( int i = 0; i < 12; i += 1 )
    {
        if ( fabs( other_locations_x[i] - x ) < eps && fabs( other_locations_y[i] - y ) < eps )
        {
            return false;
        }
    }

    // is thew new coordinate occupied by knight
    for ( int i = 0; i < 4; i += 1 )
    {
        if ( type_ == 'k' && i == idx ) continue; // own old location
        if ( fabs( knight_locations_x[i] - x ) < eps && fabs( knight_locations_y[i] - y ) < eps )
        {
            return false;
        }
    }
    
    // is thew new coordinate occupied by another pawn
    for ( int i = 0; i < 16; i += 1 )
    {
        if ( type_ == 'p' && i == idx ) continue; // own old location
        if ( fabs( pawn_locations_x[i] - x ) < eps && fabs( pawn_locations_y[i] - y ) < eps )
        {
            return false;
        }
    }
    
    // a valid move
    return true;
}

// tries to move the selected piece to a random but valid new coordinate
// Input:
//     idx:    index of the piece, 0-15 for pawns and 0-3 for knights
//     type_:  'k' for knight or 'p' for pawn
// Output:
//     (bool) whether the piece was moved or not
bool updatePieceLocation( int idx, unsigned char type_ )
{
    bool flag;
    float new_x, new_y;

    switch ( type_ )
    {
        // move the pawn
        case 'p':
            // pawn stays on the same vertical axis
            new_x = pawn_locations_x[idx];
            // randomly choose to move one step forward or backward
            new_y = rand() % 2 == 0 ? pawn_locations_y[idx] - 1 : pawn_locations_y[idx] + 1;
            // is new x,y valid?
            flag = isValidMove( idx, new_x, new_y, type_ );
            // if valid, update location of the selected piece
            if ( flag )
            {
                pawn_locations_x[idx] = new_x;
                pawn_locations_y[idx] = new_y;
                return true;
            }
            break;
        // move the knight
        case 'k':
            // knight can move to 8 possible directions, randomly choose one
            int temp_x = rand() % 8;
            switch ( temp_x )
            {
                case 0:
                    new_x = knight_locations_x[idx] + 1;
                    new_y = knight_locations_y[idx] - 2;
                    break;
                case 1:
                    new_x = knight_locations_x[idx] + 1;
                    new_y = knight_locations_y[idx] + 2;
                    break;
                case 2:
                    new_x = knight_locations_x[idx] - 1;
                    new_y = knight_locations_y[idx] - 2;
                    break;
                case 3:
                    new_x = knight_locations_x[idx] - 1;
                    new_y = knight_locations_y[idx] + 2;
                    break;
                case 4:
                    new_x = knight_locations_x[idx] + 2;
                    new_y = knight_locations_y[idx] - 1;
                    break;
                case 5:
                    new_x = knight_locations_x[idx] + 2;
                    new_y = knight_locations_y[idx] + 1;
                    break;
                case 6:
                    new_x = knight_locations_x[idx] - 2;
                    new_y = knight_locations_y[idx] - 1;
                    break;
                case 7:
                    new_x = knight_locations_x[idx] - 2;
                    new_y = knight_locations_y[idx] + 1;
                    break;
            }
            flag = isValidMove( idx, new_x, new_y, type_ );
            if ( flag )
            {
                knight_locations_x[idx] = new_x;
                knight_locations_y[idx] = new_y;
                return true;
            }
            break;
    }

    // could not move the selected piece
    return false;
}

void update(void)
{
    if (deltaMove) { // update camera position
        z += deltaMove;
    }
    glutPostRedisplay(); // redisplay everything
}

// handle keypress event
void processNormalKeys( unsigned char key, int xx, int yy )
{
    switch( key )
    {
        case 27:
        case 'q':
        case 'Q':
            // quit
            exit(0);
            break;
        case 'u':
        case 'U':
            // move eye up along z-axis
            deltaMove = 0.25;
            break;
        case 'd':
        case 'D':
            // move eye down along z-axis
            deltaMove = -0.25;
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
        case 'k':
        case 'K':
            // move a knight
            // loop until a valid move is found
            while ( true )
            {
                // select a random knight
                int to_move = rand() % 4;
                // try to move
                bool flag = updatePieceLocation(to_move, 'k');
                // break loop if successful
                if ( flag ) break;
            }
            break;
        case 'p':
        case 'P':
            // move a pawn
            while ( true )
            {
                int to_move = rand() % 16;
                bool flag = updatePieceLocation(to_move, 'p');
                if ( flag ) break;
            }
            break;
    }

    // redraw after every keypress
    update();
}

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

    // redraw after every keypress
    update();
}

// handle window resize event
void changeSize(int w, int h)
{
    // window aspect ratio
    float ratio = ((float)w) / ((float)h);
    
    glMatrixMode(GL_PROJECTION);
    // reset the projection
    glLoadIdentity(); 
    // perspective transformation
    gluPerspective(45.0, ratio, 0.1, 100.0);
    // return to modelview mode
    glMatrixMode(GL_MODELVIEW); 
    // set viewport (drawing area) to entire window
    glViewport(0, 0, w, h); 
}

// function for drawing the chess board
void drawBoard()
{
    // 2 vertical columns at a time
    for ( float dx = 0.0; dx <= 6; dx += 2 )
    {
        // draw one white one black square at a time
        for ( float dy = 0.0; dy <= 7; dy++ )
        {
            // set color of the current square
            if ( int(dy) % 2 == 0 )
                glColor3f(0.0, 0.0, 0.0);
            else
                glColor3f(1.0, 1.0, 1.0);
            
            // draw a cube scaled along z-axis
            glPushMatrix();
                glTranslatef(dx+.5, dy+.5, -0.05);
                glScalef(1.0, 1.0, 0.1);
                glutSolidCube(1);
            glPopMatrix();
            
            if ( int(dy) % 2 == 0 )
                glColor3f(1.0, 1.0, 1.0);
            else
                glColor3f(0.0, 0.0, 0.0);
            glPushMatrix();
                glTranslatef(dx+1.5, dy+.5, -0.05);
                glScalef(1.0, 1.0, 0.1);
                glutSolidCube(1);
            glPopMatrix();
        }
    }
}

// function for drawing the pieces
void drawPieces()
{
    std::vector<float> posx, posy;

    // pawns
    for ( int i = 0; i < 16; i += 1 )
    {
        if ( i < 8 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            glTranslatef(pawn_locations_x[i], pawn_locations_y[i], 0.30);
            // pawns are a little smaller to make the pieces look proportionate
            glutSolidSphere(0.30, 20, 20);
        glPopMatrix();
    }

    // rook
    posx = {0.5, 0.5, 7.5, 7.5};
    posy = {0.5, 7.5, 0.5, 7.5};
    for ( int i = 0; i < 4; i++ )
    {
        if ( i % 2 == 0 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            glTranslatef(posx[i], posy[i], 0.6/2);
            // rooks are also little smaller to make the pieces look proportionate
            glScalef(1.0, 1.0, 0.6/0.75);
            glutSolidCube(0.75);
        glPopMatrix();
    }

    // knight
    for ( int i = 0; i < 4; i++ )
    {
        if ( i % 2 == 0 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            glTranslatef(knight_locations_x[i], knight_locations_y[i], 0.85/2);
            // apply proper rotation
            glRotatef(90.0, 1.0, 0.0, 0.0);
            // scale the knight a little bit to make it look proportionate
            glScalef(0.75, 1.5, 1.0);
            glutSolidTeapot(0.75/2);
        glPopMatrix();
    }

    // bishop
    posx = {2.5, 2.5, 5.5, 5.5};
    posy = {0.5, 7.5, 0.5, 7.5};
    for ( int i = 0; i < 4; i++ )
    {
        if ( i % 2 == 0 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            glTranslatef(posx[i], posy[i], 0.0);
            glutSolidCone(0.75/2, 1, 20, 20);
        glPopMatrix();
    }

    // queen
    posx = {3.5, 3.5};
    posy = {0.5, 7.5};
    for ( int i = 0; i < 2; i++ )
    {
        if ( i % 2 == 0 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            // this works for freeglut on Windows/CentOS
            glTranslatef(posx[i], posy[i], 0.3);
            glRotatef(-90, 0.0, 1.0, 0.0);
            glScalef(1.0, 0.5, 0.5);

            // // this is for pace-ice
            // glTranslatef(posx[i], posy[i]+0.1, 0.1);
            // // rotate and scale the queen a little bit to make it look proportionate
            // glRotatef(-90, 0.0, 1.0, 1.0);
            // glScalef(0.5, 0.5, 0.5);
            glutSolidTetrahedron();
        glPopMatrix();
    }

    // king
    posx = {4.5, 4.5};
    posy = {0.5, 7.5};
    for ( int i = 0; i < 2; i++ )
    {
        if ( i % 2 == 0 )
            glColor3f(lightr, lightg, lightb);
        else
            glColor3f(darkr, darkg, darkb);
        glPushMatrix();
            glTranslatef(posx[i], posy[i], 0.75);
            // rotate and scale the king a little bit to make it look proportionate
            glRotatef(-90, 0.0, 1.0, 0.0);
            glScalef(0.65, 0.4, 0.4);
            glutSolidOctahedron();
        glPopMatrix();
    }
}

// handle rendering display
void displayFun(void)
{
    // Clear color and depth buffers
    // background is light blue
    glClearColor(0.0, 0.7, 1.0, 1.0); 
    // use double buffering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // smooth shader
    glShadeModel(GL_SMOOTH);

    // Reset transformations
    glLoadIdentity();

    // setup lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    // toggle ambient and diffuse light
    if ( light0_status ) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);
    if ( light1_status ) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    // set material properties
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    // not sure if it improves anything but leaving it here
    glEnable(GL_NORMALIZE);

    // set the camera
    gluLookAt(
        x, y, z,
        lx, ly, lz,
        0.0, 0.0, 1.0);

    // handle rotation
    glPushMatrix();
        // translate the origin to (4,4,0)
        glTranslatef(4.0, 4.0, 0.0);
        // apply rotation
        glRotatef(deltaRotate, 0.0, 0.0, 1.0); // now rotate
        glTranslatef(-4.0, -4.0, 0.0);

        // draw everything
        drawBoard();
        drawPieces();
    // reset origin
    glPopMatrix();

    // Make it all visible
    glutSwapBuffers(); 
}

// main function
int main(int argc, char** argv)
{
    // initialize
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(1000, 0);
    glutCreateWindow("Chessboard");

    // setup even callbacks
    glutReshapeFunc(changeSize);
    glutDisplayFunc(displayFun);
    glutKeyboardFunc(processNormalKeys);
    glutKeyboardUpFunc(processKeyUp);
    
    glutMainLoop();
    
    return 0;
}
