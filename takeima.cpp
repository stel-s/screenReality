// simpletexture.cpp
// by Glenn G. Chappell
// November 2003
//
// For CS 381
// Simple Example of Texture Mapping

#include <iostream>
using std::cerr;
using std::endl;
#include <stdlib.h>
//using std::exit;
#include <GL/glut.h> // GLUT stuff - includes OpenGL headers
#include <SOIL.h>

// Global variables
// Window/viewport
const int startwinsize = 400; // Starting window width & height, in pixels
const double neardist = 0.5;  // Near & far clippng planes
const double fardist = 10.;

// Keyboard
const int ESCKEY = 27;        // ASCII value of escape character

// For texture
const int img_width = 32;
const int img_height = 32;
GLubyte the_image[img_height][img_width][3];
   // The image
   // 3rd subscript 0 = R, 1 = G, 2 = B
GLuint texname;  // Name for this texture


// display
// The GLUT display function
void display()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glEnable(GL_TEXTURE_2D);  // Turn on texture.
      // Turn it off if you draw something without a texture.
 
   // Draw square
   glPushMatrix();  // We are in model/view mode

      glTranslated(0., 0., -3.);      

      glBegin(GL_POLYGON);
         glTexCoord2d(1.0, 1.0); glVertex3d( 2.0, 2.0, 0.0);
         glTexCoord2d(0.0, 1.0); glVertex3d(-2.0, 2.0, 0.0);
         glTexCoord2d(0.0, 0.0); glVertex3d(-2.0,-2.0, 0.0);
         glTexCoord2d(1.0, 0.0); glVertex3d( 2.0,-2.0, 0.0);
      glEnd();
       glPopMatrix();
    
    // GEOMETRY
    //-- TeaPot
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-1, -3, -10);
        glutSolidTeapot(5);
    glTranslatef(1, 3, 10);
    
    //-- Cube 1
    glColor3f(1.0f, 1.0f, 0.0f);
    glTranslatef(-10, -5, 30);
        glutSolidCube(3);
    glTranslatef(10, 5, -30);
    
    //-- Cube 2
    glColor3f(1.0f, 0.0f, 1.0f);
    glTranslatef(-20, 0, -40);
        glutSolidCube(3);
    glTranslatef(20, 0, 40);
    
    //-- Cube 3
    glColor3f(0.0f, 1.0f, 1.0f);
    glTranslatef(5, 5, 10);
        glutSolidCube(3);
    glTranslatef(-5, -5, -10);
    

    glDisable(GL_LIGHTING); //Disable lighting

  

   glutSwapBuffers();
}


// reshape
// The GLUT reshape function
void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, double(w)/h, neardist, fardist);

   glMatrixMode(GL_MODELVIEW);  // Always go back to modelview mode
}


// keyboard
// The GLUT keyboard function
void keyboard(unsigned char key, int x, int y)
{
   switch (key)
   {
   case ESCKEY:  // ESC: Quit
      exit(0);
      break;
   }
}


// idle
// The GLUT idle function
void idle()
{
   // Print OpenGL errors, if there are any (for debugging)
   if (GLenum err = glGetError())
   {
      cerr << "OpenGL ERROR: " << gluErrorString(err) << endl;
   }

}


// makeimage
// Make image in the_image
// (Code taken without change from drawimage.cpp)
void makeimage()
{
   for (int i=0; i<img_width; ++i)
   {
      double x = double(i)/(img_width-1);
      for (int j=0; j<img_height; ++j)
      {
         double y = double(j)/(img_height-1);

         the_image[j][i][0] = int(x*255)*15 % 256;
         the_image[j][i][1] = int(y*255)*15 % 256;
         the_image[j][i][2] = 0.75*255;
      }
   }
}
const float pixelNbrPerCm = 50.0;
int windowWidth;
int windowHeight;
float cx;
float cy;
int camWidth;
int camHeight;

float pixelToCm(int size)
{
    return (float)size/pixelNbrPerCm;
}
// init
// Initializes GL states
// Called by main
void init()
{
   glClearColor(1.0, 1.0, 1.0, 0.0);
   glEnable(GL_DEPTH_TEST);

   // Texture set-up
    int width, height;
    unsigned char* image;
//     GLuint textures[2];
   // Basic image set-up
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
 //  makeimage();
   GLuint tex_2d = SOIL_load_OGL_texture
	(
		"img.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
   // Generate a texture name and bind it to GL_TEXTURE_2D.
   // Then we no longer use texname, since GL_TEXTURE_2D is bound to it.
   // NOTE: binding a texture name is actually NOT necessary when we
   //  use only one texture, but it does not hurt.
   glGenTextures(1, &texname);
   glBindTexture(GL_TEXTURE_2D, tex_2d);

   // Set various texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  // Needs OpenGL 1.1
   
   // Create the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
   // Create the actual texture
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height,
//                 0, GL_RGB, GL_UNSIGNED_BYTE, the_image);
}
//-- dimensions

void onReshape( int w, int h )
{
    
    windowWidth = w;
    windowHeight = h;
    cx = pixelToCm(windowWidth);
    cy = pixelToCm(windowHeight);
    glViewport( 0, 0, windowWidth, windowHeight );
}

int main(int argc, char ** argv)
{
   // Initialize OpenGL/GLUT
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

   // Make a window
   glutInitWindowSize(startwinsize, startwinsize);
   glutInitWindowPosition(500, 500);
   glutCreateWindow("CS 381 - Simple Texture Demo");

   // Initialize GL states & register callbacks
   init();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals
///
  //-- intrinsic camera params
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        //gluPerspective(60, (float)600/(float)200, 1, 250);
        //-- extrinsic camera params
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //gluLookAt(+1.9, +2.5, -20, 1, 0, 0, 0, 1, 0);
    ///
    
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(idle);

   // Do something
   glutMainLoop();

   return 0;
}
