#ifndef SPECINFO_H

/* No struct Spec, use rpmSpec */
#ifdef RPMSPEC
 #ifndef _RPM_4_4_COMPAT
 #define _RPM_4_4_COMPAT
 #endif
#include <rpm/rpmlegacy.h>
#endif

#endif // SPECINFO_H
