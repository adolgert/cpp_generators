#ifndef _STREAMING_H_
#define _STREAMING_H_

/*! Writing C++ iterators in the style of Python generators.
 *  Python generators return either the next item or None. In C++, we use
 *  a pair of iterators begin() and end() in order to accomplish the same
 *  behavior because we can't change the type to None when we feel like it.
 *  While the iterator code is a mess, despite help from Boost, the core
 *  of main() looks fine thanks to the C++ auto type.
 */
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/array.hpp>


/*! Reads a file line by line. The istream_iterator reads into string 
 *  token by token, stripping whitespace, which we don't want to do.
 */
class file_iterator
  : public boost::iterator_facade<
        file_iterator
      , const std::string
      , boost::forward_traversal_tag
    >
{
	std::istream& in_stream_;
	std::string   line_;
	bool end_;
 public:
    explicit file_iterator(std::istream& in_stream,bool end=false)
      : in_stream_(in_stream), end_(end)
    {
		if (!end_) increment();
    }
private:
	friend class boost::iterator_core_access;

	void increment() {
		if (in_stream_.good()) {
			getline(in_stream_,line_);
		} else {
			end_=true;
		}
	}

    bool equal(file_iterator const& other) const
    {
		return end_ && other.end_;
    }

	const reference dereference() const { return line_; }
};



/*! This builder makes a pair of begin and end iterators to read each line of a stream.
 */
boost::array<file_iterator,2> file_by_line(std::istream& in_stream) {
	boost::array<file_iterator,2> iters = {{
		file_iterator(in_stream),file_iterator(in_stream,true)
	}};
	return iters;
}



/*! Split a string into a vector of strings, stripping whitespace.
 *  STRING_SOURCE is any iterator where *iter is an std::string.
 */
template<class STRING_SOURCE>
class split_iterator
  : public boost::iterator_facade<
        split_iterator<STRING_SOURCE>
      , const std::vector<std::string>
      , boost::forward_traversal_tag
    >
{
	boost::array<STRING_SOURCE,2>& source_;
	std::vector<std::string> field_;
	bool end_;
 public:
    explicit split_iterator(boost::array<STRING_SOURCE,2>& source,bool end=false)
		: source_(source), end_(end)
    {
		if (!end_) increment();
    }
private:
	friend class boost::iterator_core_access;

	void increment() {
		if (source_[0]==source_[1]) {
			end_=true;
			return;
		}
		field_.clear();
		const std::string& line=*source_[0];
		std::istringstream line_stream(*source_[0]);
		
		std::istream_iterator<std::string> f_begin(line_stream);
		std::istream_iterator<std::string> f_end;
		while (f_begin!=f_end) {
			field_.push_back(*f_begin);
			f_begin++;
		}
		source_[0]++;
	}

    bool equal(split_iterator const& other) const
    {
		return end_==other.end_;
    }

	const std::vector<std::string>& dereference() const { return field_; }
};



template<class T>
boost::array<split_iterator<T>,2> split_line(boost::array<T,2>& begin_end) {
	boost::array<split_iterator<T>,2> iters = {{
		split_iterator<T>(begin_end), split_iterator<T>(begin_end,true)
	}};
	return iters;
}




/*! An iterator that returns multiple copies of an input iterator.
 *  If there is a single loop consuming this iterator, it can increment this iterator
 *  twice and receive the same value.
 */
template<class SOURCE>
class tee_iterator
  : public boost::iterator_facade<
        tee_iterator<SOURCE>
      , const typename SOURCE::value_type
      , boost::forward_traversal_tag
    >
{
	boost::array<SOURCE,2>& source_;
	typename SOURCE::value_type field_;
	bool end_;
	size_t idx_;
	size_t cnt_;
 public:
    explicit tee_iterator(boost::array<SOURCE,2>& source, bool end=false)
		: source_(source), end_(end), idx_(0), cnt_(2)
    {
		if (!end_) increment();
    }
private:
	friend class boost::iterator_core_access;

	void increment() {
		if (idx_==0) {
			if (source_[0]==source_[1]) {
				end_=true;
				return;
			}
			field_=*source_[0];
			source_[0]++;
		}
		idx_++;
		idx_=idx_ % cnt_;
	}

    bool equal(tee_iterator const& other) const
    {
		return end_==other.end_;
    }

	typename SOURCE::reference dereference() const { return field_; }
};



template<class T>
boost::array<tee_iterator<T>,2> tee(boost::array<T,2>& begin_end) {
	boost::array<tee_iterator<T>,2> iters = {{
		tee_iterator<T>(begin_end), tee_iterator<T>(begin_end,true)
	}};
	return iters;
}



#endif
