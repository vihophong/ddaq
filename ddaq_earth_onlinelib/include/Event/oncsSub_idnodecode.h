#ifndef __ONCSSUB_IDNODECODE_H__
#define __ONCSSUB_IDNODECODE_H__


#include "oncsSubevent.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idnodecode : public  oncsSubevent_w4 {
#else
class  oncsSub_idnodecode : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idnodecode( subevtdata_ptr);

  void  dump ( OSTREAM& os = COUT) ;

  int *getIntArray(int *nwout, const char *what="");
  int getHitFormat() const;
protected:
};



#endif /* __ONCSSUB_IDNODECODE_H__ */
