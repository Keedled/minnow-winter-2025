#pragma once

#include "address.hh"
#include "file_descriptor.hh"

#include <functional>
#include <sys/socket.h>

//! \brief Base class for network sockets (TCP, UDP, etc.)
//! \details Socket is generally used via a subclass. See TCPSocket and UDPSocket for usage examples.
class Socket : public FileDescriptor
{
private:
  //! Get the local or peer address the socket is connected to
  Address get_address( const std::string& name_of_function,
                       const std::function<int( int, sockaddr*, socklen_t* )>& function ) const;

protected:
  //! Construct via [socket(2)](\ref man2::socket)
  //通过 socket(2) 系统调用创建套接字（socket）。
  Socket( int domain, int type, int protocol = 0 );

  //! Construct from a file descriptor.
  //用一个文件描述符构造 Socket 对象。
  Socket( FileDescriptor&& fd, int domain, int type, int protocol = 0 );

  //! Wrapper around [getsockopt(2)](\ref man2::getsockopt)
  //对 getsockopt(2) 系统调用的面向对象包装。
  template<typename option_type>
  socklen_t getsockopt( int level, int option, option_type& option_value ) const;

  //! Wrappers around [setsockopt(2)](\ref man2::setsockopt)
  template<typename option_type>
  void setsockopt( int level, int option, const option_type& option_value );

  void setsockopt( int level, int option, std::string_view option_val );

public:
  //! Bind a socket to a specified address with [bind(2)](\ref man2::bind), usually for listen/accept
  // 用 bind(2) 系统调用将 socket 绑定到指定地址（一般用于监听和接收连接）。
  void bind( const Address& address );

  //! Bind a socket to a specified device
  // 将 socket 绑定到指定的设备（比如绑定到某个网卡，如 "eth0"）。
  void bind_to_device( std::string_view device_name );

  //! Connect a socket to a specified peer address with [connect(2)](\ref man2::connect)
  // 用 connect(2) 系统调用将 socket 连接到指定的对端地址（用于客户端主动发起连接）。
  void connect( const Address& address );

  //! Shut down a socket via [shutdown(2)](\ref man2::shutdown)
  // 使用 shutdown(2) 系统调用关闭 socket 的一部分或全部连接。
  void shutdown( int how );

  //! Get local address of socket with [getsockname(2)](\ref man2::getsockname)
  // 用 getsockname(2) 获取本地 socket 绑定的地址信息。
  Address local_address() const;
  //! Get peer address of socket with [getpeername(2)](\ref man2::getpeername)
  // 用 getpeername(2) 获取对端（已连接）的地址信息。
  Address peer_address() const;

  //! Allow local address to be reused sooner via [SO_REUSEADDR](\ref man7::socket)
  // 通过设置 SO_REUSEADDR 选项，允许本地地址端口被更快地重用。
  void set_reuseaddr();

  //! Check for errors (will be seen on non-blocking sockets)
  void throw_if_error() const;
};

class DatagramSocket : public Socket
{
public:
  //! Receive a datagram and the Address of its sender
  // 接收一个数据报（datagram，通常指UDP包），并获取发送方的地址。
  void recv( Address& source_address, std::string& payload );

  //! Send a datagram to specified Address
  // 向指定地址发送一个数据报（datagram）。
  void sendto( const Address& destination, std::string_view payload );

  //! Send datagram to the socket's connected address (must call connect() first)
  // 向已连接的对端地址发送数据报（必须先用connect()指定目标）。
  void send( std::string_view payload );

protected:
  DatagramSocket( int domain, int type, int protocol = 0 ) : Socket( domain, type, protocol ) {}

  //! Construct from a file descriptor.
  DatagramSocket( FileDescriptor&& fd, int domain, int type, int protocol = 0 )
    : Socket( std::move( fd ), domain, type, protocol )
  {}
};

//! A wrapper around [UDP sockets](\ref man7::udp)
class UDPSocket : public DatagramSocket
{
  //! \param[in] fd is the FileDescriptor from which to construct
  explicit UDPSocket( FileDescriptor&& fd ) : DatagramSocket( std::move( fd ), AF_INET, SOCK_DGRAM ) {}

public:
  //! Default: construct an unbound, unconnected UDP socket
  UDPSocket() : DatagramSocket( AF_INET, SOCK_DGRAM ) {}
};

//! A wrapper around [TCP sockets](\ref man7::tcp)
//一个用于 TCP socket 的封装类。
class TCPSocket : public Socket
{
private:
  //! \brief Construct from FileDescriptor (used by accept())
  //! \param[in] fd is the FileDescriptor from which to construct
  //简要说明（brief）： 通过一个 FileDescriptor 构造 TCPSocket（通常由 accept() 调用时使用）。
  //参数说明（param[in] fd）： 这是一个用于构造 TCPSocket 的 FileDescriptor 对象（右值引用，即临时对象，资源转移）。
  explicit TCPSocket( FileDescriptor&& fd ) : Socket( std::move( fd ), AF_INET, SOCK_STREAM, IPPROTO_TCP ) {}

public:
  //! Default: construct an unbound, unconnected TCP socket
  //默认构造一个“未绑定、未连接”的 TCP socket。
  TCPSocket() : Socket( AF_INET, SOCK_STREAM ) {}

  //! Mark a socket as listening for incoming connections
  //把 socket 标记为“监听状态”，可以接受传入的连接（服务器端用）。
  //参数 backlog（默认16）：操作系统内核等待队列的最大连接数。
  //就是把 socket 变成一个“服务端”，等待客户端连接。
  void listen( int backlog = 16 );

  //! Accept a new incoming connection
  TCPSocket accept();
};

//! A wrapper around [packet sockets](\ref man7:packet)
class PacketSocket : public DatagramSocket
{
public:
  PacketSocket( const int type, const int protocol ) : DatagramSocket( AF_PACKET, type, protocol ) {}

  void set_promiscuous();
};

//! A wrapper around [Unix-domain stream sockets](\ref man7::unix)
class LocalStreamSocket : public Socket
{
public:
  //! Construct from a file descriptor
  explicit LocalStreamSocket( FileDescriptor&& fd ) : Socket( std::move( fd ), AF_UNIX, SOCK_STREAM ) {}
};

//! A wrapper around [Unix-domain datagram sockets](\ref man7::unix)
class LocalDatagramSocket : public DatagramSocket
{
  //! \param[in] fd is the FileDescriptor from which to construct
  explicit LocalDatagramSocket( FileDescriptor&& fd ) : DatagramSocket( std::move( fd ), AF_UNIX, SOCK_DGRAM ) {}

public:
  //! Default: construct an unbound, unconnected socket
  LocalDatagramSocket() : DatagramSocket( AF_UNIX, SOCK_DGRAM ) {}
};
