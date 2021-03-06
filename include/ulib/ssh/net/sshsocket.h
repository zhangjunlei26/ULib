// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    sshsocket.h
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#ifndef ULIB_SSHSOCKET_H
#define ULIB_SSHSOCKET_H 1

#include <ulib/net/socket.h>

#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_BUGREPORT

#include <libssh/libssh.h>

#undef  PACKAGE_NAME
#define PACKAGE_NAME    "ULib"
#undef  PACKAGE_VERSION
#define PACKAGE_VERSION ULIB_VERSION

/**
 * --------------------------------------------------------------------------------------------------------------------------
 * Come funziona ssh:
 * --------------------------------------------------------------------------------------------------------------------------
 * Ogni host su cui � installato ssh possiede una coppia di chiavi RSA (un algoritmo di crittografia a chiave asimmetrica)
 * lunghe 1024 bit, una pubblica ed una privata. In pi�, ogni utente che utilizza ssh pu� opzionalmente generare una propria
 * coppia di chiavi RSA. All'atto della connessione, il server comunica al client due chiavi pubbliche: una fissa di 1024 bit
 * che � la vera e propria chiave dell'host e l'altra di 768 bit che viene rigenerata ogni ora. Il client allora genera una
 * sequenza casuale di 256 bit e la codifica con le chiavi pubbliche del server. Da questo momento in poi la connessione viene
 * crittografata con uno degli algoritmi a chiave simmetrica supportati da ssh (IDEA, DES, 3DES, Arcfour, Blowfish) e si passa
 * alla fase di autenticazione.
 * --------------------------------------------------------------------------------------------------------------------------
 * The SSH protocol was designed for some goals which I resume here:
 * 
 * a) Privacy of data
 * b) Security
 * c) Authentication of the server
 * d) Authentication of the client
 * 
 * The client MUST be sure who's speaking to before entering into any authentication way. That's where the end programmer must
 * ensure the given fingerprints *are* from the legitimate server. A ssh connection must follow the following steps:
 * 
 * 1) Before connecting the socket, you can set up if you wish one or other server public key authentication ie. DSA or RSA.
 *    You can choose cryptographic algorithms you trust and compression algorithms if any
 * 2) The connection is made. A secure handshake is made, and resulting from it, a public key from the server is gained.
 *    You MUST verify that the public key is legitimate
 * 3) The client must authenticate: the two implemented ways are password, and public keys (from dsa and rsa key-pairs
 *    generated by openssh). It is harmless to authenticate to a fake server with these keys because the protocol ensures
 *    the data you sign can't be used twice. It just avoids man-in-the-middle attacks
 * 4) Now that the user has been authenticated, you must open one or several channels. channels are different subways for
 *    information into a single ssh connection. Each channel has a standard stream (stdout) and an error stream (stderr).
 *    You can theoretically open an infinity of channel
 * 5) With the channel you opened, you can do several things:
 *    - Open a shell. You may want to request a pseudo virtual terminal before
 *    - Execute a command. The virtual terminal is usable, too
 *    - Invoke the sftp subsystem. (look at chapter 6)
 *    - invoke your own subsystem. This is out the scope of this document but it is easy to do
 * 6) When everything is finished, just close the channels, and then the connection
 * ---------------------------------------------------------------------------------------------------------------------------
 */

class U_EXPORT USSHSocket : public USocket {
public:

   // COSTRUTTORI

   USSHSocket(bool bSocketIsIPv6 = false) : USocket(bSocketIsIPv6)
      {
      U_TRACE_REGISTER_OBJECT(0, USSHSocket, "%b", bSocketIsIPv6)

      buffer  = 0;
      channel = 0;
      ret     = auth = 0;
      user    = public_key = private_key = 0;

      session = (ssh_session) U_SYSCALL_NO_PARAM(ssh_new);
      }

   ~USSHSocket()
      {
      U_TRACE_UNREGISTER_OBJECT(0, USSHSocket)

      close();

      if (buffer) U_SYSCALL_VOID(buffer_free, "%p", buffer);
      }

   // VARIE

   void close();
   bool setError();
   bool SSHConnection(int fd);

   void setUser(const char* _user)
      {
      U_TRACE(1, "USSHSocket::setUser(%S)", _user)

      U_INTERNAL_ASSERT_POINTER(_user)

      user = _user;

      (void) U_SYSCALL(ssh_options_set, "%p,%d,%S", session, SSH_OPTIONS_USER, user);
      }

   void setKey(const char* _private_key = "/%U/.ssh/id_rsa",
               const char*  _public_key = "/%U/.ssh/id_rsa.pub")
      {
      U_TRACE(1, "USSHSocket::setKey(%S,%S)", _private_key, _public_key)

      public_key  = _public_key;
      private_key = _private_key;
      }

   void setVerbosity(int num = -1)
      {
      U_TRACE(1, "USSHSocket::setVerbosity(%d)", num)

      (void) U_SYSCALL(ssh_options_set, "%p,%d,%p", session, SSH_OPTIONS_LOG_VERBOSITY, &num);
      }

   void sendEOF()
      {
      U_TRACE(1, "USSHSocket::sendEOF()")

      U_INTERNAL_ASSERT_POINTER(channel)

      U_SYSCALL_VOID(channel_send_eof, "%p", channel);
      }

   // VIRTUAL METHOD

   /**
    * This method is called to connect the socket to a server SSH deamon that is specified
    * by the provided host name and port number. We call the ssh_connect() function to perform the connection
    */

   virtual bool connectServer(const UString& server, unsigned int iServPort, int timeoutMS = 0) U_DECL_OVERRIDE; // 22

   virtual int recv(      void* pBuffer,  uint32_t iBufLength)     U_DECL_OVERRIDE;
   virtual int send(const char* pPayload, uint32_t iPayloadLength) U_DECL_OVERRIDE;

#if defined(U_STDCPP_ENABLE) && defined(DEBUG)
   const char* dump(bool reset) const;
#endif

protected:
   const char* user;
   ssh_buffer buffer;
   ssh_channel channel;
   ssh_session session;
   const char* public_key;
   const char* private_key;
   int ret, auth;

   void setStatus() U_NO_EXPORT;
   void setStatus(int n) { ret = n; setStatus(); }

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   USSHSocket(const USSHSocket&) = delete;
   USSHSocket& operator=(const USSHSocket&) = delete;
#else
   USSHSocket(const USSHSocket&) : USocket(false) {}
   USSHSocket& operator=(const USSHSocket&)       { return *this; }
#endif      
};

#endif
