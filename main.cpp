#include "SimSearcher.h"
#include <iostream>

using namespace std;


int main(int argc, char **argv) {
	SimSearcher searcher;

	vector<pair<unsigned, unsigned> > resultED;
	vector<pair<unsigned, double> > resultJaccard;

	unsigned q = 2, edThreshold = 2;
	double jaccardThreshold = 0.85;

	searcher.createIndex(argv[1], q);
	searcher.searchJaccard("abc", jaccardThreshold, resultJaccard);
	searcher.searchED("abc", edThreshold, resultED);

	for (auto item : resultED) {
		cout << item.first << " " << item.second << endl;
	}
	return 0;
}

