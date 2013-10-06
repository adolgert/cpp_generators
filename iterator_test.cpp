#define BOOST_TEST_MODULE iterator_test
#include "boost/test/included/unit_test.hpp"
#include "streaming.h"



BOOST_AUTO_TEST_SUITE( adaptor )

// Reading list for working with iterators.

// New style iterators
// http://www.boost.org/doc/libs/1_54_0/libs/iterator/doc/new-iter-concepts.html

// SCARY iterators
// http://www.open-std.org/jtc1/sc22/WG21/docs/papers/2009/n2911.pdf

// Range iterators
// http://www.boost.org/doc/libs/1_54_0/libs/range/doc/html/index.html

BOOST_AUTO_TEST_CASE( simple )
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


BOOST_AUTO_TEST_SUITE_END()
