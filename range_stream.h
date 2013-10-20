#ifndef _RANGE_STREAM_H_
#define _RANGE_STREAM_H_ 1

#include <memory>
#include "boost/iterator/iterator_facade.hpp"
#include "boost/random/uniform_01.hpp"


template<typename RNGen>
class StreamingRange
{
public:
  using value_type=double;
  using reference_type=double&;
  using difference_type=size_t;
private:
  using UniformNorm=boost::random::uniform_01<double>;
  size_t _cnt;
  UniformNorm _uniform;
  RNGen* _rn_gen;
  // The value to which we point is stored as a pointer because
  // the call to return a reference is itself const, so it cannot
  // return a reference to a non-const value. It can, however,
  // dereference a const pointer to a non-const value.
  std::unique_ptr<value_type> _value;

public:
  StreamingRange(RNGen& rn_gen, size_t cnt)
  : _rn_gen(&rn_gen), _cnt(cnt+1), _value(new value_type())
  {
    this->advance_begin(1);
  }

  StreamingRange()
  : _rn_gen(nullptr), _cnt(0), _value(nullptr)
  {
  }

  reference_type front() const { return *_value; }
  bool empty() const { return _cnt==0; }
  StreamingRange<RNGen>& advance_begin(difference_type n) {
    while (n>0)
    {
      if (_cnt>0)
      {
        *_value=_uniform(*_rn_gen);
        --_cnt;
      }
      --n;
    }
    return *this;
  }
};





template<class SimpleRange>
class RangeFacade
: public boost::iterator_facade
  <
  RangeFacade<SimpleRange>,
  typename SimpleRange::value_type,
  boost::forward_traversal_tag
  >
{
  SimpleRange* _range;

public:
  explicit RangeFacade(SimpleRange& range)
  : _range(&range)
  {}
  explicit RangeFacade()
  : _range(nullptr)
  {}

private:
  friend class boost::iterator_core_access;

  void increment()
  {
    _range->advance_begin(1);
  }

  // This equality test just asks whether both are empty.
  bool equal(RangeFacade const& other) const
  {
    bool l,r;
    if (_range!=nullptr)
    {
      l=_range->empty();
    }
    else
    {
      l=true;
    }
    if (other._range!=nullptr)
    {
      r=other._range->empty();
    }
    else
    {
      r=true;
    }
    return l&&r;
  }

  typename SimpleRange::value_type& dereference() const
  {
    return _range->front();
  }
};



#endif
