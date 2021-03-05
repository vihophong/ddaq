#include <stdio.h>
#include <stdlib.h>
#include "typedefs.h"
#include "f_histo_io.h"
#include "f_find_peaks.h"

main(int argc, char **argv)
{
  int i,iret=0,*pi,type;
  head_histobin h_head; // file header
  void *buffer;
  // for peak finder:
  int npeaks=0,binsum=1,ipmin[100],ipmax[100];
  double dpmin[100],dpint[100],dpwidth[100],noisefact=1.0,noisemin=1.0;

  // The peak finder steps through the data field and checks if the
  // bin contents are inside a noise band. The noise band is derived
  // from +- noisefact*SQRT(content) > noisemin. 
  // If a bin content is obove or below the band,
  // the band is calculated new from current content. A peak is found
  // if after leaving the band upstairs it is left downstairs. The last up and
  // down mark the interval of a maximum. Then the first and second moments
  // of this interval are calculated giving the peak and width.
  // Binsum allows for summing up bins (smoothing). 

  if(argc == 1) {printf("file [noisefact [noisemin [binsum]]]\n");return;}

  if(argc > 2)sscanf(argv[2],"%lf",&noisefact); // lf for double
  if(argc > 3)sscanf(argv[3],"%lf",&noisemin);
  if(argc > 4)sscanf(argv[4],"%d",&binsum);

  // read file header to get spectrum size and type
  iret=f_histo_head(argv[1],&h_head,1);
  if(iret) { printf("Cannot read %s\n",argv[1]);return;}

  if(h_head.isfloat) type=TYPE__FLOAT;
  else               type=TYPE__INT;

  buffer=(void *)malloc(h_head.itembytes);

  iret=f_histo_read(argv[1],buffer); // read data

  if(h_head.ybins == 1) // find peaks in 1d only
    {
      f_find_peaks(buffer,type,0,h_head.xbins,
		   binsum,noisefact,noisemin,0,100,
		   &npeaks,ipmin,dpmin,ipmax,dpwidth,dpint);
      printf("Binsum: %d Noisefactor: %8.2f Noiseminimum: %8.2f\n",
	     binsum,noisefact,noisemin);
      printf("Peak  at width    int\n");
      for(i=0;i<npeaks;i++)
	{
	  printf("%3d %4d %3.1f   %8.1f\n",
		 i+1,ipmax[i],dpwidth[i],dpint[i]);
	}
    }
}
