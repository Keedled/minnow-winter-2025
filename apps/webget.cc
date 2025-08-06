#include "socket.hh"

#include <cstdlib>
#include <iostream>
#include <span>
#include <string>

using namespace std;

void get_URL( const string& host, const string& path )
{
  // 创建套接字
  TCPSocket s;
  // 设置服务器ip,并连接
  s.connect(Address(host,"80"));
  // 创建收发消息的字符串
  string Recv;
  string Send;
  // - 发送和接收消息
  Send.append("GET "+path+" HTTP/1.1\r\n");
  Send.append("Host: "+host+"\r\n");
  Send.append("Connection: close\r\n");
  Send.append("\r\n");
  s.write(Send);

  while(true){
    Recv.clear();
    s.read(Recv);
    if(Recv.empty())break;
    cout<<Recv;
  }
  s.read(Recv);
  cout<<Recv;
  s.read(Recv);
  cout<<Recv;
  // - 关闭套接字
  s.close();  
}

int main( int argc, char* argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort(); // For sticklers: don't try to access argv[0] if argc <= 0.
    }

    auto args = span( argv, argc );

    // The program takes two command-line arguments: the hostname and "path" part of the URL.
    // Print the usage message unless there are these two arguments (plus the program name
    // itself, so arg count = 3 in total).
    if ( argc != 3 ) {
      cerr << "Usage: " << args.front() << " HOST PATH\n";
      cerr << "\tExample: " << args.front() << " stanford.edu /class/cs144\n";
      return EXIT_FAILURE;
    }

    // Get the command-line arguments.
    const string host { args[1] };
    const string path { args[2] };

    // Call the student-written function.
    get_URL( host, path );
  } catch ( const exception& e ) {
    cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
