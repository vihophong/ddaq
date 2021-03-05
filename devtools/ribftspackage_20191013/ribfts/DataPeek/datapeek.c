/*****************************************************************************
*  datacopy.c       Uses the dataspy library
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dataspy.h>


void Usage(char *progname) {
    fprintf(stderr,"Usage\n%s -I [ID] -d [data_width]\n",progname);
    fprintf(stderr,"\tID is the number of the data stream to peek (default 0)\n");
    fprintf(stderr,"\td is the width of data in the buffer. Supports 16, 32 or 64 bits. Default = 32\n");
    exit(1);
}

main (int argc, char **argv)
{

        short buffer16 [32*1024];
     	int buffer32 [16*1024];
     	long long buffer64 [8*1024];

	int i,j,k;
        int x;
	int ID = 0;
        int d = 32;
        int v = 0;


    if (argc >1) {
        for(i=1;i <argc;i++) {
            if ( (argv[i][0] == '-') || (argv[i][0] == '/') ) {
                switch(argv[i][1]) {
                    case 'I':
                        ID = atoi(argv[++i]);
                        break;
                    case 'd':
                        d = atoi(argv[++i]);
                        break;
                    case 'v':
                        v = atoi(argv[++i]);
                        break;
                    default:
                        Usage(argv[0]);
                        break;
                }
            }
        }
    }

        dataSpyVerbose(v);


	i = dataSpyOpen (ID);
	printf ("open = %d\n", i);

retry:

               switch(d) {
                  case 16:
                     x = dataSpyRead (ID, (char*)buffer16, 64*1024);   /* read 64 Kbytes */
                  break;
                  default:
                  case 32:
                     x = dataSpyRead (ID, (char*)buffer32, 64*1024);
                  break;
                  case 64:
                     x = dataSpyRead (ID, (char*)buffer64, 64*1024);
                  break;
                }

                if (x == 0) {sleep(1); goto retry;}

		if (x > 0)
		{
 
			printf ("read %d\n", x);
                        k=0;

        for (i = 0; i < 64; i++) {
          for (j = 0; j < 8; j++) {

               switch(d) {
                  case 16:
                     printf(" 0x%04x", buffer16[k] & 0x0000ffff);
                  break;
                  default:
                  case 32:
                     printf(" 0x%08lx", buffer32[k] & 0x00000000ffffffff);
                  break;
                  case 64:
                     printf(" 0x%016llx", buffer64[k]);
                  break;
                }

            k++;
          }
          printf ("\n");
        }
        printf ("\n");

               } else {printf ("no data\n");}

	i = dataSpyClose (ID);
	printf ("\nclose\n");
}


