#ifndef UTIL_NODE_H
#define UTIL_NODE_H

namespace util {
  class node {
    public:
      node* prev;
      node* next;

      // Constructor.
      node();

      // Clear.
      void clear();
  };

  inline node::node()
  {
    clear();
  }

  inline void node::clear()
  {
    prev = nullptr;
    next = nullptr;
  }
}

#endif // UTIL_NODE_H
