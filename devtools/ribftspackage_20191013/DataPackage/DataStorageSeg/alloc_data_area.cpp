//    alloc_data_area.C


#include "common.h"
#include "message.h"

#include <string.h>
#include <stdio.h>

#ifdef UNIX

#include <stdlib.h> /* getenv, exit */
#include <errno.h>

#if (defined SOLARIS || defined POSIX)

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif
    int shm_open(const char *, int, mode_t);
#ifdef __cplusplus
}
#endif

#endif  // of (defined SOLARIS || defined POSIX)

//    the GNU option is for Linux systems which do not support the real time POSIX 4 options
//    the POSIX option is for Linux systems which DO support the real time POSIX 4 options

#ifdef GNU

#include <sys/shm.h>
#include <sys/ipc.h>

#endif

#include <sys/stat.h>

#endif   // of ifdef UNIX

void * alloc_data_area(int id, int size)
{

#ifndef GNU
char object_name[16];
#endif

#ifdef GNU
   int i;
   size_t shmsize;               // segment size in bytes
   int shmid;
#endif

#if (defined SOLARIS || defined POSIX)
   int i;
   size_t shmsize;
   int shmid;
#endif

#ifdef WIN32
   DWORD shmsize;
   HANDLE shmid;
#endif

  void * shmaddress;
  int shmkey;


//    obtain shared memory ID - this will create segment if needed
//    In practice only the MASTER task which starts first will ever create and initialise

    shmkey = id;
    shmsize = size;
              
#if (defined SOLARIS || defined POSIX)

//    create a file mapped object (MASTER) or obtain ID of existing object

    sprintf(object_name,"/SHM_%d", shmkey);

#ifdef MASTER
    shmid = shm_open(object_name, O_RDWR|O_CREAT, (mode_t) -1);
#else
    shmid = shm_open(object_name, O_RDWR, (mode_t) -1);
#endif
    if (shmid == -1)  {
             ReportError();
             message(MSG_FAILURE, "shm_open failed - This must be fixed - exiting.");
             exit(1);
    }

#endif  // of (defined SOLARIS || defined POSIX)

#ifdef GNU
 
//    Obtain ID of the Shared Memory Segment

#ifdef MASTER
    shmid = shmget(shmkey, shmsize, 0666|IPC_CREAT);
#else
    shmid = shmget(shmkey, 0, S_IWUSR|S_IRUSR);
#endif

    if (shmid == -1)  {
         ReportError();
         sprintf(message_buffer, "shmget called with key = %d, size = %d", shmkey, shmsize);
         report_message(MSG_FAILURE);

#ifdef MASTER
//    Try to delete and recreate the segment (this handles case when the size required has increased)

//    obtain ID of any existing Shared Memory Segment - use size 0 

         shmid = shmget(shmkey, 0, S_IWUSR|S_IRUSR);

//    if a good return delete the existing Shared Memory Segment

         if (shmid != -1) {
             printf("Deleting existing Shared memory Segment %d\n",shmid);
             i = shmctl (shmid, IPC_RMID, NULL);
             if (i == -1) {
                 ReportError();
                 sprintf(message_buffer, "shmctl called with id = %d", shmid);
                 report_message(MSG_FAILURE);
             }
         } else {
             ReportError();
             sprintf(message_buffer, "shmget called with key = %d", shmkey);
             report_message(MSG_FAILURE);
         }

//    try create now

         shmid = shmget(shmkey, shmsize, 0666|IPC_CREAT);
         if (shmid == -1)  {
#endif

//    give up!

             ReportError();
             sprintf(message_buffer, "shmget failed %d - This must be fixed - exiting.", errno);
             report_message(MSG_FAILURE);
             sprintf(message_buffer, "shmget called with key = %d, size = %d", shmkey, shmsize);
             report_message(MSG_FAILURE);
#ifdef MASTER
             sprintf(message_buffer, "Check that your system supports this amount of shared memory");
             report_message(MSG_FAILURE);
#endif
             exit(1);
#ifdef MASTER
         }
#endif
    }

#endif  // of ifdef UNIX

#ifdef WIN32

//    create a file mapped object

    sprintf(object_name,"SHM_%d", shmkey);

    shmid = CreateFileMapping( INVALID_HANDLE_VALUE, NULL,
                               PAGE_READWRITE,
                               0,
                               (DWORD) shmsize+1,
                               object_name);

    if (!shmid)  {

//    give up!

             ReportError();
             sprintf(message_buffer, "CreateFileMapping failed %d - This must be fixed - exiting.", errno);
             report_message(MSG_FAILURE);
             ExitProcess(1);
    }
#endif

#ifndef GNU
    if (shmsize != 0)
       sprintf(message_buffer, "File mapped object %s of size %d created", object_name, shmsize);
    else
       sprintf(message_buffer, "File mapped object %s of size %d accessed", object_name, shmsize);
#else
    #ifdef MASTER
        sprintf(message_buffer, "Shared memory object ID=%d of size %d created", shmkey, shmsize);
    #else
        sprintf(message_buffer, "Shared memory object ID=%d accessed", shmkey);
    #endif
#endif
    report_message(MSG_INFORMATION);

    sprintf(message_buffer, "Shared memory ID is %x" , shmid);
    report_message(MSG_INFORMATION);
    
#if (defined SOLARIS || defined POSIX)

#ifdef MASTER

//    size the memory object

    i = ftruncate(shmid, shmsize);
    if (i == -1)  {
             ReportError();
             message(MSG_FAILURE, "ftruncate failed - This must be fixed - exiting.");
             exit(1);
    }
#endif

//    attach the memory segment

    shmaddress = mmap((caddr_t) NULL, (size_t) shmsize, PROT_READ|PROT_WRITE, MAP_SHARED, shmid, (off_t) 0);
    if (shmaddress == (void *) MAP_FAILED) {
        ReportError();
        message(MSG_FAILURE, "mmap failed - This must be fixed - exiting.");
        exit(1);
    }

//    tidy up 

    close(shmid);

    sprintf(message_buffer, "File mapped object %s located at address %lx.", object_name, shmaddress);
    report_message(MSG_INFORMATION);

    sprintf(message_buffer, "Shared memory segment ID %x released" , shmid);
    report_message(MSG_INFORMATION);

#endif    // of (defined SOLARIS || defined POSIX)
    
#ifdef GNU

//    attach the memory segment

    shmaddress = shmat(shmid, (void *) 0, 0);
    if (shmaddress == (void *) -1) {
        sprintf(message_buffer, "shmat failed %s - This must be fixed - exiting.", strerror(errno));
        report_message(MSG_FAILURE);
        exit(1);
    }

    sprintf(message_buffer, "Shared memory segment located at address %lx.", shmaddress);
    report_message(MSG_INFORMATION);

#endif

#ifdef WIN32

//    map a view of the file mapped object

    shmaddress = MapViewOfFile(shmid,
                                 FILE_MAP_ALL_ACCESS,
                                 0, 0, 0);

    if (shmaddress == NULL) {
    
        ReportError();
        sprintf(message_buffer, "MapViewOfFile failed %d - This must be fixed - exiting.", errno);
        report_message(MSG_FAILURE);
        ExitProcess(1);
    }

    sprintf(message_buffer, "File mapped object located at address %lx.", shmaddress);
    report_message(MSG_INFORMATION);

#endif
    
    return shmaddress;

}
