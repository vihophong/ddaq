#define MAXNUM 0x1000
#define MR2ADDR 0x01
#define STATION 16

extern int _cam_errno;

main(argc, argv)
     int argc;
     char *argv[];
{   
  int i,status=0, data[MAXNUM], station=STATION,start=0,channels=MAXNUM, *pd;
  char error[256];

  if(argc == 1)
    {
      printf("-i|c|r|w|g|h node [station [start [channels]]]");
      exit(0);
    }
  if(argc > 3) station  = atol(argv[3]);
  if(argc > 4) start    = atol(argv[4]);
  if(argc > 5) channels = atol(argv[5]);

  //-----------------------------------------
  if(strstr(argv[1],"-r") != 0)
    {
      printf("read\n");
      printf("Node %s, station %d: start read %d channels %d\n",argv[2],station,start,channels);
      status=f_mr2_read(argv[2],station,start,channels,data,error);
      if(status == 0)
	{
      printf ("output of data > 0\n");
      pd = data;
      for(i = 0; i < channels; i++)
	{
	  if(*pd != 0) printf("%6d : %8d\n", i, *pd);
	  pd++;
	}
	}
    }
  //-----------------------------------------
  else if(strstr(argv[1],"-i") != 0)
    {
      printf("init\n");
      status=f_mr2_init(argv[2],station,error);
    }
  //-----------------------------------------
  else if(strstr(argv[1],"-g") != 0)
    {
      printf("start\n");
      status=f_mr2_start(argv[2],station,error);
    }
  //-----------------------------------------
  else if(strstr(argv[1],"-h") != 0)
    {
      printf("halt\n");
      status=f_mr2_stop(argv[2],station,error);
    }
  //-----------------------------------------
  else if(strstr(argv[1],"-w") != 0)
    {
      printf("write\n");
      status=f_mr2_write(argv[2],station,start,channels,data,error);
    }
  //-----------------------------------------
  else if(strstr(argv[1],"-c") != 0)
    {
      printf("clear\n");
      status=f_mr2_clear(argv[2],station,start,channels,error);
    }
  else 
      printf("-i|c|r|w|g|h node [station [start [channels]]]");

  if(status) printf("ERROR: %s\n",error);
      exit(0);
}
