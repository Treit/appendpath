/* +++Date last modified: 05-Jul-1997 */

/*
** Designation:  StriStr
**
** Call syntax:  TCHAR *stristr(TCHAR *String, TCHAR *Pattern)
**
** Description:  This function is an ANSI version of strstr() with
**               case insensitivity.
**
** Return item:  TCHAR *pointer if Pattern is found in String, else
**               pointer to 0
**
** Rev History:  07/04/95  Bob Stout  ANSI-fy
**               02/03/94  Fred Cole  Original
**
** Hereby donated to public domain.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <tchar.h>
#include <windows.h>

typedef unsigned int uint;

#if defined(__cplusplus) && __cplusplus
 extern "C" {
#endif


TCHAR *stristrW(const TCHAR *String, const TCHAR *Pattern)
{
      TCHAR *pptr, *sptr, *start;
      uint  slen, plen;

		slen  = _tcslen(String);
		
      for (start = (TCHAR *)String,
           pptr  = (TCHAR *)Pattern,
           slen  = _tcsclen(String),
           plen  = _tcsclen(Pattern),


			
           /* while string length not shorter than pattern length */

           slen >= plen;

           start++, slen--;)
      {
            /* find start of pattern in string */
            while (toupper(*start) != toupper(*Pattern))
            {
                  start++;
                  slen--;

                  /* if pattern longer than string */

                  if (slen < plen)
                        return(NULL);
            }

            sptr = start;
            pptr = (TCHAR *)Pattern;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if ('\0' == *pptr)
                        return (start);
            }
      }
      return(NULL);
}
char *stristr(const char *String, const char *Pattern)
{
      char *pptr, *sptr, *start;
      uint  slen, plen;

      for (start = (char *)String,
           pptr  = (char *)Pattern,
           slen  = strlen(String),
           plen  = strlen(Pattern);

           /* while string length not shorter than pattern length */

           slen >= plen;

           start++, slen--)
      {
            /* find start of pattern in string */
            while (toupper(*start) != toupper(*Pattern))
            {
                  start++;
                  slen--;

                  /* if pattern longer than string */

                  if (slen < plen)
                        return(NULL);
            }

            sptr = start;
            pptr = (char *)Pattern;

            while (toupper(*sptr) == toupper(*pptr))
            {
                  sptr++;
                  pptr++;

                  /* if end of pattern then pattern was found */

                  if ('\0' == *pptr)
                        return (start);
            }
      }
      return(NULL);
}


#if defined(__cplusplus) && __cplusplus
 }
#endif

#ifdef TEST

int main(void)
{
      TCHAR buffer[80] = "heLLo, HELLO, hello, hELLo, HellO";
      TCHAR *sptr = buffer;

      while (0 != (sptr = stristr(sptr, "hello")))
            printf("Found %5.5s!\n", sptr++);

      return(0);
}

#endif /* TEST */
