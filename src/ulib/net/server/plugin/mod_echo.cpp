// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    mod_echo.cpp - this is plugin ECHO for userver
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#include <ulib/net/server/client_image.h>
#include <ulib/net/server/server_plugin.h>
#include <ulib/net/server/plugin/mod_echo.h>

U_CREAT_FUNC(server_plugin_echo, UEchoPlugIn)

/*
#define U_RESPONSE_FOR_TEST \
"HTTP/1.0 200 OK\r\n" \
"Server: ULib\r\n" \
"Connection: close\r\n" \
"Content-Type: text/html\r\n" \
"Content-Length: 22\r\n\r\n" \
"<h1>Hello stefano</h1>"
*/

// Server-wide hooks

// Connection-wide hooks

int UEchoPlugIn::handlerRequest()
{
   U_TRACE(0, "UEchoPlugIn::handlerRequest()")

   UClientImage_Base::body->clear();

   UClientImage_Base::setNoHeaderForResponse();

#ifdef U_RESPONSE_FOR_TEST
   UClientImage_Base::wbuffer->assign(U_CONSTANT_TO_PARAM(U_RESPONSE_FOR_TEST));
#else
   *UClientImage_Base::wbuffer = *UClientImage_Base::request;
#endif

   U_RETURN(U_PLUGIN_HANDLER_PROCESSED | U_PLUGIN_HANDLER_FINISHED);
}
