#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

void Writer::push( string data )
{
  uint64_t length = data.size();
  if(is_closed()){
    cerr<<"*** This ByteStream has been full!**\n";
    return;
  }
  //如果缓冲区可用长度小于等于data.size(),那需要修正data的长度.
  if(this->available_capacity()<=length){
    length = this->available_capacity();
  }
  //进入缓冲区数量加
  this->total_bytes_pushed += length;
  //复制进入缓冲区
  for(uint64_t i = 0;i<length;i++){
    this->buffer.push_back(data[i]);
  }
  //更新缓冲区中字节流最后的位置
  this->byte_end += length; 
}

void Writer::close()
{
  this->closed_ = true;
}

bool Writer::is_closed() const
{
  return this->closed_;
}

uint64_t Writer::available_capacity() const
{
  return this->capacity_ - (this->byte_end - this->byte_start);
}

uint64_t Writer::bytes_pushed() const
{
  return this->total_bytes_pushed;
}

//查看缓冲区中的下一个字节序列（不会移除）
string_view Reader::peek() const
{
  return std::string_view(this->buffer.begin(),this->buffer.end());
}

void Reader::pop( uint64_t len )
{
  uint64_t length = len;
  if(length > this->byte_end - this->byte_start){
    length = this->byte_end - this->byte_start;
  }
  this->byte_start += length;
  this->buffer.erase(0,length);
  
}

bool Reader::is_finished() const
{
  return this->closed_ && this->byte_end == this->byte_start;
}

uint64_t Reader::bytes_buffered() const
{
  return this->byte_end - this->byte_start;
}

uint64_t Reader::bytes_popped() const
{
  return this->byte_start + 1;
}

