#ifndef __UTILS_BUILD_DATE_H__
#define __UTILS_BUILD_DATE_H__

#define _BUILD_YEAR_CH0 (__DATE__[ 7])
#define _BUILD_YEAR_CH1 (__DATE__[ 8])
#define _BUILD_YEAR_CH2 (__DATE__[ 9])
#define _BUILD_YEAR_CH3 (__DATE__[10])

#define _BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define _BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define _BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define _BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define _BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define _BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define _BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define _BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define _BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define _BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define _BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define _BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define _BUILD_MONTH_CH0 \
((_BUILD_MONTH_IS_OCT || _BUILD_MONTH_IS_NOV || _BUILD_MONTH_IS_DEC) ? '1' : '0')
#define _BUILD_MONTH_CH1 \
( \
(_BUILD_MONTH_IS_JAN) ? '1' : \
(_BUILD_MONTH_IS_FEB) ? '2' : \
(_BUILD_MONTH_IS_MAR) ? '3' : \
(_BUILD_MONTH_IS_APR) ? '4' : \
(_BUILD_MONTH_IS_MAY) ? '5' : \
(_BUILD_MONTH_IS_JUN) ? '6' : \
(_BUILD_MONTH_IS_JUL) ? '7' : \
(_BUILD_MONTH_IS_AUG) ? '8' : \
(_BUILD_MONTH_IS_SEP) ? '9' : \
(_BUILD_MONTH_IS_OCT) ? '0' : \
(_BUILD_MONTH_IS_NOV) ? '1' : \
(_BUILD_MONTH_IS_DEC) ? '2' : \
/* error default */ '?' \
)

#define _BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define _BUILD_DAY_CH1 (__DATE__[ 5])

#define _BUILD_DATE _BUILD_YEAR_CH0, _BUILD_YEAR_CH1, _BUILD_YEAR_CH2, _BUILD_YEAR_CH3,\
    _BUILD_MONTH_CH0, _BUILD_MONTH_CH1,\
    _BUILD_DAY_CH0, _BUILD_DAY_CH1

constexpr char kBuildDate[] = { _BUILD_DATE, '\0' };

#endif  // __UTILS_BUILD_DATE_H__
