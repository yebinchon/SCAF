#include "llvm/IR/Constants.h"
#include "MemoryAnalysisModules/LoopAA.h"  // for FULL_UNIVERSAL
#include "AnalysisUtilities/CallSiteFactory.h"
#include "MemoryAnalysisModules/GetCallers.h"

namespace liberty
{
  bool getCallers(const Function *fcn, CallSiteList &callsitesOut)
  {
    bool addressCaptured = false;
    for(Value::const_user_iterator i=fcn->user_begin(), e=fcn->user_end(); i!=e; ++i)
    {
      const Value *v = *i;

      CallSite cs = getCallSite(v);
      if( cs.getInstruction() )
      {
        callsitesOut.push_back(cs);
        continue;
      }

      if( const ConstantExpr *cexp = dyn_cast< ConstantExpr >(v) )
        if( cexp->isCast() && cexp->hasOneUse() )
        {
          cs = getCallSite( * cexp->user_begin() );
          if( cs.getInstruction() )
          {
            callsitesOut.push_back(cs);
            continue;
          }
        }

      addressCaptured = true;
    }

    if( addressCaptured )
      return false;

    return FULL_UNIVERSAL || fcn->hasLocalLinkage();
  }
}
