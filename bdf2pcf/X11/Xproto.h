#ifndef _UNIX_PROT_H
#define _UNIX_PROT_H

#include "Xmd.h"

typedef struct {
	INT16 leftSideBearing B16,
		rightSideBearing B16,
		characterWidth B16,
		ascent B16,
		descent B16;
	CARD16 attributes B16;
} xCharInfo;

#endif // !_UNIX_PROT_H
