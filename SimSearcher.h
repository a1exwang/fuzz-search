#pragma once
#include <vector>
#include <map>
#include <utility>

const int SUCCESS = 0;
const int FAILURE = 1;
constexpr int MAX_DATA = 1000000;
const char NEW_LINE = '\n';

class SimSearcher {
public:
	SimSearcher();
	~SimSearcher();

	int createIndex(const char *filename, unsigned q);
	int searchJaccard(
      const char *query, 
      double threshold, 
      std::vector<std::pair<unsigned, double>> &result
  );
	int searchED(
      const char *query, 
      unsigned threshold, 
      std::vector<std::pair<unsigned, unsigned>> &result
  );
  int fd;
  uint32_t filesize;
  char *mem;

  int data_count;
  char *data[MAX_DATA];
  uint32_t data_lengths[MAX_DATA];
};

