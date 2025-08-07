#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <deque>
#include <iostream>

class Reader;
class Writer;

class ByteStream
{
public:
  //explicit 关键字：防止编译器进行隐式类型转换，只能显式调用。
  explicit ByteStream( uint64_t capacity );

  // Helper functions (provided) to access the ByteStream's Reader and Writer interfaces
  //（已提供的）辅助函数，用于访问 ByteStream 的 Reader（读取器）和 Writer（写入器）接口。
  Reader& reader();
  const Reader& reader() const;
  Writer& writer();
  const Writer& writer() const;

  void set_error() { error_ = true; };       // Signal that the stream suffered an error.
  bool has_error() const { return error_; }; // Has the stream had an error?

protected:
  // Please add any additional state to the ByteStream here, and not to the Writer and Reader interfaces.
  uint64_t capacity_;
  //等价于 bool error_ = false;，但 {} 更通用，意为置该元素的默认值,适用于所有类型。
  bool error_ {};

  //需要自己添加的状态位和函数
  // 将字节保存在一个双端队列中
  std::string buffer {};
  //所有被压入byte_stream中字符的数量
  uint64_t total_bytes_pushed {};

  //当前保存在byte_stream中的第一个字节的前一个字节的下标。
  uint64_t byte_start = -1;
  //保存在byte_stream中的最后一个字节的下标。
  uint64_t byte_end = -1;
  //缓冲区是否关闭
  bool closed_ {};

};

class Writer : public ByteStream
{
public:
  void push( std::string data ); // Push data to stream, but only as much as available capacity allows.
  void close();                  // Signal that the stream has reached its ending. Nothing more will be written.

  bool is_closed() const;              // Has the stream been closed?
  uint64_t available_capacity() const; // How many bytes can be pushed to the stream right now?
  uint64_t bytes_pushed() const;       // Total number of bytes cumulatively pushed to the stream
};

class Reader : public ByteStream
{
public:
  std::string_view peek() const; // Peek at the next bytes in the buffer 查看缓冲区中的下一个字节序列（不会移除）
  void pop( uint64_t len );      // Remove `len` bytes from the buffer 从缓冲区移除 `len` 个字节

  bool is_finished() const;        // Is the stream finished (closed and fully popped)? 流是否已结束（已关闭且全部弹出）？
  uint64_t bytes_buffered() const; // Number of bytes currently buffered (pushed and not popped) 当前缓冲区中尚未弹出的字节数
  uint64_t bytes_popped() const;   // Total number of bytes cumulatively popped from stream 累计从流中弹出的总字节数
};

/*
 * read: A (provided) helper function thats peeks and pops up to `max_len` bytes
 * from a ByteStream Reader into a string;
 */
void read( Reader& reader, uint64_t max_len, std::string& out );
