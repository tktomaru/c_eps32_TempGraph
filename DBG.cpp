#include "DBG.h"

#if _DEBUG
void DBG2( const char *format, ... )
{
  va_list ap;
  char chbuff[512];

  va_start( ap, format );
  vsprintf( chbuff, format, ap );
  va_end( ap );
  DBG(chbuff);
}
#else
void DBG2( const char *format, ... )
{
}
#endif /* _DBG */
