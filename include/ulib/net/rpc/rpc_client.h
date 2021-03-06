// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    rpc_client.h
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#ifndef ULIB_RPC_CLIENT_H
#define ULIB_RPC_CLIENT_H 1

#include <ulib/net/client/client.h>
#include <ulib/net/rpc/rpc_encoder.h>

class URDBClient_Base;

class U_EXPORT URPCClient_Base : public UClient_Base {
public:

   // SERVICES

   bool processRequest(URPCMethod& method)
      {
      U_TRACE(0, "URPCClient_Base::processRequest(%p)", &method)

      U_INTERNAL_ASSERT_POINTER(URPCMethod::encoder)

      UString request = URPCMethod::encoder->encodeMethodCall(method, UString::getStringNull());

      if (sendRequest(request, false)            &&
          readResponse(socket, buffer, response) &&
          buffer.equal(U_CONSTANT_TO_PARAM("DONE")))
         {
         U_RETURN(true);
         }

      U_RETURN(false);
      }

#if defined(U_STDCPP_ENABLE) && defined(DEBUG)
   const char* dump(bool _reset) const { return UClient_Base::dump(_reset); }
#endif

protected:

   // COSTRUTTORI

   URPCClient_Base(UFileConfig* _cfg) : UClient_Base(_cfg)
      {
      U_TRACE_REGISTER_OBJECT(0, URPCClient_Base, "%p", _cfg)

      URPCMethod::encoder = U_NEW(URPCEncoder);
      }

   ~URPCClient_Base()
      {
      U_TRACE_UNREGISTER_OBJECT(0, URPCClient_Base)

      U_INTERNAL_ASSERT_POINTER(URPCMethod::encoder)

      delete URPCMethod::encoder;
      }

   static bool readResponse(USocket* sk, UString& buffer, UString& response)
      {
      U_TRACE(0, "URPCClient_Base::readResponse(%p,%.*S,%.*S)", sk, U_STRING_TO_TRACE(buffer), U_STRING_TO_TRACE(response))

      uint32_t rstart = 0;

      // NB: we force for U_SUBSTR_INC_REF case (string can be referenced more)...

        buffer.setEmptyForce();
      response.setEmptyForce();

      if (URPC::readTokenString(sk, 0, buffer, rstart, response))
         {
         // NB: we force for U_SUBSTR_INC_REF case (string can be referenced more)...

         buffer.size_adjust_force(U_TOKEN_NM);
         }

      U_INTERNAL_DUMP("buffer = %.*S response = %.*S)", U_STRING_TO_TRACE(buffer), U_STRING_TO_TRACE(response))

      if (buffer) U_RETURN(true);

      U_RETURN(false);
      }

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   URPCClient_Base(const URPCClient_Base&) = delete;
   URPCClient_Base& operator=(const URPCClient_Base&) = delete;
#else
   URPCClient_Base(const URPCClient_Base&) : UClient_Base(0) {}
   URPCClient_Base& operator=(const URPCClient_Base&)        { return *this; }
#endif

   friend class URDBClient_Base;
};

template <class Socket> class U_EXPORT URPCClient : public URPCClient_Base {
public:

   // COSTRUTTORI

   URPCClient(UFileConfig* _cfg) : URPCClient_Base(_cfg)
      {
      U_TRACE_REGISTER_OBJECT(0, URPCClient, "%p", _cfg)

      UClient_Base::socket = U_NEW(Socket(UClient_Base::bIPv6));
      }

   ~URPCClient()
      {
      U_TRACE_UNREGISTER_OBJECT(0, URPCClient)
      }

#if defined(U_STDCPP_ENABLE) && defined(DEBUG)
   const char* dump(bool _reset) const { return URPCClient_Base::dump(_reset); }
#endif

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   URPCClient(const URPCClient&) = delete;
   URPCClient& operator=(const URPCClient&) = delete;
#else
   URPCClient(const URPCClient&) : URPCClient_Base(0) {}
   URPCClient& operator=(const URPCClient&)           { return *this; }
#endif
};

#ifdef USE_LIBSSL // specializzazione con USSLSocket

template <> class U_EXPORT URPCClient<USSLSocket> : public URPCClient_Base {
public:

   URPCClient(UFileConfig* _cfg) : URPCClient_Base(_cfg)
      {
      U_TRACE_REGISTER_OBJECT(0, URPCClient<USSLSocket>, "%p", _cfg)

      UClient_Base::setSSLContext();
      }

   ~URPCClient()
      {
      U_TRACE_UNREGISTER_OBJECT(0, URPCClient<USSLSocket>)
      }

   // DEBUG

#if defined(U_STDCPP_ENABLE) && defined(DEBUG)
   const char* dump(bool _reset) const { return URPCClient_Base::dump(_reset); }
#endif

protected:

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   URPCClient<USSLSocket>(const URPCClient<USSLSocket>&) = delete;
   URPCClient<USSLSocket>& operator=(const URPCClient<USSLSocket>&) = delete;
#else
   URPCClient<USSLSocket>(const URPCClient<USSLSocket>&) : URPCClient_Base(0) {}
   URPCClient<USSLSocket>& operator=(const URPCClient<USSLSocket>&)           { return *this; }
#endif
};

#endif
#endif
