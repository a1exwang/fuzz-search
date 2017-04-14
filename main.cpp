#include "SimSearcher.h"
#include <iostream>

using namespace std;


int main(int argc, char **argv) {
	SimSearcher searcher;
  auto query = argv[2];

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 2, edThreshold = 10;
//	double jaccardThreshold = 0.6;

	searcher.createIndex(argv[1], q);

//	searcher.searchJaccard(query, jaccardThreshold, resultJaccard);
//	cout << "Jaccard" << endl;
//	for (auto item : resultJaccard) {
//		cout << item.first << " " << item.second << endl;
//	}

	searcher.searchED(query, edThreshold, resultED);
	cout << "ED" << endl;
	for (auto item : resultED) {
		cout << item.first << " " << item.second << endl;
	}
	return 0;
}

