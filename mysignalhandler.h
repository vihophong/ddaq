// mysignalhandler.h
#ifndef __MY_SIGNAL_HANDLER_H__
#define __MY_SIGNAL_HANDLER_H__
#include <TSysEvtHandler.h>
class mysignalhandler : public TSignalHandler {
   public:
      mysignalhandler( ESignals sig ) : TSignalHandler( sig ) {}
      Bool_t Notify();
};
#endif
