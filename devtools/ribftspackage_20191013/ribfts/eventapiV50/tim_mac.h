#include "typedefs.h"
/* tim_mac.h
 *
 * real time and CPU time measurement
 * R.S. Mayer
 * 12-Aug-1994
 *
 */
#include <time.h>
#include <timeb.h>
#define TIM__DEF static struct timeb s_timeb_star,\
                                     s_timeb_stop,\
                                     s_timeb_x[16];\
                 static clock_t      clock_t_star,\
                                     clock_t_stop,\
                                     clock_t_x[16]
#define TIM__STA clock_t_star = clock();\
                 ftime(&s_timeb_star)
#define TIM__STP clock_t_stop = clock();\
                 ftime(&s_timeb_stop)
/* +++ msec real (elapsed) time +++ */
#define TIM__TIM (1000 * (s_timeb_stop.time - s_timeb_star.time) +\
                  s_timeb_stop.millitm - s_timeb_star.millitm    )
/* +++ msec CPU time +++ */
#define TIM__TIC ((clock_t_stop - clock_t_star)/1000)

/* +++++++++++++++ */
/* +++ indexed +++ */
/* +++++++++++++++ */
#define TIM__T(i)    clock_t_x[i] = clock();\
                     ftime(&s_timeb_x[i])
/* +++ msec real (elapsed) time between j and i +++ */
#define TIM__TM(i,j) (1000 * (s_timeb_x[j].time - s_timeb_x[i].time) +\
                     s_timeb_x[j].millitm - s_timeb_x[i].millitm    )
/* +++ msec CPU time between j and i +++ */
#define TIM__TC(i,j) ((clock_t_x[j] - clock_t_x[i])/1000)
/* +++ equivalence i = j +++ */
#define TIM__EQ(i,j) s_timeb_x[i] = s_timeb_x[j];\
                     clock_t_x[i] = clock_t_x[j]
