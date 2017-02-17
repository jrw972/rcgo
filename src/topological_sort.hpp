#ifndef RC_SRC_TOPOLOGICAL_SORT_HPP
#define RC_SRC_TOPOLOGICAL_SORT_HPP

#include <iterator>

namespace util
{

struct NodeInfo
{
  NodeInfo ()
    : mark (Not_Started)
  { }

  enum Mark
  {
    Not_Started,
    In_Progress,
    Finished,
    Cyclic
  };
  Mark mark;
  size_t idx;
};

template <typename T>
void visit (T* node, std::map<T*, NodeInfo>& info)
{
  NodeInfo& i = info[node];
  switch (i.mark)
    {
    case NodeInfo::Not_Started:
      i.mark = NodeInfo::In_Progress;
      for (typename T::const_adjacency_iterator pos = node->adjacency_begin ()
           , limit = node->adjacency_end ();
           pos != limit;
           ++pos)
        {
          visit (*pos, info);
        }
      if (i.mark == NodeInfo::In_Progress)
        {
          i.mark = NodeInfo::Finished;
        }
      break;
    case NodeInfo::In_Progress:
      // A cycle has formed.
      i.mark = NodeInfo::Cyclic;
      break;
    case NodeInfo::Finished:
      // Do nothing.
      break;
    case NodeInfo::Cyclic:
      // Do nothing.
      break;
    }
}

template <typename Iter>
void topological_sort (Iter begin, Iter end)
{
  typedef std::iterator_traits<Iter> traits;
  //typename traits::difference_type count = std::distance (begin, end);

  std::map<typename traits::value_type, NodeInfo> info;
  for (Iter pos = begin; pos != end; ++pos)
    {
      info[*pos] = NodeInfo ();
    }

  for (Iter pos = begin; pos != end; ++pos)
    {
      NodeInfo& i = info[*pos];
      if (i.mark == NodeInfo::Not_Started)
        {
          visit (*pos, info);
        }
    }

  // TODO:  Finish the topological sort.
}
}

#endif // RC_SRC_TOPOLOGICAL_SORT_HPP
