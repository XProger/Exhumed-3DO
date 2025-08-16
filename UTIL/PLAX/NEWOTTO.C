#include <mem.h>
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#define VOID	0	// My NULL 
#define GOOD	1	// My TRUE 
#define EVIL	0	// My FALSE 

typedef unsigned long   ULONG;
typedef unsigned short 	UWORD;
typedef unsigned char 	UBYTE;

#include "modex.c"
#include "anm.c"
#include "trigdat.c"

short page=0;
UWORD Frame;
PALETTE palette[256];

#pragma aux setvmode =\
	"int 0x10",\
	parm [eax]\

void initScreen()
{
	set320x240x256_X();
	setVisiblePage(page);
	WritePalette(palette);
	page = (page+1)&1;
	setActivePage(page);
}

void PrintValue (FILE *file, int trigger, char *new, char *form, ...)
{
	va_list arg;
	va_start(arg, form);	// Initialize arument list

	if (trigger)
		fprintf(file,",");
	else	
		fprintf(file,"\n\t%s\t",new);

	fprintf(file,form,va_arg(arg,char *));
}


//int DetectChar (x, y)
//{
//  int x1, y1;
//
//  for (y1=0; y1<8; y1++)
//    for (x1=0; x1<8; x1++)
//      if (Bitmap[y1+y][x1+x]%COLORS)
//        return GOOD;
//  return EVIL;  
//}   


void clearScreen()
{
	int x,y;
	for (y=0;y<height;y++)
		for (x=0;x<width;x++)
			putPixel_X(x,y,255);
}

void writePaletteFile(char *filename, PALETTE *palette)
{
	FILE *file;
	unsigned short color;
	int	i;

  file = fopen(filename, "wt");
	
	fprintf (file, "{\n");
	for (i=0; i<256; i++)
	{
		color=(palette[i].r>>3)+(palette[i].g>>3<<5)+(palette[i].b>>3<<10);
		fprintf (file, " 0x%04x", color);
		if (i<255) fprintf (file, ",");
		fprintf (file, "\n");
	}
	fprintf (file, "};\n");

//  file = fopen(filename, "wb");
//  for (i=0; i<256; i++)
//	{
//		color=(palette[i].r>>3)+(palette[i].g>>3<<5)+(palette[i].b>>3<<10);
//		fwrite (&color, 2, 1, file);
//	}
    fclose (file);
}

void main(int argc, char *argv[])
{
	ANIM *Anim;
	UWORD Xmax, Ymax, x, y, z, count;
	UWORD i, infile, outfile;
	FILE *file;
	char filename[80], *ptr;
	char drive[_MAX_DRIVE],dir[_MAX_DIR],fname[_MAX_FNAME],ext[_MAX_EXT];
	char fname2[10];
	short	xcenter, ycenter;

	if (argc<2)
	{
		_splitpath(argv[0], drive, dir, fname, ext);
		printf("\nUsage : %s [options] <anim file> [output file]\n", fname);
    printf("Version 06/13/95\n");
		printf("Makes palette and tile files from anim file.\n");
    printf("\nWritten by Jeff Blazier.\n");
    printf("\nSalvaged from code Written by John Yuill.\n");
		exit(0);
	}

  outfile=infile=0;
  for (i=1; i<argc; i++) 
  {
    ptr=strlwr(argv[i]);
    if (*ptr=='/')
    {
      while (*(++ptr))                        
      {
        switch(*ptr)
        {
          case ('b'):
              printf("Tile Bunching Off!\n");
              break;
          case ('t'):
              printf("Tile Order Character Fill!\n");
              break;
                default  :
              printf("Unrecognized Command '%c'!\n", *ptr);
              exit(0);
        }
      }
    }
    else
    {
      if (!infile)
        infile=i;
			else
      	outfile=i;
    }      
  }
   
  if (!infile)
	{
		printf("No input file specified!\n");
		exit(0);
	}

	if (!outfile)
		outfile=infile;
 
  _splitpath(argv[infile], drive, dir, fname, ext);

	if (*ext && strcmp(strupr(ext), ".ANM"))
	{
		printf("File extension '%s' is unrecognized!\n", ext);		    	
		exit(0);
	}	
	strcat(strcpy (filename, fname), ".anm");	// Build up Anim name

	if (!(Anim=CreateAnim(filename)))
		exit(0);
                         
	ConvertPaletteFromDA (Anim->A.palette, palette);

  _splitpath(argv[outfile], drive, dir, fname, ext);

	strcat(strcpy (filename, fname), ".pal");
	writePaletteFile (&filename,&palette);

	printf("Animation contains %d screens.\n",Anim->LastFrame);

	initScreen();

 	for (Frame=0; Frame<Anim->LastFrame; Frame++)
 	{
		clearScreen();

    DrawAnimPage(Anim);
        
    Xmax = Ymax = 0;

    for (y=0; y<200; y++)       
		{
      for (x=0; x<320; x++)
			{
				if (Anim->PictureBuffer[320*y + x]!=255)
				{
					if (x>Xmax)
						Xmax=x;
				
					if (y>Ymax)
						Ymax=y;
				}
			}
		}				  
	
		Xmax=(Xmax+4)&0xfffc;
		Ymax=(Ymax+4)&0xfffc;
	
		fname[5]=0;
		sprintf(fname2, "%03d.til", Frame);
		strcat(strcpy (filename, fname), fname2);
  
  	file = fopen(filename, "wt");
//  	file = fopen(filename, "wb");

//		fwrite(&Xmax, 2, 1, file);
//		fwrite(&Ymax, 2, 1, file);

		z=320*(Ymax-1)+Xmax-1;

		xcenter=(width-Xmax)/2;
		ycenter=(height-Ymax)/2;
		
		fprintf (file, "%s", "{\n");

		for (y=0; y<Ymax; y++)
		{
			count = y*Xmax;
			for (x=0; x<Xmax; x++)
			{
				i=320*y+x;
				fprintf (file, " 0x%02x", Anim->PictureBuffer[i]);
				if (i<z)
					fprintf (file, ",");
				if (((x+count)&15)==15)
					fprintf (file, "\n");
//				fwrite (&(Anim->PictureBuffer[i]), 1, 1, file);
				putPixel_X(xcenter+x,ycenter+y,Anim->PictureBuffer[i]);
			}
		}

		fprintf (file, "%s", "};\n");
	
		fclose (file);

		setVisiblePage(page);
		page = (page+1)&1;
  	setActivePage(page);

	}
	
	DeleteAnim(Anim);

  setvmode(0x3); 
}


