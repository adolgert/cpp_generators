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
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/array.hpp>


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
		size_t pos = 0;
		size_t field_start=0;
		size_t field_finish=0;
		while (pos!=std::string::npos) {
			pos=line.find_first_not_of(" \n\t\r", pos);
			if (pos!=std::string::npos) {
				field_start=pos;
				pos=line.find_first_of(" \n\t\r",pos);
				if (pos!=std::string::npos) {
					field_.push_back(line.substr(field_start,pos-field_start));
				} else {
					field_.push_back(line.substr(field_start,line.size()-field_start));
				}
			}
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



/*!
Drew-Dolgerts-MacBook-Pro:generative ajd27$ ./gen
line to split: header0 header1 header2
  split result: header0:header1:header2:
line to split:   2.5 3.7   4.2
  split result: header0:header1:header2:
line to split:   2.5 3.7   4.2
  split result: 2.7:4.5:3.2:
line to split: 
  split result: 2.7:4.5:3.2:
 */
int main(int argc, char* argv[])
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
	return 0;
}
