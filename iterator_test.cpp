#define BOOST_TEST_MODULE iterator_test

#include <map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <type_traits>
#include "boost/test/included/unit_test.hpp"
#include "boost/range/adaptors.hpp"
#include "boost/random/mersenne_twister.hpp"
#include "streaming.h"
#include "range_stream.h"
#include "demangle.h"



BOOST_AUTO_TEST_SUITE( adaptor )

// Reading list for working with iterators.

// New style iterators
// http://www.boost.org/doc/libs/1_54_0/libs/iterator/doc/new-iter-concepts.html

// SCARY iterators
// http://www.open-std.org/jtc1/sc22/WG21/docs/papers/2009/n2911.pdf

// Range iterators
// http://www.boost.org/doc/libs/1_54_0/libs/range/doc/html/index.html

BOOST_AUTO_TEST_CASE( map_first )
{
  using MapType=std::map<int,double>;
  using KeyType=boost::range_detail::select_first_range<std::map<int, double>>;
  MapType m_map;
  m_map.insert(std::make_pair(1,3.0));
  m_map.insert(std::make_pair(2,7.0));

  KeyType by_key(m_map| boost::adaptors::map_keys);
  std::cout << show_name(by_key) << std::endl;
  for (auto k: by_key)
  {
    std::cout << k << std::endl;
  }

  auto by_initializer_list{m_map| boost::adaptors::map_keys};
  std::cout << show_name(by_initializer_list) << std::endl;
  for (auto k: by_initializer_list)
  {
    std::cout << k << std::endl;
  }



}



BOOST_AUTO_TEST_CASE( pipeline )
{
  // Construct the pipeline. The result of each step is
  // a pair of iterators in a boost::array<ITER,2>.
  std::ifstream in_file("z.txt");
  auto file_line=file_by_line(in_file);
  auto tee_line=tee(file_line);
  auto splits=split_line(tee_line);

  // Invoke the pipeline by requesting results.
  auto the_line=tee_line[0];
  auto splitted=splits[0];
  
  while (splitted!=splits[1]) {
    std::cout << "line to split: " << *the_line << std::endl;
    std::cout << "  split result: ";
    for (auto word=begin(*splitted); word!=end(*splitted); word++) {
      std::cout << *word << ":";
    }
    std::cout << std::endl;
    
    the_line++;
    splitted++;
  }
}


BOOST_AUTO_TEST_CASE( random_stream )
{
  boost::random::mt19937 rn_gen;
  StreamingRange<decltype(rn_gen)> sr(rn_gen, 5);

  std::cout << "Printing random numbers." << std::endl;
  while (!sr.empty())
  {
    std::cout << "n=" << sr.front() << std::endl;
    sr.advance_begin(1);
  }
}


BOOST_AUTO_TEST_SUITE_END()
