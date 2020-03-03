#include <iostream>
#include <fstream>
//#include <opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <Eigen/Core >
#include <string>
#include <vector>
//#include <functional>
//#include <iterator>
//#include <fstream>
//#include <random>
//#include <memory>
//#include <map>
//#include <set>
//#include <utility>

//#include <ctime>
#include "yyj01.h"
//#include "StrBlob.h"
#include "TextQuery.h"
//#include "SaleBooks.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
//using namespace cv;
//using namespace Eigen;



int main(int argc, char* argv[])
{  
	std::ifstream fin("test_doc.txt");
	TextQuery t(fin);
	Query a("are");
	Query b("of");
	Query c("to");
	print(cout, ((~(a & b)) | c).eval(t));
	return 0;
}

	
