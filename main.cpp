#include "SimSearcher.h"
#include "SimSearcherC.h"
#include <iostream>
#include <sys/time.h>

using namespace std;


int main(int argc, char **argv) {
	SimSearcher searcher;
  SimSearcherC searcherc;
  auto query = argv[2];

	vector<pair<unsigned, unsigned>> resultED;
  vector<pair<unsigned, unsigned>> resultEDC;
	vector<pair<unsigned, double>> resultJaccard;
	vector<pair<unsigned, double>> resultJaccardC;

	unsigned q = 0, edThreshold = 8;
	double jaccardThreshold = 0.6;

	int r1 = searcher.createIndex(argv[1], q);
  int r2 = searcherc.createIndex(argv[1], q);
	if (r1 != r2) {
		cout << "create index results are not the same!" << endl;
		exit(1);
	}

	timeval t1, t2, t3;
	gettimeofday(&t1, NULL);

	searcher.searchJaccard(query, jaccardThreshold, resultJaccard);

  gettimeofday(&t2, NULL);
	searcherc.searchJaccard(query, jaccardThreshold, resultJaccardC);
	gettimeofday(&t3, NULL);

	double dt1 = (t2.tv_sec + t2.tv_usec / 1000000.0) - (t1.tv_sec + t1.tv_usec / 1000000.0);
	double dt2 = (t3.tv_sec + t3.tv_usec / 1000000.0) - (t2.tv_sec + t2.tv_usec / 1000000.0);
	cout << "Jaccard" << endl;
	for (auto item : resultJaccard) {
		cout << item.first << " " << item.second << endl;
	}

	cout << "Jaccard Correct" << endl;
	for (auto item : resultJaccardC) {
		cout << item.first << " " << item.second << endl;
	}
	cout << "dt1 = " << dt1 << ", dt2 = " << dt2 << endl;
	if (resultJaccard.size() != resultJaccardC.size()) {
		cout << "Sizes are not the same," << resultJaccard.size() << " vs " << resultJaccardC.size() << endl;
		exit(1);
	}
	for (int i = 0; i < resultJaccard.size(); ++i) {
		if (resultJaccard[i] != resultJaccardC[i]) {
			cout << "The " << i << "-th item are not the same" << endl;
		}
	}
	return 0;
}

