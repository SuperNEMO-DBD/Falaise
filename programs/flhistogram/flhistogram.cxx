/* From histogram/gsl-histogram.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
 * Copyright (C) 2026 François Mauger
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/algorithm/string/trim.hpp>

#include <mygsl/histogram.h>

int main(int argc_, char ** argv_)
{
  // bool debug  = false
  double a = 0.0;
  double b = 1.0;
  size_t n = 10;
  int precision = 10;

  if (argc_ < 4) {
    std::cerr << "Usage: flhistogram xmin xmax n [precision]" << '\n';
    std::cerr << "Computes a histogram of the data on stdin using n bins from xmin to xmax.\n";
    exit(EXIT_FAILURE);
  }

  try {
    a = std::atof(argv_[1]);
    b = std::atof(argv_[2]);
    n = std::atoi(argv_[3]);
    // std::clog << "[debug] a=" << a << '\n';
    // std::clog << "[debug] b=" << b << '\n';
    // std::clog << "[debug] n=" << n << '\n';
    if (argc_ >= 5) {
      precision = atoi(argv_[4]);
    }
    // std::clog << "[debug] precision=" <<precision << '\n';
    {
      mygsl::histogram h(n, a, b);
      double binWidth = (b - a) / n;
      while (std::cin and not std::cin.eof()) {
	std::string line;
	std::getline(std::cin, line);
	boost::algorithm::trim_copy(line);
	if (line.empty()) continue;
	if (line[0] == '#') continue;
	std::istringstream strIn(line);
	double x;
	strIn >> x;
	if (!strIn) {
	  throw std::logic_error("flhistogram: input format error");
	}
	h.fill(x);
	std::cin >> std::ws;
      }

      {
	double min = h.min();
	double max = h.max();
	auto nbins = h.bins();
	double mean = h.mean();
	double sigma = h.sigma();
	double sum = h.sum();
	std::cout.precision(precision);
	std::cout << "#@min=" << min << '\n';
	std::cout << "#@max=" << max << '\n';
	std::cout << "#@nbins=" << nbins << '\n';
	std::cout << "#@binwidth=" << binWidth << '\n';
	std::cout << "#@mean=" << mean << '\n';
	std::cout << "#@sigma=" << sigma << '\n';
	std::cout << "#@sum=" << sum << std::endl;
	for (auto iBin = 0u; iBin < nbins; iBin++) {
	  std::pair<double,double> binBounds = h.get_range(iBin);
	  std::cout << binBounds.first << ' ' <<  binBounds.second << ' ' << h.get(iBin) << '\n';
	}
	std::cout << std::endl;
      }
    }
  } catch (std::exception & error) {
    std::cerr << "flhistogram: error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
