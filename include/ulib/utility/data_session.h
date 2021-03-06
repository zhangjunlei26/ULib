// ============================================================================
//
// = LIBRARY
//    ULib - c++ library
//
// = FILENAME
//    data_session.h - data session utility
//
// = AUTHOR
//    Stefano Casazza
//
// ============================================================================

#ifndef ULIB_DATA_SESSION_H
#define ULIB_DATA_SESSION_H 1

#include <ulib/container/vector.h>

template <class T> class URDBObjectHandler;

class U_EXPORT UDataStorage {
public:

   // Check Memory
   U_MEMORY_TEST

   // Allocator e Deallocator
   U_MEMORY_ALLOCATOR
   U_MEMORY_DEALLOCATOR

   // COSTRUTTORE

   UDataStorage()
      {
      U_TRACE_REGISTER_OBJECT(0, UDataStorage, "", 0)
      }

   UDataStorage(const UString& key) : keyid(key)
      {
      U_TRACE_REGISTER_OBJECT(0, UDataStorage, "%.*S", U_STRING_TO_TRACE(key))
      }

   virtual ~UDataStorage()
      {
      U_TRACE_UNREGISTER_OBJECT(0, UDataStorage)
      }

   // method VIRTUAL to define

   virtual void clear() {}

   // SERVICES

   void setKeyId();

   bool isDataSession()
      {
      U_TRACE(0, "UDataStorage::isDataSession()")

      U_INTERNAL_DUMP("keyid = %.*S", U_STRING_TO_TRACE(keyid))

      if (keyid) U_RETURN(true);

      U_RETURN(false);
      }

   void resetDataSession()
      {
      U_TRACE(0, "UDataStorage::resetDataSession()")

      U_INTERNAL_DUMP("keyid = %.*S", U_STRING_TO_TRACE(keyid))

      if (keyid.isNull() == false) keyid.clear();
      }

   void setKeyIdDataSession(const UString& key)
      {
      U_TRACE(0, "UDataStorage::setKeyIdDataSession(%.*S)", U_STRING_TO_TRACE(key))

      U_INTERNAL_DUMP("keyid = %.*S", U_STRING_TO_TRACE(keyid))

      U_INTERNAL_ASSERT(key)

      keyid = key;
      }

   void setKeyIdDataSession(const char* s, uint32_t n)
      {
      U_TRACE(0, "UDataStorage::setKeyIdDataSession(%.*S,%u)", n, s, n)

      U_INTERNAL_DUMP("keyid = %.*S", U_STRING_TO_TRACE(keyid))

      (void) keyid.assign(s, n);
      }

#ifdef U_STDCPP_ENABLE
   virtual void   toStream(ostream& os) {}
   virtual void fromStream(istream& is) {}

   friend istream& operator>>(istream& is, UDataStorage& d) { d.fromStream(is); return is; }
   friend ostream& operator<<(ostream& os, UDataStorage& d) {   d.toStream(os); return os; }

#  ifdef DEBUG
   const char* dump(bool reset) const;
#  endif
#endif

protected:
   UString keyid;

   // method VIRTUAL to define

   virtual char* toBuffer();
   virtual void  fromData(const char* ptr, uint32_t len);

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   UDataStorage(const UDataStorage&) = delete;
   UDataStorage& operator=(const UDataStorage&) = delete;
#else
   UDataStorage(const UDataStorage&)            {}
   UDataStorage& operator=(const UDataStorage&) { return *this; }
#endif      

   template <class T> friend class URDBObjectHandler;
};

class U_EXPORT UDataSession : public UDataStorage {
public:

   // COSTRUTTORE

   UDataSession()
      {
      U_TRACE_REGISTER_OBJECT(0, UDataSession, "", 0)

      vec_var = U_NEW(UVector<UString>);

      creation = last_access = u_now->tv_sec;
      }

   virtual ~UDataSession()
      {
      U_TRACE_UNREGISTER_OBJECT(0, UDataSession)

      delete vec_var;
      }

   // SERVICES

   bool isNewSession()
      {
      U_TRACE(0, "UDataSession::isNewSession()")

      if (last_access == creation) U_RETURN(true);

      U_RETURN(false);
      }

   bool isDataSessionExpired()
      {
      U_TRACE(0, "UDataSession::isDataSessionExpired()")

      U_INTERNAL_DUMP("keyid = %.*S", U_STRING_TO_TRACE(keyid))

      bool result = ((last_access - creation) > U_ONE_DAY_IN_SECOND);

      U_RETURN(result);
      }

   UString getSessionCreationTime()
      {
      U_TRACE(0, "UDataSession::getSessionCreationTime()")

      UString x(40U);

      x.snprintf("%#5D", creation);

      U_RETURN_STRING(x);
      }

   UString getSessionLastAccessedTime()
      {
      U_TRACE(0, "UDataSession::getSessionLastAccessedTime()")

      UString x(40U);

      x.snprintf("%#5D", last_access);

      U_RETURN_STRING(x);
      }

   void getValueVar(uint32_t index, UString& value)
      {
      U_TRACE(0, "UDataSession::getValueVar(%u,%p)", index, &value)

      value = vec_var->at(index);

      U_INTERNAL_DUMP("value = %.*S", U_STRING_TO_TRACE(value))
      }

   void putValueVar(uint32_t index, const UString& value)
      {
      U_TRACE(0, "UDataSession::putValueVar(%u,%.*S)", index, U_STRING_TO_TRACE(value))

      if (index < vec_var->size()) vec_var->replace(index, value);
      else
         {
         U_INTERNAL_ASSERT_EQUALS(index, vec_var->size())

         vec_var->push_back(value);
         }
      }

   UString setKeyIdDataSession(uint32_t counter);

   // define method VIRTUAL of class UDataStorage

   virtual void clear();

   // STREAM

#ifdef U_STDCPP_ENABLE
   virtual void   toStream(ostream& os);
   virtual void fromStream(istream& is);

   friend istream& operator>>(istream& is, UDataSession& d) { d.fromStream(is); return is; }
   friend ostream& operator<<(ostream& os, UDataSession& d) {   d.toStream(os); return os; }

#  ifdef DEBUG
   const char* dump(bool reset) const;
#  endif
#endif

protected:
   UVector<UString>* vec_var;
   long creation, last_access;

private:
#ifdef U_COMPILER_DELETE_MEMBERS
   UDataSession(const UDataSession&) = delete;
   UDataSession& operator=(const UDataSession&) = delete;
#else
   UDataSession(const UDataSession& d) : UDataStorage() {}
   UDataSession& operator=(const UDataSession&)         { return *this; }
#endif      

                      friend class UHTTP;
   template <class T> friend class URDBObjectHandler;
};

#endif
