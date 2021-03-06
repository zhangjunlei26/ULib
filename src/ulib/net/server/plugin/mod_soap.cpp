// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    mod_soap.cpp - this is a plugin soap for userver
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#include <ulib/file_config.h>
#include <ulib/utility/uhttp.h>
#include <ulib/net/server/server.h>
#include <ulib/xml/soap/soap_object.h>
#include <ulib/xml/soap/soap_parser.h>
#include <ulib/net/server/plugin/mod_soap.h>

U_CREAT_FUNC(server_plugin_soap, USoapPlugIn)

USOAPParser* USoapPlugIn::soap_parser;

USoapPlugIn::USoapPlugIn()
{
   U_TRACE_REGISTER_OBJECT_WITHOUT_CHECK_MEMORY(0, USoapPlugIn, "")
}

USoapPlugIn::~USoapPlugIn()
{
   U_TRACE_UNREGISTER_OBJECT(0, USoapPlugIn)

   if (soap_parser)
      {
      delete soap_parser;
      delete URPCMethod::encoder;
      delete URPCObject::dispatcher;
      }
}

// Server-wide hooks

int USoapPlugIn::handlerConfig(UFileConfig& cfg)
{
   U_TRACE(0, "USoapPlugIn::handlerConfig(%p)", &cfg)

   // Perform registration of server SOAP method

   soap_parser = U_NEW(USOAPParser);

   USOAPObject::loadGenericMethod(&cfg);

   U_RETURN(U_PLUGIN_HANDLER_PROCESSED | U_PLUGIN_HANDLER_GO_ON);
}

int USoapPlugIn::handlerInit()
{
   U_TRACE(0, "USoapPlugIn::handlerInit()")

   // NB: soap is NOT a static page, so to avoid stat() syscall we use alias mechanism...

   if (soap_parser)
      {
#  ifndef U_ALIAS
      U_SRV_LOG("WARNING: Sorry, I can't run soap plugin because alias URI support is missing, please recompile ULib...");
#  else
      if (UHTTP::valias == 0) UHTTP::valias = U_NEW(UVector<UString>(2U));

      UHTTP::valias->push_back(U_STRING_FROM_CONSTANT("/soap"));
      UHTTP::valias->push_back(U_STRING_FROM_CONSTANT("/nostat"));

      U_RETURN(U_PLUGIN_HANDLER_PROCESSED | U_PLUGIN_HANDLER_GO_ON);
#  endif
      }

   U_RETURN(U_PLUGIN_HANDLER_ERROR);
}

// Connection-wide hooks

int USoapPlugIn::handlerRequest()
{
   U_TRACE(0, "USoapPlugIn::handlerRequest()")

   if (UHTTP::isSOAPRequest())
      {
      if (soap_parser == 0) UHTTP::setInternalError();
      else
         {
         // process the SOAP message -- should be the contents of the message from "<SOAP:" to the end of the string

         bool bSendingFault;

         UString body   = soap_parser->processMessage(*UClientImage_Base::body, *URPCObject::dispatcher, bSendingFault),
                 method = soap_parser->getMethodName();

         U_SRV_LOG_WITH_ADDR("method %.*S process %s for", U_STRING_TO_TRACE(method), (bSendingFault ? "failed" : "passed"));

#     ifdef DEBUG
         (void) UFile::writeToTmp(U_STRING_TO_PARAM(body), false, "soap.res", 0);
#     endif

         u_http_info.nResponseCode  = HTTP_OK;

         UHTTP::setResponse(UHTTP::str_ctype_soap, &body);
         }

      U_RETURN(U_PLUGIN_HANDLER_PROCESSED | U_PLUGIN_HANDLER_GO_ON);
      }

   U_RETURN(U_PLUGIN_HANDLER_GO_ON);
}

// DEBUG

#if defined(U_STDCPP_ENABLE) && defined(DEBUG)
const char* USoapPlugIn::dump(bool reset) const
{
   *UObjectIO::os << "soap_parser (USOAPParser " << (void*)soap_parser << ')';

   if (reset)
      {
      UObjectIO::output();

      return UObjectIO::buffer_output;
      }

   return 0;
}
#endif
