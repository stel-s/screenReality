/* fopen example */
#include <stdio.h>
int main ()
{
  FILE * pFile;
  pFile = fopen ("img.bmp","rb");
   if ( !pFile ) {
      fprintf(stderr, "Unable to open file: %s\n", "34");
    
      return false;
   }
  if (pFile!=NULL)
  {
    fputs ("fopen example",pFile);
    fclose (pFile);
  }
  return 0;
}