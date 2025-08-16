#include <stdio.h>
#include <stdlib.h>

void printDef(char *name)
{char buff[160];
 char *c;
 int i;
 for (c=name+strlen(name);c>name && *c!='\\';c--) ;
 if (*c=='\\')
    c++;
 strcpy(buff,c);
 for (i=0;i<strlen(buff);i++)
    if (buff[i]>='a' && buff[i]<='z')
       buff[i]+='A'-'a';

 printf("File %s\n",buff);
 printf("    FileSource %s\n",name);
 printf("    EndFileSource\n");
 printf("EndFile\n");
}

char *audioList[]=
{
 "KARNAK.RED",
 "TRIBAL.RED",
 "SELKIS.RED",
 "SWAMP.RED",
 "ROCKIN.RED",
 "QUARRY.RED",
 "MAGMA.RED",
 "SANCTUM.RED",
 "ENDCREDT.RED",
 "KILMAAT1.RED",
 "KILMAAT2.RED",
 "WATER2.RED",
 "MAP.RED",
 NULL
};


int main(int argc,char **argv)
{int i;
 printf("Disc SRUINS.DSK\n"
	"CatalogNo 0\n"
	"Session CDROM\n"
	"LeadIn MODE1\n"
	"EndLeadIn\n"
        "SystemArea %s\n"
	"Track MODE1\n"
	"\n"
        "Volume ISO9660 TEST.PVD\n"
	"PrimaryVolume 0:2:16   ;location of first file\n"
	"SystemIdentifier \"SEGA SEGASATURN\"\n"
	"VolumeIdentifier \"%s\"\n"
	"VolumeSetIdentifier \"%s\"\n"
	"PublisherIdentifier \"%s\"\n"
	"DataPreparerIdentifier \"LOBOTOMY SOFTWARE\"\n"
	"CopyrightFileIdentifier \"NITS_CPY.TXT\"\n"
	"AbstractFileIdentifier \"NITS_ABS.TXT\"\n"
	"BibliographicFileIdentifier \"NITS_BIB.TXT\"\n"
	"EndPrimaryVolume\n"
        "EndVolume\n"
        "\n",argv[1],argv[2],argv[2],argv[3]);
 /* print file defs */
 for (i=4;i<argc;i++)
    printDef(argv[i]);

 printf("\n"
	"; this is where you would place all of your datafiles\n"
	"\n"
	"\n"
        "PostGap 150\n"
	"\n"
	"EndTrack\n"
	"\n"
	"; take the following comments out if you wish to include\n"
	"; redbook audio tracks\n"
	"\n");
 
 for (i=0;audioList[i];i++)
    printf("Track CDDA\n"
	   "Pause 150\n"
           "FileSource d:\\saturn\\red\\%s\n"
	   "EndFileSource\n"
	   "EndTrack\n"
           "\n",audioList[i]);

 printf("LeadOut CDDA   ;change this to CDDA if you include redbook\n"
	"Empty 500\n"
	"EndLeadOut\n"
	"EndSession\n"
	"EndDisc\n");

 return 0;
}

