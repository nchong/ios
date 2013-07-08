#if defined(CONCRETE)

DECLARE_UF_BINARY(CIRC, int, int, int);
#define TYPE int
#define OPERATOR(X, Y) CIRC(X, Y)

#elif defined(ABSTRACT)

#define MAKE_PAIR(lower, upper) (lower,upper)
#define GET_UPPER(X) X.lo
#define GET_LOWER(X) X.hi

// Identity is identified by bad interval (1, 0)
#define IDENTITY MAKE_PAIR(1, 0)

// Top is identified by bad interval (2, 0)
#define TOP MAKE_PAIR(2, 0)

#define TYPE uint2
#define OPERATOR(X, Y) ((X == IDENTITY) ? Y : ((Y ==  IDENTITY) ? X : ((X == TOP) | (Y == TOP) | (GET_UPPER(X) != GET_LOWER(Y)) ? TOP : MAKE_PAIR(GET_LOWER(X), GET_UPPER(Y)))))

#else

#error Must define CONCRETE or ABSTRACT

#endif
