#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII 1
#define INV 2
#define BLUR 3
//
// MATH 442: pgmdo
//
// Simple portable grey map (PGM) image processor.
//


// outASCII
// 
// Output a character to the given file, one that
// corresponds in brightness to the value (from 0.0
// to 1.0) given.
//
void outASCII(FILE *f, float value) {
  if (value > 0.8) {
    fprintf(f,"#");
  } else if (value > 0.5) {
    fprintf(f,"x");
  } else if (value > 0.25) {
    fprintf(f,";");
  } else if (value > 0.15) {
    fprintf(f,",");
  } else {
    fprintf(f," ");
  }
}

// eatLine
//
// Reads a line of characters.
//
void eatLine(FILE *f) {
  while (fgetc(f) != '\n');
}


// echoASCII_v1
//
// Read the a PGM file opened as "inf" and write a text file
// of characters to "outf", ones whose brightness suggest the
// levels of grey specified by the original image.
//

void echoASCII(float *imgA, int rows, int cols, FILE *outf) {
  int p;
  float pix;
  for(int r=0; r<rows; r++) {
    for(int c=0;c<cols; c++) {
      p = c + r*cols;
      pix = imgA[p];
      outASCII(outf,pix);
    }
    // end of row
    fprintf(outf,"\n");
  }
}


void echoASCII_v1(FILE *inf, FILE *outf) {

  int width, height;
  int max;
  int pixel;
  int r,c;

  // Read the PGM file's header info,
                             // for example: 
       
  eatLine(inf);                // P5 (or P2)
  eatLine(inf);                // # this was produced by some software
  fscanf(inf, "%d", &width);   // 9 5
  fscanf(inf, "%d", &height);
  fscanf(inf, "%d", &max);     // 255

  // for each image row
  for (r=0; r<height; r++) {
    // for each image column
    for (c=0; c<width; c++) {
      
      // read a PGM pixel grey value (from 0 to max)
      fscanf(inf,"%d", &pixel);
                             // 0 10 30 80 120 135 225 245 255 
                             // (4 more rows)

      // output a pixel character to make ASCII art
      outASCII(outf,(float)pixel/(float)max);
    }

    // end the ASCII text line 
    fprintf(outf,"\n");
  }
}




void invert(float *imgA, int rows, int cols, FILE *outf) {

  int p;

  fprintf(outf,"This file has been 'C'zed \n by a digital inversion pirate. Bwahaha! \n");
  fprintf(outf,"%d %d \n", cols, rows);
  fprintf(outf,"%d \n",255);

  for (int r=0; r < rows; r++) {
    for (int c=0; c < cols; c++) {
      p = c + r*cols;
      imgA[p] = 255*(1 - imgA[p]);
      fprintf(outf,"%d\n",(int)imgA[p]);
    }
  }
}


void blur(float *imgA, int rows, int cols, FILE *outf) {

  int p;
  int res;

  fprintf(outf,"This file has been 'C'zed \n by la vie en blur! \n");
  fprintf(outf,"%d %d \n", cols, rows);
  fprintf(outf,"%d \n",255);

  for (int r=0; r < rows; r++) {
    for (int c=0; c < cols; c++) {

      // The following is pretty gruesome.
      // Pixels are blurred differently depending
      //   upon their location on the corner, perim,
      //   or in the middle.

      /***********
	  BEGIN IFs
      ************/
      if (r == 0) {
	if (c == 0) {
	  // Blur the top left corner
	  imgA[0] = (imgA[0] + imgA[1] + imgA[cols])/3;

	  // Store the .pgm value
	  res = (int)255*imgA[0];

	} else if (c==cols-1) {
	  // Blur the top right corner
	  imgA[cols-1] = (imgA[cols-2] + imgA[cols-1] + imgA[2*cols -1])/3;
	  res = (int)255*imgA[cols -1];
	} else {
	  // Blur the top row (not corners)
	  imgA[c] = (imgA[c-1] + imgA[c] + imgA[c+1] + imgA[2*c])/4;
	  res = (int)255*imgA[c];
	}
      } else if (r == rows-1) {
	if (c==0) {
	  // Blur bottom left corner
	  imgA[cols*r] = (imgA[cols*(r-1)] + imgA[cols*r] + imgA[cols*r + 1])/3;
	  res = (int)255*imgA[cols*r];
	} else if (c==cols-1) {
	  // Blur bottom right corner
	  imgA[r*c] = (imgA[r*c - 1] + imgA[r*c] + imgA[r*c - cols])/3;
	  res = (int)255*imgA[r*c];
	} else {
	  //Blur bottom rows
	  p = c + r*cols;
	  imgA[p] = (imgA[p -1] + imgA[p] + imgA[p + 1] + imgA[p-cols])/4;
	  res = (int)255*imgA[p];
	}
      } else {
	if (c==0) {
	  // Blur left edge, not corners
	  p = r*cols;
	  imgA[p] = (imgA[p] + imgA[p+1] + imgA[p - cols] + imgA[p + cols])/4;
	  res = (int)255*imgA[p];
	} else if (c==cols-1) {
	  // Blur right edge, not corners
	  p = r*cols + cols -1;
	  imgA[p] = (imgA[p-1] + imgA[p] + imgA[p - cols] + imgA[p + cols])/4;
	  res = (int)255*imgA[p];
	} else {
	  // Blur pixel with four adjacent pixels
	  p = c + r*cols;
	  imgA[p] = (imgA[p-1] + imgA[p] + imgA[p+1] + imgA[p-cols] + imgA[p+cols])/5;
	  res = (int)255*imgA[p];
	}
      }
      /********
	  ENDIF
      *********/

      fprintf(outf,"%d\n",res);

    }
  }
}

void *readImage(FILE *inf, int opt, FILE *outf){ 

  int width, height; 
  int max;
  int r,c;
  int pixel;

  // Process the pgm file as a bufft
  // I.e., eat the header
  eatLine(inf);
  eatLine(inf);
  fscanf(inf,"%d",&width);  // the first integer on line 1 is the width
  fscanf(inf,"%d",&height); // second is the height
  fscanf(inf,"%d",&max);   // third is max number of pixels

  float *array = malloc(sizeof(float)*width*height);
  int p;
  // iterate through rows and columns
  // scan in graymap values, convert to pixels
  // fill array with pixel values
  for (r=0; r < height; r++) {
    for (c=0; c < width; c++) {
      fscanf(inf,"%d",&pixel);
      p = c + r*width;
      array[p] = (float)pixel/(float)max;
    }
  }

  // Select function to call
  // 
  switch (opt) {

  case(ASCII):
    echoASCII(array,height,width,outf);
    break;

  case(INV):
    invert(array,height,width,outf);
  case(BLUR):
    blur(array,height,width,outf);
  }
  // exit(-1);
} 


// usage
//
// Outputs a message that gives the command-line description.
//
void usage(char *cmd) {
  fprintf(stderr,"usage: %s <option> <infile> <outfile>\n", cmd);
  fprintf(stderr,"\twhere <infile> is a PGM and <option> is one of\n");
  fprintf(stderr,"\t\t--blur    :outputs a blurred PGM image\n");
  fprintf(stderr,"\t\t--invert  :outputs PGM negative image\n");
  fprintf(stderr,"\t\t--ascii   :converts to ASCII art\n");
}

// main
// 
// This program accepts three arguments: a processing option
// ("blur", "invert", or "ascii"), a PGM file name for input,
// and a text file name for output.  It reads the PGM file and
// creates an output file with either an appropriate PGM (if 
// one of the first two options are given) or a text file (if
// the last option is given).
//
// Right now, only the "ascii" option works.
//
int main(int argc, char **argv) {

  // input and output file "handles"
  FILE *inf, *outf;

  if (argc < 4) {
    
    // whoops! not enough arguments
    fprintf(stderr,"Error: not enough arguments!\n");
    usage(argv[0]);
    return -1;

  } else {

    // open the input (PGM) file
    inf = fopen(argv[2],"r");
    if (inf == NULL) {
      fprintf(stderr,"Error: can't open file '%s' for reading.\n",argv[2]);
      return -1;
    }

    // open the output file
    outf = fopen(argv[3],"w");
    if (outf == NULL) {
      fprintf(stderr,"Error: can't open file '%s' for writing.\n",argv[3]);
      return -1;
    }

    if (strcmp(argv[1],"--blur") == 0) {

      readImage(inf,BLUR,outf);

      // fprintf(stderr, "***BLUR NOT SUPPORTED YET!!!***\n");


    } else if (strcmp(argv[1],"--invert") == 0) {

      readImage(inf,INV,outf);

      // fprintf(stderr, "***INVERT NOT SUPPORTED YET!!!***\n");


    } else if (strcmp(argv[1],"--ascii") == 0) {

      readImage(inf,ASCII,outf);
      //
      // change this so that it is given an image array and
      // the outf
      //

    } else {

      fprintf(stderr,"Error: unrecognized option '%s.'\n",argv[1]);
      usage(argv[0]);
      
      // return FAIL
      return -1;
    }

    // close the files
    fclose(inf);
    fclose(outf);

    // return OK
    return 0;
  }
}
  


/* NOT IN USE: I wrote this before I understood how the character buffer worked
// imgSpec
// Function that returns the given .pgm's 
//   - height (specified by the HTOPT option),
//   - width  (specified by the WDOPT option),   or
//   - max    (specified by the MAXOPT option),
// uses the code from echoASCII to read the file
int imgSpec(FILE *f, int option) {
  int width, height;
  int max;
  
  switch (option) {
  case(MAXOPT):
    fscanf(inf, "%d", &width);   // 9 5
    fscanf(inf, "%d", &height);
    fscanf(inf, "%d", &max);     // 255
    return max;
  case(WDOPT):
    return width;
  case(HTOPT):
    return height;
  }
  return -1;
}
*/
