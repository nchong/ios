#if defined(CONCRETE)

DECLARE_UF_BINARY(CIRC, int, int, int);
#define TYPE int
#define OPERATOR(X, Y) CIRC(X, Y)

#elif defined(ABSTRACT)

#define NBITS ((TYPE)32)
#define MASK ((TYPE)(((TYPE)1 << NBITS)-(TYPE)1))

#define MAKE_PAIR(lower, upper) ((((TYPE)lower) << NBITS) | ((TYPE)upper))
#define GET_UPPER(X) (((TYPE)X) & (MASK))
#define GET_LOWER(X) ((((TYPE)X) >> NBITS) & (MASK))

// Identity is identified by bad interval (1, 0)
#define IDENTITY MAKE_PAIR(1, 0)

// Top is identified by bad interval (2, 0)
#define TOP MAKE_PAIR(2, 0)

#define TYPE unsigned long long
#define OPERATOR(X, Y) ((X == IDENTITY) ? Y : ((Y ==  IDENTITY) ? X : ((X == TOP) | (Y == TOP) | (GET_UPPER(X) != GET_LOWER(Y)) ? TOP : MAKE_PAIR(GET_LOWER(X), GET_UPPER(Y)))))

#else

#error Must define CONCRETE or ABSTRACT

#endif
