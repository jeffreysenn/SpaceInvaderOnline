// networking.cc

#define _CRT_SECURE_NO_WARNINGS 1
#include "gamma.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

namespace gamma {
   namespace network {
      sockaddr_in to_sockaddr(const ip_address &addr) {
         sockaddr_in result = {};
         result.sin_family = AF_INET;
         result.sin_addr.s_addr = htonl(addr.host_);
         result.sin_port = htons(addr.port_);
         return result;
      }

      ip_address from_sockaddr(const sockaddr_in &addr) {
         return ip_address(ntohl(addr.sin_addr.s_addr), htons(addr.sin_port));
      }

      bool init() {
         WSADATA data = {};
         int result = WSAStartup(MAKEWORD(2, 2), &data);
         return result == 0;
      }

      void shut() {
         WSACleanup();
      }

      namespace error {
         network_error_code get_error() {
            int error_code = WSAGetLastError();
            switch (error_code) {
               case 0:
                  return NETERR_NO_ERROR;
               case WSA_INVALID_HANDLE:
                  return NETERR_INVALID_HANDLE;
               case WSA_NOT_ENOUGH_MEMORY:
                  return NETERR_NOT_ENOUGH_MEMORY;
               case WSA_INVALID_PARAMETER:
                  return NETERR_INVALID_PARAMETER;
               case WSA_OPERATION_ABORTED:
                  return NETERR_OPERATION_ABORTED;
               case WSAEINTR:
                  return NETERR_INTERUPTED_CALL;
               case WSAEBADF:
                  return NETERR_BAD_FILE_HANDLE;
               case WSAEACCES:
                  return NETERR_SOCKET_ACCESS_DENIED;
               case WSAEFAULT:
                  return NETERR_BAD_ADDRESS;
               case WSAEINVAL:
                  return NETERR_INVALID_ARGUMENT;
               case WSAEMFILE:
                  return NETERR_TOO_MANY_OPEN_FILES;
               case WSAEWOULDBLOCK:
                  return NETERR_WOULD_BLOCK;
               case WSAEINPROGRESS:
                  return NETERR_IN_PROGRESS;
               case WSAEALREADY:
                  return NETERR_ALREADY_IN_PROGRESS;
               case WSAENOTSOCK:
                  return NETERR_HANDLE_NON_SOCKET;
               case WSAEDESTADDRREQ:
                  return NETERR_DESTINATION_ADDRESS_REQUIRED;
               case WSAEMSGSIZE:
                  return NETERR_MESSAGE_TOO_LONG;
               case WSAEPROTOTYPE:
                  return NETERR_WRONG_PROTOTYPE;
               case WSAENOPROTOOPT:
                  return NETERR_BAD_PROTOCOL_OPTION;
               case WSAEPROTONOSUPPORT:
                  return NETERR_PROTOCOL_NOT_SUPPORTED;
               case WSAESOCKTNOSUPPORT:
                  return NETERR_SOCKET_TYPE_NOT_SUPPORTED;
               case WSAEOPNOTSUPP:
                  return NETERR_OPERATION_NOT_SUPPORTED;
               case WSAEPFNOSUPPORT:
                  return NETERR_PROTOCOL_FAMILY_NOT_SUPPORT;
               case WSAEAFNOSUPPORT:
                  return NETERR_ADDRESS_FAMILY_NOT_SUPPORT;
               case WSAEADDRINUSE:
                  return NETERR_ADDRESS_IN_USE;
               case WSAEADDRNOTAVAIL:
                  return NETERR_ADDRESS_NOT_AVAILABLE;
               case WSAENETDOWN:
                  return NETERR_NETWORK_DOWN;
               case WSAENETUNREACH:
                  return NETERR_NETWORK_UNREACHABLE;
               case WSAENETRESET:
                  return NETERR_NETWORK_DROPPED_CONNECTION;
               case WSAECONNABORTED:
                  return NETERR_CONNECTION_RESET_BY_SOFTWARE;
               case WSAECONNRESET:
                  return NETERR_CONNECTION_RESET_BY_PEER;
               case WSAENOBUFS:
                  return NETERR_NO_BUFFER_SPACE_AVAIABLE;
               case WSAEISCONN:
                  return NETERR_ALREADY_CONNECTED;
               case WSAENOTCONN:
                  return NETERR_NOT_CONNECTED;
               case WSAESHUTDOWN:
                  return NETERR_SEND_SHUTDOWN;
               case WSAETOOMANYREFS:
                  return NETERR_TOO_MANY_REFS;
               case WSAETIMEDOUT:
                  return NETERR_CONNECTION_TIMED_OUT;
               case WSAECONNREFUSED:
                  return NETERR_CONNECTION_REFUSED;
               case WSAELOOP:
                  return NETERR_TRANSLATE_NAME;
               case WSAENAMETOOLONG:
                  return NETERR_NAME_TOO_LONG;
               case WSAEHOSTDOWN:
                  return NETERR_HOST_DOWN;
               case WSAEHOSTUNREACH:
                  return NETERR_HOST_UNREACHABLE;
               case WSAEPROCLIM:
                  return NETERR_PROCESS_LIMIT;
               case WSASYSNOTREADY:
                  return NETERR_NETWORK_SYSTEM_NOT_READY;
               case WSAVERNOTSUPPORTED:
                  return NETERR_VERSION_NOT_SUPPORTED;
               case WSANOTINITIALISED:
                  return NETERR_NETWORK_NOT_INITIALISED;
               case WSAEDISCON:
                  return NETERR_DISCONNECT_IN_PROGRESS;
               case WSAEINVALIDPROCTABLE:
                  return NETERR_INVALIDPROCTABLE;
               case WSAEINVALIDPROVIDER:
                  return NETERR_INVALIDPROVIDER;
               case WSAEPROVIDERFAILEDINIT:
                  return NETERR_PROVIDERFAILEDINIT;
               case WSASYSCALLFAILURE:
                  return NETERR_SYSCALLFAILURE;
               case WSAHOST_NOT_FOUND:
                  return NETERR_HOST_NOT_FOUND;
               case WSATRY_AGAIN:
                  return NETERR_TRY_AGAIN;
               case WSANO_RECOVERY:
                  return NETERR_NO_RECOVERY;
               case WSANO_DATA:
                  return NETERR_NO_DATA;
            }

            return NETERR_UNKNOWN;
         }

         const char *as_string(network_error_code error_code) {
            switch (error_code) {
               case NETERR_NO_ERROR:
                  return "NETERR_NO_ERROR";
               case NETERR_INVALID_HANDLE:
                  return "NETERR_INVALID_HANDLE";
               case NETERR_NOT_ENOUGH_MEMORY:
                  return "NETERR_NOT_ENOUGH_MEMORY";
               case NETERR_INVALID_PARAMETER:
                  return "NETERR_INVALID_PARAMETER";
               case NETERR_OPERATION_ABORTED:
                  return "NETERR_OPERATION_ABORTED";
               case NETERR_INTERUPTED_CALL:
                  return "NETERR_INTERUPTED_CALL";
               case NETERR_BAD_FILE_HANDLE:
                  return "NETERR_BAD_FILE_HANDLE,";
               case NETERR_SOCKET_ACCESS_DENIED:
                  return "NETERR_SOCKET_ACCESS_DENIED";
               case NETERR_BAD_ADDRESS:
                  return "NETERR_BAD_ADDRESS";
               case NETERR_INVALID_ARGUMENT:
                  return "NETERR_INVALID_ARGUMENT";
               case NETERR_TOO_MANY_OPEN_FILES:
                  return "NETERR_TOO_MANY_OPEN_FILES";
               case NETERR_WOULD_BLOCK:
                  return "NETERR_WOULD_BLOCK";
               case NETERR_IN_PROGRESS:
                  return "NETERR_IN_PROGRESS";
               case NETERR_ALREADY_IN_PROGRESS:
                  return "NETERR_ALREADY_IN_PROGRESS";
               case NETERR_HANDLE_NON_SOCKET:
                  return "NETERR_HANDLE_NON_SOCKET";
               case NETERR_DESTINATION_ADDRESS_REQUIRED:
                  return "NETERR_DESTINATION_ADDRESS_REQUIRED";
               case NETERR_MESSAGE_TOO_LONG:
                  return "NETERR_MESSAGE_TOO_LONG";
               case NETERR_WRONG_PROTOTYPE:
                  return "NETERR_WRONG_PROTOTYPE";
               case NETERR_BAD_PROTOCOL_OPTION:
                  return "NETERR_BAD_PROTOCOL_OPTION";
               case NETERR_PROTOCOL_NOT_SUPPORTED:
                  return "NETERR_PROTOCOL_NOT_SUPPORTED";
               case NETERR_SOCKET_TYPE_NOT_SUPPORTED:
                  return "NETERR_SOCKET_TYPE_NOT_SUPPORTED";
               case NETERR_OPERATION_NOT_SUPPORTED:
                  return "NETERR_OPERATION_NOT_SUPPORTED";
               case NETERR_PROTOCOL_FAMILY_NOT_SUPPORT:
                  return "NETERR_PROTOCOL_FAMILY_NOT_SUPPORT";
               case NETERR_ADDRESS_FAMILY_NOT_SUPPORT:
                  return "NETERR_ADDRESS_FAMILY_NOT_SUPPORT";
               case NETERR_ADDRESS_IN_USE:
                  return "NETERR_ADDRESS_IN_USE";
               case NETERR_ADDRESS_NOT_AVAILABLE:
                  return "NETERR_ADDRESS_NOT_AVAILABLE";
               case NETERR_NETWORK_DOWN:
                  return "NETERR_NETWORK_DOWN";
               case NETERR_NETWORK_UNREACHABLE:
                  return "NETERR_NETWORK_UNREACHABLE";
               case NETERR_NETWORK_DROPPED_CONNECTION:
                  return "NETERR_NETWORK_DROPPED_CONNECTION";
               case NETERR_CONNECTION_RESET_BY_SOFTWARE:
                  return "NETERR_CONNECTION_RESET_BY_SOFTWARE";
               case NETERR_CONNECTION_RESET_BY_PEER:
                  return "NETERR_CONNECTION_RESET_BY_PEER";
               case NETERR_NO_BUFFER_SPACE_AVAIABLE:
                  return "NETERR_NO_BUFFER_SPACE_AVAIABLE";
               case NETERR_ALREADY_CONNECTED:
                  return "NETERR_ALREADY_CONNECTED";
               case NETERR_NOT_CONNECTED:
                  return "NETERR_NOT_CONNECTED";
               case NETERR_SEND_SHUTDOWN:
                  return "NETERR_SEND_SHUTDOWN";
               case NETERR_TOO_MANY_REFS:
                  return "NETERR_TOO_MANY_REFS";
               case NETERR_CONNECTION_TIMED_OUT:
                  return "NETERR_CONNECTION_TIMED_OUT";
               case NETERR_CONNECTION_REFUSED:
                  return "NETERR_CONNECTION_REFUSED";
               case NETERR_TRANSLATE_NAME:
                  return "NETERR_TRANSLATE_NAME";
               case NETERR_NAME_TOO_LONG:
                  return "NETERR_NAME_TOO_LONG";
               case NETERR_HOST_DOWN:
                  return "NETERR_HOST_DOWN";
               case NETERR_HOST_UNREACHABLE:
                  return "NETERR_HOST_UNREACHABLE";
               case NETERR_PROCESS_LIMIT:
                  return "NETERR_PROCESS_LIMIT";
               case NETERR_NETWORK_SYSTEM_NOT_READY:
                  return "NETERR_NETWORK_SYSTEM_NOT_READY";
               case NETERR_VERSION_NOT_SUPPORTED:
                  return "NETERR_VERSION_NOT_SUPPORTED";
               case NETERR_NETWORK_NOT_INITIALISED:
                  return "NETERR_NETWORK_NOT_INITIALISED";
               case NETERR_DISCONNECT_IN_PROGRESS:
                  return "NETERR_DISCONNECT_IN_PROGRESS";
               case NETERR_INVALIDPROCTABLE:
                  return "NETERR_INVALIDPROCTABLE";
               case NETERR_INVALIDPROVIDER:
                  return "NETERR_INVALIDPROVIDER";
               case NETERR_PROVIDERFAILEDINIT:
                  return "NETERR_PROVIDERFAILEDINIT";
               case NETERR_SYSCALLFAILURE:
                  return "NETERR_SYSCALLFAILURE";
               case NETERR_HOST_NOT_FOUND:
                  return "NETERR_HOST_NOT_FOUND";
               case NETERR_TRY_AGAIN:
                  return "NETERR_TRY_AGAIN";
               case NETERR_NO_RECOVERY:
                  return "NETERR_NO_RECOVERY";
               case NETERR_NO_DATA:
                  return "NETERR_NO_DATA";
               case NETERR_REMOTE_DISCONNECT:
                  return "NETERR_REMOTE_DISCONNECT";
               case NETERR_SELECT_TIMEOUT:
                  return "NETERR_SELECT_TIMEOUT";
            }

            return "NETERR_UNKNOWN";
         }

         bool is_non_critical(network_error_code error_code) {
            switch (error_code) {
               case NETERR_NO_ERROR:
               case NETERR_WOULD_BLOCK:
               case NETERR_SELECT_TIMEOUT:
                  return true;
            }
            return false;
         }
      } // !error
   } // !network

   // static
   bool ip_address::local_addresses(dynamic_array<ip_address> &addresses) {
      DWORD size = 0;
      GetAdaptersAddresses(AF_INET,
                           GAA_FLAG_INCLUDE_PREFIX,
                           NULL,
                           NULL,
                           &size);

      IP_ADAPTER_ADDRESSES *adapter_addresses = (IP_ADAPTER_ADDRESSES *)calloc(1, size);
      GetAdaptersAddresses(AF_INET,
                           GAA_FLAG_INCLUDE_PREFIX,
                           NULL,
                           adapter_addresses,
                           &size);

      for (IP_ADAPTER_ADDRESSES *iter = adapter_addresses; iter != NULL; iter = iter->Next) {
         if (iter->OperStatus == IfOperStatusUp && (iter->IfType == IF_TYPE_ETHERNET_CSMACD || iter->IfType == IF_TYPE_IEEE80211)) {
            for (IP_ADAPTER_UNICAST_ADDRESS *ua = iter->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
               char addrstr[1024] = {};
               getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, addrstr, sizeof(addrstr), NULL, 0, NI_NUMERICHOST);
               if (ua->Address.lpSockaddr->sa_family == AF_INET) {
                  sockaddr_in ai = *(sockaddr_in *)ua->Address.lpSockaddr;
                  ip_address address;
                  address.host_ = ntohl(ai.sin_addr.s_addr);
                  address.port_ = ntohs(ai.sin_port);
                  addresses.push_back(address);
               }
            }
         }
      }

      free(adapter_addresses);

      return !addresses.empty();
   }

   bool ip_address::lookup(const string &dns, dynamic_array<ip_address> &addresses) {
      ADDRINFO *query_result = NULL;
      ADDRINFO hint = {};
      hint.ai_family = AF_INET;
      hint.ai_socktype = SOCK_DGRAM;
      bool result = getaddrinfo(dns.c_str(), NULL, &hint, &query_result) == 0;
      if (result) {
         ADDRINFO *iter = query_result;
         while (iter) {
            sockaddr_in addrin = *(sockaddr_in *)iter->ai_addr;
            ip_address address;
            address.host_ = ntohl(addrin.sin_addr.s_addr);
            address.port_ = ntohs(addrin.sin_port);
            addresses.push_back(address);

            iter = iter->ai_next;
         }
      }

      freeaddrinfo(query_result);

      return !addresses.empty();
   }

   ip_address::ip_address()
      : host_(0)
      , port_(0)
   {
   }

   ip_address::ip_address(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port)
      : host_(0)
      , port_(port)
   {
      host_ |= ((uint32)a << 24);
      host_ |= ((uint32)b << 16);
      host_ |= ((uint32)c << 8);
      host_ |= ((uint32)d << 0);
   }

   ip_address::ip_address(uint32 host, uint16 port)
      : host_(host)
      , port_(port)
   {
   }

   bool ip_address::operator==(const ip_address &rhs) const {
      return host_ == rhs.host_;
   }

   void ip_address::set_host(uint8 a, uint8 b, uint8 c, uint8 d) {
      host_ = 0;
      host_ |= ((uint32)a << 24);
      host_ |= ((uint32)b << 16);
      host_ |= ((uint32)c << 8);
      host_ |= ((uint32)d << 0);
   }

   void ip_address::set_port(uint16 port) {
      port_ = port;
   }

   bool ip_address::as_string(string &str) const {
      char buf[128] = {};
      sockaddr_in inaddr = network::to_sockaddr(*this);
      if (inet_ntop(AF_INET, &inaddr.sin_addr, buf, sizeof(buf))) {
         size_t len = strlen(buf);
         sprintf(buf + len, ":%d", port_);
         str = buf;
         return true;
      }
      return false;
   }

   udp_socket::udp_socket()
      : handle_(~0u)
   {
   }

   bool udp_socket::is_valid() const {
      return handle_ != ~0u;
   }

   void udp_socket::close() {
      if (!is_valid()) {
         return;
      }

      closesocket(handle_);
      handle_ = ~0u;
   }

   bool udp_socket::open(ip_address &addr) {
      if (is_valid()) {
         return false;
      }

      uint32_t handle = (int)socket(AF_INET, SOCK_DGRAM, 0);
      if (handle == ~0u) {
         return false;
      }

      // note: enable non-blocking mode
      u_long non_blocking = 1;
      if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) {
         return false;
      }

      // note: enable address reuse mode
      BOOL value = TRUE;
      if (setsockopt(handle, SOL_SOCKET, SO_REUSEADDR, (const char *)&value, sizeof(value)) != 0) {
         return false;
      }

      sockaddr_in addr_in = network::to_sockaddr(addr);
      if (bind(handle, (const sockaddr *)&addr_in, sizeof(addr_in)) != 0) {
         return false;
      }

      handle_ = handle;

      return true;
   }

   bool udp_socket::send_to(const ip_address &address, byte_stream &stream) {
      if (!is_valid()) {
         return false;
      }

      sockaddr_in addr_in = network::to_sockaddr(address);
      const char *data = (const char *)stream.base_;
      int size = (int)stream.length();
      int sent = 0;
      while (sent < size) {
         int result = sendto(handle_, data + sent, size - sent, 0, (const sockaddr *)&addr_in, sizeof(addr_in));
         if (result < 0) {
            return false;
         }

         sent += result;
      }

      return true;
   }

   bool udp_socket::recv_from(ip_address &address, byte_stream &stream) {
      if (!is_valid()) {
         return false;
      }

      char *base = (char *)stream.base_;
      int size = (int)stream.capacity();
      sockaddr_in addr_in = {};
      int remote_size = sizeof(addr_in);
      int result = recvfrom(handle_, base, size, 0, (sockaddr *)&addr_in, &remote_size);
      if (result < 0) {
         return false;
      }

      address.host_ = ntohl(addr_in.sin_addr.s_addr);
      address.port_ = ntohs(addr_in.sin_port);
      stream.at_ = const_cast<uint8_t *>(stream.base_) + result;

      return true;
   }

   bool udp_socket::address_of(ip_address &address) {
      if (!is_valid()) {
         return false;
      }

      int size = sizeof(sockaddr_in);
      sockaddr_in addr_in = {};
      addr_in.sin_family = AF_INET;
      if (getsockname(handle_, (sockaddr *)& addr_in, &size) != 0) {
         return false;
      }

      address = network::from_sockaddr(addr_in);

      return true;
   }

   byte_stream::byte_stream(uint64 capacity, uint8 *base)
      : capacity_(capacity)
      , base_(base)
      , at_(base)
   {
   }

   void byte_stream::reset() {
      at_ = base_;
   }

   uint64 byte_stream::capacity() const {
      return capacity_;
   }

   uint64 byte_stream::length() const {
      return (at_ - base_);
   }

   namespace {
      bool is_past_stream_end(const byte_stream &stream, const uint8 *cursor, const uint64 size) {
         uint32 length = (uint32)(cursor - stream.base_);
         return length + size > stream.capacity();
      }

      template <typename T>
      bool write_to(byte_stream &stream, uint8 *&cursor, T value) {
         if (!is_past_stream_end(stream, cursor, sizeof(T))) {
            *reinterpret_cast<T *>(cursor) = value;
            cursor += sizeof(T);
            return true;
         }

         return false;
      }

      template <typename T> 
      bool read_from(byte_stream &stream, uint8 *&cursor, T &value) {
         if (!is_past_stream_end(stream, cursor, sizeof(T))) {
            value = *reinterpret_cast<T *>(cursor);
            cursor += sizeof(T);
            return true;
         }

         return false;
      }
   }

   byte_stream_writer::byte_stream_writer(byte_stream &stream)
      : stream_(stream)
   {
   }

   bool byte_stream_writer::serialize(float value) {
      union  {
         float  f;
         uint32 u;
      } f2u;
      f2u.f = value;
      return serialize(f2u.u);
   }

   bool byte_stream_writer::serialize(uint64 value) {
      return write_to<uint64 >(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint32 value) {
      return write_to<uint32>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint16 value) {
      return write_to<uint16>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint8 value) {
      return write_to<uint8>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint32 count, const uint8 *values) {
      const uint32 size = count * sizeof(uint8);

      if (!is_past_stream_end(stream_, stream_.at_, size)) {
         for (uint32 index = 0; index < count; index++) {
            stream_.at_[index] = values[index];
         }

         stream_.at_ += size;

         return true;
      }

      return false;
   }

   byte_stream_reader::byte_stream_reader(byte_stream &stream)
      : stream_(stream)
      , cursor_(stream.base_)
   {
   }

   bool byte_stream_reader::serialize(float &value) {
      union 
      {
         uint32 u;
         float  f;
      } u2f;

      if (!serialize(u2f.u)) {
         return false;
      }

      value = u2f.f;

      return true;
   }

   bool byte_stream_reader::serialize(uint64 &value) {
      return read_from<uint64>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint32 &value) {
      return read_from<uint32>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint16 &value) {
      return read_from<uint16>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint8 &value) {
      return read_from<uint8>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(const uint32 count, uint8 *values) {
      const uint32 size = count * sizeof(uint8);

      if (!is_past_stream_end(stream_, cursor_, size)) {
         for (uint32 index = 0; index < count; index++) {
            values[index] = cursor_[index];
         }

         cursor_ += size;

         return true;
      }

      return false;
   }
} // !uu
