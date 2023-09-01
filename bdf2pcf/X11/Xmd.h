#ifndef _UNIX_MD_H
#define _UNIX_MD_H

# define _SIZEOF(x) sz_##x
# define SIZEOF(x) _SIZEOF(x)

/*
 * Bitfield suffixes for the protocol structure elements, if you
 * need them.  Note that bitfields are not guaranteed to be signed
 * (or even unsigned) according to ANSI C.
 */
# define B32 /* bitfield not needed on architectures with native 32-bit type */
# define B16 /* bitfield not needed on architectures with native 16-bit type */
# ifdef LONG64
typedef long INT64;
typedef int INT32;
# else
typedef long INT32;
# endif
typedef short INT16;

typedef signed char    INT8;

# ifdef LONG64
typedef unsigned long CARD64;
typedef unsigned int CARD32;
# else
typedef unsigned long long CARD64;
typedef unsigned long CARD32;
# endif
typedef unsigned short CARD16;
typedef unsigned char  CARD8;

typedef CARD32		BITS32;
typedef CARD16		BITS16;

typedef CARD8		BYTE;
typedef CARD8		BOOL;

#endif // !_UNIX_MD_H
