#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define BOOST_UBLAS_NESTED_CLASS_DR45

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/sinc.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <kml/gaussian.hpp>
#include <kml/online_svm.hpp>
#include <kml/io.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using std::string; using std::cout; using std::endl; 
using std::ifstream; using std::stringstream;
using std::getline; using std::cerr;

namespace ublas = boost::numeric::ublas;

typedef kml::classification<ublas::vector<double>, bool> problem_type;
typedef std::vector<ublas::vector<double> >::iterator vec_iter;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    cout << "Error: need an input file to train and test on." << endl 
	      << "usage: svm_classification (trainfile) (testfile)" << endl;
    return 0;
  }

  std::vector<ublas::vector<double> > points;
  std::vector<problem_type::output_type> target;

  cerr << "Reading in vector...";
  kml::read_svm_light(argv[1], points, target);
  cerr << "Done, " << points.size() << " points of size " << points[0].size() << endl;
  kml::online_svm<problem_type, kml::gaussian> my_machine(3.162277, 1.0);

  for( unsigned int i=0; i<points.size(); ++i ) my_machine.push_back( points[i], target[i] );

  //my_machine.learn(points, target);
  cerr << "Done training" << endl;
  for (std::vector<double>::iterator i = my_machine.weight.begin();
       i != my_machine.weight.end(); ++i)
    cout << boost::lexical_cast<double>(*i) << " ";
  cout << endl;

  std::vector<ublas::vector<double> > testpoints;
  std::vector<problem_type::output_type> testtarget;
  cerr << "Reading in test vector...";
  kml::read_svm_light(argv[2], testpoints, testtarget);
  cerr << "Done, " << testpoints.size() << " points of size " << testpoints[0].size() << endl;
  //for (vec_iter i = testpoints.begin(); i != testpoints.end(); ++i) {
  for( unsigned int i=0; i<testpoints.size(); ++i ) {
  
  	//cout << my_machine.evaluate_f(*i) << " ";
        cout << my_machine(testpoints[i]) << "  " << testtarget[i] << std::endl;
	
    }
  return 0;
}
