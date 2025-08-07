#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  debug( "---- insert({}, {}, {}) called ----", first_index, data, is_last_substring );
  //首先检查是否是否是空字符串并且已经达到了末尾,如果到了并且data为空，我们直接关闭缓冲区即可
  if(first_index == this->index_in_buffer && data.size() == 0 && is_last_substring){
    this->output_.writer().close();
    return ;
  } 
  if(is_last_substring){
    
  }
  //检查一下data是否有部分已经送入缓冲区中，如果有就修改data和first_index
  if(first_index <= index_in_buffer){
    uint64_t prefix = index_in_buffer + 1 - first_index;
    std::string s_t = data.substr(prefix, data.size() - prefix);
    swap(s_t,data);
    first_index += prefix; 
  }

  //首先合并字符串,顺便将字符串添加进set中
  merge_strings(first_index,data);

  //检查set中最左侧的字符串是否能够添加进缓冲区中
  Node node_left = *this->Set.begin();
  if(this->index_in_buffer+1 == node_left.p.first){
    //如果是就写入缓冲区中
    this->output_.writer().push(node_left.data);
    //删除最左侧的节点
    this->Set.erase(this->Set.begin());
    //更新缓冲区中最后一个字节的序号
    this->index_in_buffer += node_left.data.size();
  }

}
void Reassembler::merge_strings(uint64_t first_index,std::string data){
  Node n = {{first_index,first_index+data.size()},data};

  //首先检查set中是否存在和需要合并的data相同起始位置的string
  for(auto it = this->Set.begin();it != this->Set.end();it++){
    if((*it).p.first == first_index){
      //发现有相同起始位置的string之后检查谁更长
      if((*it).data.size() >= data.size()){
        //原来的更长或一样长，那之前已经有了，不进行任何操作
        return ;
      }
      else {
        // 发现新的更长
        this->Set.erase(it);
        break;
      }
    }
  }

  auto Left = this->Set.lower_bound(n);
  auto Right = this->Set.upper_bound(n);
  //检查左侧能否合并
  if(Left != this->Set.end() && Left->p.second + 1 >= n.p.first){
    //进行字符串合并操作
    uint64_t padding = Left->p.second + 1 - n.p.first;
    std::string s_new(Left->data);
    for(uint64_t i = padding ;i<n.data.length();i++){
      s_new.push_back(n.data[i]);
    }

    uint64_t first = Left->p.first;

    //删除旧节点
    this->Set.erase(Left);
    //修改新的节点
    n.p.first = first;
    n.data = s_new;
  }
  //检查右侧能否合并
  if(Right != this->Set.end() && Right->p.first <= n.p.second + 1){
    //进行字符串合并操作
    uint64_t padding = n.p.second + 1 - Right->p.first;
    std::string s_new(n.data);
    for(uint64_t i = padding ;i<Right->data.length();i++){
      s_new.push_back(Right->data[i]);
    }
    uint64_t second = Right->p.second;

    //删除旧节点
    this->Set.erase(Right);
    //修改新的节点
    n.p.second = second;
    n.data = s_new;
  }
    this->Set.insert(n);
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  uint64_t size {};
  for(auto node : this->Set){
    size += node.data.size();
  }
  return size;
}
