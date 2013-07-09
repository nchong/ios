#if defined(CONCRETE)

DECLARE_UF_BINARY(CIRC, int, int, int);
#define TYPE int
#define OPERATOR(X, Y) CIRC(X, Y)

#elif defined(ABSTRACT)

#if 0
#define MAKE_PAIR(lower, upper) (((lower) << 16) | (upper))
#define GET_UPPER(X) ((X) & (0x0000ffff))
#define GET_LOWER(X) (((X) >> 16) & (0x0000ffff))

// Identity is identified by bad interval (1, 0)
#define IDENTITY MAKE_PAIR(1, 0)

// Top is identified by bad interval (2, 0)
#define TOP MAKE_PAIR(2, 0)

#define TYPE unsigned
#define OPERATOR(X, Y) ((X == IDENTITY) ? Y : ((Y ==  IDENTITY) ? X : ((X == TOP) | (Y == TOP) | (GET_UPPER(X) != GET_LOWER(Y)) ? TOP : MAKE_PAIR(GET_LOWER(X), GET_UPPER(Y)))))
#else
#define MAKE_PAIR(lower, upper) ((0xffff0000 & ((lower) << 16)) | (upper & 0xffff))
#define GET_UPPER(X) ((X) & (0x0000ffff))
#define GET_LOWER(X) (((X) >> 16) & (0x0000ffff))

// Identity is identified by bad interval (1, 0)
#define IDENTITY MAKE_PAIR(1, 0)

// Top is identified by bad interval (2, 0)
#define TOP MAKE_PAIR(2, 0)

#define TYPE unsigned long
#define OPERATOR(X, Y) ((X == IDENTITY) ? Y : ((Y ==  IDENTITY) ? X : ((X == TOP) | (Y == TOP) | (GET_UPPER(X) != GET_LOWER(Y)) ? TOP : MAKE_PAIR(GET_LOWER(X), GET_UPPER(Y)))))
#endif

#else

#error Must define CONCRETE or ABSTRACT

#endif
