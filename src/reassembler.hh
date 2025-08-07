#pragma once

#include "byte_stream.hh"
#include<set>
#include<utility>

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
   /*
  * 插入一个新的子字符串，重新组装成一个字节流（ByteStream）。
  *   `first_index`：子字符串的第一个字节的索引
  *   `data`：子字符串本身
  *   `is_last_substring`：这个子字符串表示流的结束
  *   `output`：一个可变引用，指向输出写入器（Writer）
  *
  * Reassembler 的任务是将已索引的子字符串（可能是无序的，也可能是重叠的）重新组装回原始的字节流。当 Reassembler 得到流中的下一个字节时，它应该将该字节写入输出。
  *
  * 如果 Reassembler 获取到字节，虽然它们适合流的可用容量，但暂时不能写入（因为之前的字节仍未知），它应该将这些字节暂时存储在内部，直到缺失的字节被填补。
  *
  * Reassembler 应该丢弃任何超出流的可用容量的字节（即使填补了前面的缺口，这些字节也无法写入）。Reassembler 应该在写入最后一个字节后关闭流。
  */

  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  // This function is for testing only; don't add extra state to support it.
  // Reassembler 本身存储了多少字节？
  // 这个函数仅用于测试；不要添加额外的状态来支持它。
  uint64_t count_bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  ByteStream output_;

  //自己加的
  struct Node {
    std::pair<uint64_t,uint64_t> p;
    std::string data;

    bool operator<(const Node& other) const {
            return p.first < other.p.first;  // 从小到大排序
        }
  };


  //定义一个std::set用于存储当前获得的所有字节
  
  std::set<Node> Set = {};
  //记录已经加入到缓冲区中的最后一个字节的序号
  uint64_t index_in_buffer = -1;

  void merge_strings(uint64_t first_index,std::string data);



};
