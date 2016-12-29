#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <GL/gl.h>
#include <GL/freeglut.h>
#include <SOIL.h>
#include <iostream>
#include <stdio.h>

/** Constants */
const int minFaceSize = 80; // in pixel. The smaller it is, the further away you can go
const float f = 500; //804.71
const float eyesGap = 6.5; //cm
const float pixelNbrPerCm = 50.0;
const float far = 60.0;
const float near = 1.0;

/** Global variables */
//-- capture opencv
const cv::String face_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier face_cascade;
cv::VideoCapture *capture = NULL;
cv::Mat frame;

//-- display
bool bPause = false;                //- press ' ' to change
bool bFullScreen = true;            //- press 'f' to change
bool bInvertCam = false;            //- press 'i' to change
bool bDisplayCam = true;            //- press 'c' to change
bool bDisplayDetection = true;      //- press 'd' to change
bool bDisplayBox = true;            //- press 'b' to change
bool bPolygonMode = false;          //- press 'm' to change
bool bProjectionMode = true;        //- press 'p' to change
float camRatio = 0.3;               //- press '+/-' to change

//-- dimensions
int windowWidth;
int windowHeight;
float cx;
float cy;
int camWidth;
int camHeight;

//-- opengl camera
GLdouble glCamX;
GLdouble glCamY;
GLdouble glCamZ;
GLuint texname; 
/** Functions */
void redisplay();
using std::cerr;
using std::endl;
cv::Mat detectEyes(cv::Mat image);
void displayCam(cv::Mat camImage);

void setGlCamera();
void draw3dScene();
void drawFrame(float z);
void drawLineToInf(float x, float y, float z);
void drawAxes(float length);

float pixelToCm(int size);

void onReshape( int w, int h );
void onMouse( int button, int state, int x, int y );
void onKeyboard( unsigned char key, int x, int y );
void onIdle();
// For texture
const int ESCKEY = 27;        // ASCII value of escape character
const int img_width = 32;
const int img_height = 32;
const int startwinsize = 400; // Starting window width & height, in pixels
const double neardist = 0.5;  // Near & far clippng planes
const double fardist = 10.;
GLubyte the_image[img_height][img_width][3];
   // The image
   // 3rd subscript 0 = R, 1 = G, 2 = B


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
         glTexCoord2d(1.0, 1.0); glVertex3d( 1.0, 1.0, 0.0);
         glTexCoord2d(0.0, 1.0); glVertex3d(-1.0, 1.0, 0.0);
         glTexCoord2d(0.0, 0.0); glVertex3d(-1.0,-1.0, 0.0);
         glTexCoord2d(1.0, 0.0); glVertex3d( 1.0,-1.0, 0.0);
      glEnd();

   glPopMatrix();

   glutSwapBuffers();
}


// reshape
// The GLUT reshape function
// void reshape(int w, int h)
// {
//     glViewport(0, 0, w, h);

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(60, double(w)/h, neardist, fardist);

//    glMatrixMode(GL_MODELVIEW);  // Always go back to modelview mode
// }


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


// init
// Initializes GL states
// Called by main
// void init()
// {
//    glClearColor(1.0, 1.0, 1.0, 0.0);
    
//    glEnable(GL_DEPTH_TEST);

//    // Texture set-up

//    // Basic image set-up
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//    makeimage();
//     int width, height;
//     unsigned char* image;
//     GLuint textures[2];

// image = SOIL_load_image("img.png", &width, &height, 0, SOIL_LOAD_RGB);

          

//    // Generate a texture name and bind it to GL_TEXTURE_2D.
//    // Then we no longer use texname, since GL_TEXTURE_2D is bound to it.
//    // NOTE: binding a texture name is actually NOT necessary when we
//    //  use only one texture, but it does not hurt.
//     glGenTextures(1, &texname);
//     glBindTexture(GL_TEXTURE_2D, texname);

// //    // Set various texture parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  // Needs OpenGL 1.1
// //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
// //               GL_UNSIGNED_BYTE, image);
// //    // Create the actual texture
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height,
//                 0, GL_RGB, GL_UNSIGNED_BYTE, the_image);
// }
void init()
{
   glClearColor(1.0, 1.0, 1.0, 0.0);
   glEnable(GL_DEPTH_TEST);

   // Texture set-up

   // Basic image set-up
   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   makeimage();

   // Generate a texture name and bind it to GL_TEXTURE_2D.
   // Then we no longer use texname, since GL_TEXTURE_2D is bound to it.
   // NOTE: binding a texture name is actually NOT necessary when we
   //  use only one texture, but it does not hurt.
   glGenTextures(1, &texname);
   glBindTexture(GL_TEXTURE_2D, texname);

   // Set various texture parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);  // Needs OpenGL 1.1

   // Create the actual texture
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height,
                0, GL_RGB, GL_UNSIGNED_BYTE, the_image);
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, double(w)/h, neardist, fardist);

   glMatrixMode(GL_MODELVIEW);  // Always go back to modelview mode
}


/**
 * @function main
 */
int main( int argc, char **argv )
{
    // OPENCV INIT
    //-- Load the cascades
    if( !face_cascade.load( face_cascade_name ) )
    {
        fprintf(stderr,"-- (!) ERROR loading 'haarcascade_frontalface_alt.xml'\n");
        fprintf(stderr,"Please edit 'face_cascade_name' path in main.cpp:22 and recompile the project.\n");
        return -1;
    };

    // VIDEO CAPTURE
    //-- start video capture from camera
    capture = new cv::VideoCapture(0);

    //-- check that video is working
    if ( capture == NULL || !capture->isOpened() ) {
        fprintf( stderr, "Could not start video capture\n" );
        return 1;
    }

    // CAMERA IMAGE DIMENSIONS
    camWidth = (int) capture->get( CV_CAP_PROP_FRAME_WIDTH );
    camHeight = (int) capture->get( CV_CAP_PROP_FRAME_HEIGHT );

    // GLUT INIT
    glutInit( &argc, argv );
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    if(!bFullScreen){
        windowWidth = camWidth*1.5;
        windowHeight = camHeight*1.5;
        cx = pixelToCm(windowWidth);
        cy = pixelToCm(windowHeight);
        glutInitWindowPosition( 200, 80 );
        glutInitWindowSize( windowWidth, windowHeight );
    }
    glutCreateWindow( "ScreenReality - Vision 3D" );

    // RENDERING INIT
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normals

    // SCREEN DIMENSIONS
    if(bFullScreen)
    {
        glutFullScreen();
        windowWidth = glutGet(GLUT_SCREEN_WIDTH);
        windowHeight = glutGet(GLUT_SCREEN_HEIGHT);
        cx = pixelToCm(windowWidth);
        cy = pixelToCm(windowHeight);
        glViewport( 0, 0, windowWidth, windowHeight );
    }
 init();

   glutDisplayFunc(redisplay);
    // GUI CALLBACK FUNCTIONS
   // glutDisplayFunc( redisplay );
    glutReshapeFunc( onReshape );
    glutMouseFunc( onMouse );
    glutKeyboardFunc( onKeyboard );
    glutIdleFunc( onIdle );

    // GUI LOOP
    glutMainLoop();

    return 0;
}

/**
 * @function redisplay
 * (Called at each openGL step)
 * - Processes the webcam frame to detect the eyes with OpenCV,
 * - Creates a 3D scene with OpenGL,
 * - Render the scene and the webcam image.
 */
 void redisplay()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glEnable(GL_TEXTURE_2D);  // Turn on texture.
      // Turn it off if you draw something without a texture.

   // Draw square
   glPushMatrix();  // We are in model/view mode

      glTranslated(0., 0., -3.);      

      glBegin(GL_POLYGON);
         glTexCoord2d(1.0, 1.0); glVertex3d( 1.0, 1.0, 0.0);
         glTexCoord2d(0.0, 1.0); glVertex3d(-1.0, 1.0, 0.0);
         glTexCoord2d(0.0, 0.0); glVertex3d(-1.0,-1.0, 0.0);
         glTexCoord2d(1.0, 0.0); glVertex3d( 1.0,-1.0, 0.0);
      glEnd();

   glPopMatrix();

   glutSwapBuffers();
}
void redisplay2()
{
     
   //  // Turn on texture.
      // Turn it off if you draw something without a texture.
glEnable(GL_TEXTURE_2D);
   // Draw square
  

   
    if(frame.empty()) return;

    if(!bPause)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OPENCV
        //-- flip frame image
        cv::Mat tempimage;
        if(bInvertCam) cv::flip(frame, tempimage, 0);
        else cv::flip(frame, tempimage, 1);
        //-- detect eyes
        tempimage = detectEyes(tempimage);

        // OPENGL
        //-- scene
        setGlCamera();
        draw3dScene();
        //-- cam
        if(bDisplayCam) displayCam(tempimage);

        // RENDER
        glutSwapBuffers();
    }
    //-- post the next redisplay
    glutPostRedisplay();
}

/**
 * @function detectEyes
 * - Uses OpenCV to detect face
 * - Interpolate eyes position on image
 * - Computes eyes position in space
 * - Add some display for the detection
 */
cv::Mat detectEyes(cv::Mat image)
{
    // INIT
    std::vector<cv::Rect> faces;
    cv::Mat image_gray;
    cv::cvtColor( image, image_gray, CV_BGR2GRAY );
    cv::equalizeHist( image_gray, image_gray );

    // DETECT FACE
    //-- Find bigger face (opencv documentation)
    face_cascade.detectMultiScale( image_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE|CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(minFaceSize, minFaceSize) );

    for( size_t i = 0; i < faces.size(); i++ )
    {
        // DETECT EYES
        //-- points in pixel
        cv::Point leftEyePt( faces[i].x + faces[i].width*0.30, faces[i].y + faces[i].height*0.37 );
        cv::Point rightEyePt( faces[i].x + faces[i].width*0.70, faces[i].y + faces[i].height*0.37 );
        cv::Point eyeCenterPt( faces[i].x + faces[i].width*0.5, leftEyePt.y );

        //-- normalize with webcam internal parameters
        GLdouble normRightEye = (rightEyePt.x - camWidth/2)/f;
        GLdouble normLeftEye = (leftEyePt.x - camWidth/2)/f;
        GLdouble normCenterX = (eyeCenterPt.x - camWidth/2)/f;
        GLdouble normCenterY = (eyeCenterPt.y - camHeight/2)/f;

        //-- get space coordinates
        float tempZ = eyesGap/(normRightEye-normLeftEye);
        float tempX = normCenterX*glCamZ;
        float tempY = -normCenterY*glCamZ;

        //-- update cam coordinates (smoothing)
        glCamZ = (glCamZ*0.5) + (tempZ*0.5);
        glCamX = (glCamX*0.5) + (tempX*0.5);
        glCamY = (glCamY*0.5) + (tempY*0.5);

        // DISPLAY
        if(bDisplayCam && bDisplayDetection)
        {
            //-- face rectangle
            cv::rectangle(image, faces[i], 1234);

            //-- face lines
            cv::Point leftPt( faces[i].x, faces[i].y + faces[i].height*0.37 );
            cv::Point rightPt( faces[i].x + faces[i].width, faces[i].y + faces[i].height*0.37 );
            cv::Point topPt( faces[i].x + faces[i].width*0.5, faces[i].y);
            cv::Point bottomPt( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height);
            cv::line(image, leftPt, rightPt, cv::Scalar( 0, 0, 0 ), 1, 1, 0);
            cv::line(image, topPt, bottomPt, cv::Scalar( 0, 0, 0 ), 1, 1, 0);

            //-- eyes circles
            cv::circle(image, rightEyePt, 0.06*faces[i].width, cv::Scalar( 255, 255, 255 ), 1, 8, 0);
            cv::circle(image, leftEyePt, 0.06*faces[i].width, cv::Scalar( 255, 255, 255 ), 1, 8, 0);

            //-- eyes line & center
            cv::line(image, leftEyePt, rightEyePt, cv::Scalar( 0, 0, 255 ), 1, 1, 0);
            cv::circle(image, eyeCenterPt, 2, cv::Scalar( 0, 0, 255 ), 3, 1, 0);
        }
    }
    return image;
}

/**
 * @function setGlCamera
 * Set OpenGL camera parameters.
 * The off-axis projection is what gives
 * the feeling of augmented reality.
 */
void setGlCamera()
{
    if(bProjectionMode)
    {
        /* SKEWED FRUSTRUM / OFF-AXIS PROJECTION
        ** My implementation is based on the following paper:
        ** Name:   Generalized Perspective Projection
        ** Author: Robert Kooima
        ** Date:   August 2008, revised June 2009
        */

        //-- space corners coordinates
        float pa[3]={-cx,-cy,0};
        float pb[3]={cx,-cy,0};
        float pc[3]={-cx,cy,0};
        float pe[3]={glCamX,glCamY,glCamZ};
        //-- space points
        cv::Vec3f Pa(pa);
        cv::Vec3f Pb(pb);
        cv::Vec3f Pc(pc);
        cv::Vec3f Pe(pe);
        //-- Compute an orthonormal basis for the screen.
        cv::Vec3f Vr = Pb-Pa;
        Vr /= cv::norm(Vr);
        cv::Vec3f Vu = Pc-Pa;
        Vu /= cv::norm(Vu);
        cv::Vec3f Vn = Vr.cross(Vu);
        Vn /= cv::norm(Vn);
        //-- Compute the screen corner vectors.
        cv::Vec3f Va = Pa-Pe;
        cv::Vec3f Vb = Pb-Pe;
        cv::Vec3f Vc = Pc-Pe;
        //-- Find the distance from the eye to screen plane.
        float d = -Va.dot(Vn);
        //-- Find the extent of the perpendicular projection.
        float l = Va.dot(Vr) * near / d;
        float r = Vr.dot(Vb) * near / d;
        float b = Vu.dot(Va) * near / d;
        float t = Vu.dot(Vc) * near / d;
        //-- Load the perpendicular projection.
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(l, r, b, t, near, far+d);
        //-- Rotate the projection to be non-perpendicular.
        float M[16];
        memset(M, 0, 16 * sizeof (float));
        M[0] = Vr[0]; M[1] = Vu[0]; M[2] = Vn[0];
        M[4] = Vr[1]; M[5] = Vu[1]; M[6] = Vn[1];
        M[8] = Vr[2]; M[9] = Vu[2]; M[10] = Vn[2];
        M[15] = 1.0f;
        glMultMatrixf(M);
        //-- Move the apex of the frustum to the origin.
        glTranslatef(-pe[0], -pe[1], -pe[2]);
        //-- Reset modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
    else
    {
        //-- intrinsic camera params
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60, (float)windowWidth/(float)windowHeight, 1, 250);
        //-- extrinsic camera params
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(glCamX, glCamY, glCamZ, 0, 0, 0, 0, 1, 0);
    }
}
GLfloat angle;
 void _Timer(int value)
{
  /* increment angle */
  angle++;

  /* send redisplay event */
  glutPostRedisplay();

  /* call this function again in 10 milliseconds */
  glutTimerFunc(10, _Timer, 0);
}

/**
 * @function draw3dScene
 * Draws OpenGL 3D scene
 */

void draw3dScene2(){

     //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // glEnable(GL_TEXTURE_2D);
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // glBindTexture(GL_TEXTURE_2D, myTextureName);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-1, -3, -10);
        glutSolidTeapot(5);
    glTranslatef(1, 3, 10);
    drawLineToInf( -1, -3, -10);
 
    glDisable(GL_TEXTURE_2D);

}

void draw3dScene()
{
      glutTimerFunc(10, _Timer, 0); /* register callback for a timer */

    // DISPLAY MODE
    if(bPolygonMode){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawAxes(10.0);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
 
   // Draw square
  
    // BOUNDING BOX
    if(bDisplayBox){
        for(int i = 0; i <= 10; i++){
            float j = (float)i/10.0;
            //-- lines
            drawLineToInf((2*j*cx)-cx, cy, 0.0);//top lines
            drawLineToInf(cx, cy-(2*j*cy), 0.0);//right lines
            drawLineToInf(cx-(2*j*cx), -cy, 0.0);//bottom lines
            drawLineToInf(-cx, (2*j*cy)-cy, 0.0);//left lines
            //-- frames
            glColor3f(1.0-j,1.0-j,1.0-j);
            if((i%2) == 0) drawFrame(-j*far);
        }
    }

    // LIGHTING
    glEnable(GL_LIGHTING);
    //-- Add ambient light
    GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
    //-- Add positioned light
    GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
    //-- Add directed light
    GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f}; //Positioned at (-1, 0.5, 0.5)
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

    // GEOMETRY
    //-- TeaPot
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(-1, -3, -10);
        glutSolidTeapot(5);
    glTranslatef(1, 3, 10);
    drawLineToInf( -1, -3, -10);
    //-- Cube 1
    glColor3f(1.0f, 1.0f, 0.0f);
    glTranslatef(-10, -5, 30);
        glutSolidCube(3);
    glTranslatef(10, 5, -30);
    drawLineToInf(-10, -5, 30);
    //-- Cube 2
    glColor3f(1.0f, 0.0f, 1.0f);
    glTranslatef(-20, 0, -40);
        glutSolidCube(3);
    glTranslatef(20, 0, 40);
    drawLineToInf(-20, 0, -40);
    //-- Cube 3
    glColor3f(0.0f, 1.0f, 1.0f);
    glTranslatef(5, 5, 10);
        glutSolidCube(3);
    glTranslatef(-5, -5, -10);
    drawLineToInf(5, 5, 10);

    glDisable(GL_LIGHTING); //Disable lighting

}


/**
 * @function displayCam
 * Draws the webcam image in window + detection info
 */
void displayCam(cv::Mat camImage)
{
    //-- Save matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, windowWidth, 0.0, windowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    //-- Display Coordinates
    if(bDisplayDetection)
    {
        //-- Coord text
        std::stringstream sstm;
        sstm << "(x,y,z) = (" << (int)glCamX << "," << (int)glCamY << "," << (int)glCamZ << ")";
        std::string s = sstm.str();
        //std::cout<<s<<std::endl;

        //-- Display text
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2i(10,  windowHeight-(camRatio*camImage.size().height)-20);
        void * font = GLUT_BITMAP_9_BY_15;
        for (std::string::iterator i = s.begin(); i != s.end(); ++i)
        {
          char c = *i;
          glutBitmapCharacter(font, c);
        }
    }

    //-- Display image
    glRasterPos2i(0, windowHeight-(camRatio*camImage.size().height));
    cv::flip(camImage, camImage, 0);
    cv::resize(camImage, camImage, cv::Size(camRatio*camWidth, camRatio*camHeight), 0, 0, cv::INTER_CUBIC);
    glDrawPixels( camImage.size().width, camImage.size().height, GL_BGR, GL_UNSIGNED_BYTE, camImage.ptr() );

    //-- Load matrix
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/**
 * @function drawFrame
 * Draws lines between the corners of what we consider to be the screen
 */
void drawFrame(float z)
{
    glBegin(GL_LINE_LOOP);
        glVertex3f(cx, cy, z);
        glVertex3f(cx, -cy, z);
        glVertex3f(-cx, -cy, z);
        glVertex3f(-cx, cy, z);
    glEnd();
}

/**
 * @function drawLineToInf
 * Draws line from the (x,y,z) coordinate given to (x,y,far)
 */
void drawLineToInf(float x, float y, float z)
{
    glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 1.0f);
        glVertex3f(x, y, z);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(x, y, -far);
    glEnd();
}

/**
 * @function drawAxes
 ** Display the coordinate system
 */
void drawAxes(float length)
{
  glBegin(GL_LINES) ;
      glColor3f(1,0,0) ;
      glVertex3f(0,0,0) ;
      glVertex3f(length,0,0);

      glColor3f(0,1,0) ;
      glVertex3f(0,0,0) ;
      glVertex3f(0,length,0);

      glColor3f(0,0,1) ;
      glVertex3f(0,0,0) ;
      glVertex3f(0,0,length);
  glEnd() ;
}

float pixelToCm(int size)
{
    return (float)size/pixelNbrPerCm;
}

/**
 * @function onReshape;
 ** Adapts the viewport to the window size when the window is reshaped
 */
void onReshape( int w, int h )
{
    
    windowWidth = w;
    windowHeight = h;
    cx = pixelToCm(windowWidth);
    cy = pixelToCm(windowHeight);
    glViewport( 0, 0, windowWidth, windowHeight );
}

/**
 * @function onMouse
 */
void onMouse( int button, int state, int x, int y )
{
  if ( button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {

    }
}

/**
 * @function onKeyboard
 */
void onKeyboard( unsigned char key, int x, int y )
{
    if( bFullScreen && ((key == 'f' || key == 'F') || ((int)key == 27)))
    {
        glutReshapeWindow(camWidth*1.5, camHeight*1.5);
        glutPositionWindow( 200, 80);
        bFullScreen = false;
    }
    else if(!bFullScreen && (key == 'f' || key == 'F'))
    {
        glutFullScreen();
        bFullScreen = true;
    }
    else switch ( key )
    {
        // invert cam ortientation
        case 'i': bInvertCam = !bInvertCam; break;
        case 'I': bInvertCam = !bInvertCam; break;

        // change cam display
        case 'c': bDisplayCam = !bDisplayCam; break;
        case 'C': bDisplayCam = !bDisplayCam; break;

        // change detection display
        case 'd': bDisplayDetection = !bDisplayDetection; break;
        case 'D': bDisplayDetection = !bDisplayDetection; break;

        // change cam ratio
        case '+': if(camRatio < 1.8) camRatio += 0.1 ; break;
        case '-': if(camRatio > 0.2) camRatio -= 0.1; break;

        // change homography correction
        case 'b': bDisplayBox = !bDisplayBox; break;
        case 'B': bDisplayBox = !bDisplayBox; break;

        // change axes display
        case 'm': bPolygonMode = !bPolygonMode; break;
        case 'M': bPolygonMode = !bPolygonMode; break;

        // change projection mode
        case 'p': bProjectionMode = !bProjectionMode; break;
        case 'P': bProjectionMode = !bProjectionMode; break;

        // change projection mode
        case ' ': bPause = !bPause; break;

        // quit app
        case 'q': exit(0); break;
        case 'Q': exit(0); break;

        default: break;
    }
}

/**
 * @function onIdle
 * (Called at each openGL step)
 * Updates the 'frame' image from the captured video
 */
void onIdle()
{
    if(!bPause)
    {
        (*capture) >> frame;
    }
}

