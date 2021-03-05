/*****************************************************************************
*  datacopy.c       Uses the dataspy library
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dataspy.h>


int OutFlag =0;     /* no file output by default  */
FILE *OutStream;
char *FileName= (char*)"/tmp/copy.dat";

void Usage(char *progname) {
    fprintf(stderr,"Usage\n%s -f [file path name] - i [ID]\n",progname);
    fprintf(stderr,"\tfile path name is root of the file name used to store data  (default null and no data is stored)\n");
    fprintf(stderr,"\tID is the number of the data stream to peek (default 0)\n");");
    fprintf(stderr,"\tlimit is the number of data buffers to handle (default 100)\n");
    exit(1);
}

main (int argc, char **argv)
{
	unsigned int buffer [16*1024];
        unsigned int data;
	int i,j,n, x;
	char *c;
	long *l;
	int ID = 0;
        int count = 0;
        int limit = 100;
        int verbose = 0;


    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(argv[i][1]) {
                    case 'f':
                        FileName = argv[++i];
                        printf("Writing to %s\n",FileName);
                        OutFlag = 1;
                        OutStream = fopen(FileName,"w");
                        break;
                    case 'I':
                        ID = atoi(argv[++i]);
                        break;
                    case 'l':
                        limit = atoi(argv[++i]);
                        break;
                    case 'v':
                        verbose = atoi(argv[++i]);
                        break;
                    default:
                        Usage(argv[0]);
                        break;
                }
            }
        }
    }


        dataSpyVerbose(verbose);

	i = dataSpyOpen (ID);
	printf ("open = %d\n", i);

	for (i = 0; i < limit; )
	{
		x = dataSpyRead (ID, (char*)buffer, 64*1024);

		if (x > 0)
		{
                        i++;
			if (verbose != 0) printf ("read %d\n", x);
                        count++;

                        for (j = 0; j < 48;  j= j+16)
                        {

			      for (n=0;n<16;n++)
                              {
                                   data = buffer[j+n] & 0x00000000ffffffff;
			           if (OutFlag == 0) printf("0x%08lx ", data);
                              }
                        }
                        if (OutFlag == 0) printf("\n");

                     if (OutFlag != 0)  fwrite(buffer, 1, x, OutStream);

		}
		else  sleep(1);
	}
	i = dataSpyClose (ID);
        if (OutFlag != 0) fclose(OutStream);
	printf ("\nclose = %d after %d blocks\n", i, count);
}


