#include <stdio.h>
#include <GL/glut.h> // The GL Utility Toolkit (Glut) Header
#include <GL/glaux.h>

void terrain();


GLfloat light_position[] = { 0.0, 2.0, 0.0, 1.0 };
GLfloat mat_amb_diff_r[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_amb_diff_b[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 100.0 };
GLfloat mat_emission[] = {1, 1, 1, 1.0};

GLuint	texture[1];	// Storage For One Texture ( NEW )


int b=0;


AUX_RGBImageRec *LoadBMP(char *Filename)	// Loads A Bitmap Image
{
FILE *File=NULL;	// File Handle
if (!Filename)	// Make Sure A Filename Was Given
{
return NULL;	// If Not Return NULL
}
File=fopen(Filename,"r");	// Check To See If The File Exists
if (File)	// Does The File Exist?
{
fclose(File);	// Close The Handle
return auxDIBImageLoad(Filename);	// Load The Bitmap And Return A Pointer
}
return NULL;	// If Load Failed Return NULL
}

int LoadGLTextures()	// Load Bitmaps And Convert To Textures
{
int Status=FALSE;	// Status Indicator
AUX_RGBImageRec *TextureImage[1];	// Create Storage Space For The Texture
memset(TextureImage,0,sizeof(void *)*1);	// Set The Pointer To NULL

// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
if (TextureImage[0]=LoadBMP("wall.bmp"))
{
Status=TRUE;	// Set The Status To TRUE
glGenTextures(1, &texture[0]);	// Create The Texture

// Typical Texture Generation Using Data From The Bitmap
glBindTexture(GL_TEXTURE_2D, texture[0]);

// Generate The Texture
glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);

glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTE R,GL_LINEAR);	// Linear Filtering
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTE R,GL_LINEAR);	// Linear Filtering
}

if (TextureImage[0])	// If Texture Exists
{
if (TextureImage[0]->data)	// If Texture Image Exists
{
free(TextureImage[0]->data);	// Free The Texture Image Memory
}
free(TextureImage[0]);	// Free The Image Structure
}
return Status;	// Return The Status
}

void init ( GLvoid ) // Create Some Everyday Functions
{
glClearColor (0,0,0,1);

if ( !LoadGLTextures() ) {
MessageBox( NULL, "Cannot Load - wall.bmp", "Error", MB_ICONINFORMATION | MB_OK );
exit( 0 );
}
glEnable (GL_TEXTURE_2D);
glBindTexture( GL_TEXTURE_2D, texture[0] );


glClearDepth(1.0f);	// Depth Buffer Setup
glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
glDepthFunc(GL_LEQUAL);	// The Type Of Depth Testing To Do

glEnable ( GL_COLOR_MATERIAL );
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

glShadeModel (GL_SMOOTH);

glLightfv(GL_LIGHT0, GL_POSITION, light_position);
glEnable(GL_LIGHTING);
glEnable(GL_LIGHT0);
glEnable(GL_DEPTH_TEST);
} 

void display ( void ) // Create The Display Function
{
glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
glMatrixMode (GL_MODELVIEW); 
glLoadIdentity(); 

gluLookAt (0,5,20,0,0,0,0,1,0); 

b++;
glRotated (b,0,0,0);


glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff_r);
glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

glBegin( GL_POLYGON );
glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -0.6f, -0.5f, 0.5f );
glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 0.6f, -0.5f, 0.5f );
glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 0.6f, 0.5f, 0.5f );
glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -0.6f, 0.5f, 0.5f );
glEnd();


glutSwapBuffers ( );
// Swap The Buffers To Not Be Left With A Clear Screen
}

void reshape ( int w, int h ) // Create The Reshape Function (the viewport)
{
glViewport ( 0, 0, w, h );
glMatrixMode ( GL_PROJECTION ); // Select The Projection Matrix
glLoadIdentity ( ); // Reset The Projection Matrix
if ( h==0 ) // Calculate The Aspect Ratio Of The Window
gluPerspective ( 45, ( float ) w, 1.0, 100.0 );
else
gluPerspective ( 45, ( float ) w / ( float ) h, 1.0, 100.0 );
glMatrixMode ( GL_MODELVIEW ); // Select The Model View Matrix
glLoadIdentity ( ); // Reset The Model View Matrix
}

void keyboard ( unsigned char key, int x, int y ) // Create Keyboard Function
{
switch ( key ) {
case 27: // When Escape Is Pressed...
exit ( 0 ); // Exit The Program
break; // Ready For Next Case
default: // Now Wrap It Up
break;
}
}


void arrow_keys ( int a_keys, int x, int y ) // Create Special Function (required for arrow keys)
{
switch ( a_keys ) {
case GLUT_KEY_DOWN: // When Down Arrow Is Pressed...
glutReshapeWindow ( 500, 500 ); // Go Into A 500 By 500 Window
break; 
case GLUT_KEY_UP: // When Up Arrow Is Pressed...
glutFullScreen ( ); // Go Into Full Screen Mode
break;
default:
break;
}
}

void main ( int argc, char** argv ) // Create Main Function For Bringing It All Together
{
glutInit ( &argc, argv ); // Erm Just Write It =)
init ();
glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE ); // Display Mode
glutInitWindowSize ( 640, 480 ); // If glutFullScreen wasn't called this is the window size
glutInitWindowPosition (100, 100);
glutCreateWindow ( "NeHe's OpenGL Framework" ); // Window Title (argv[0] for current directory as title)
//glutFullScreen ( ); // Put Into Full Screen
glutDisplayFunc ( display ); // Matching Earlier Functions To Their Counterparts
glutIdleFunc( display );
glutReshapeFunc ( reshape );
glutKeyboardFunc ( keyboard );
glutSpecialFunc ( arrow_keys );
glutMainLoop ( ); // Initialize The Main Loop
}