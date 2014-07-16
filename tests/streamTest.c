#include <stdio.h>
#include <stdlib.h>


void eatline(FILE *f) {
  while (fgetc(f) != '\n');
}

int getWidth(FILE *f) {
  int wd;
  eatline(f);
  eatline(f);
  fscanf(f,"%d",&wd);
  return wd;
}
int getHeight(FILE *f) {
  int ht;
  fscanf(f,"%d",&ht);
  return ht;
}
int getMax(FILE *f) {
  int max;
  fscanf(f,"%d",&max);
  return max;
}

int main(int argc, char **argv) {

  FILE *inf;
  FILE *outf;
  
  inf = fopen(argv[1],"r");
  printf("%d width %d height %d max",getWidth(inf),getHeight(inf),getMax(inf));

  

}
