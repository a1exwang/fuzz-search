#include "SimSearcher.h"
#include <fstream>
#include <iostream>

#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

using namespace std;

SimSearcher::SimSearcher() :data(new str[MAX_DATA]), max_len(0) {
}

SimSearcher::~SimSearcher() {
  delete []data;
}

int SimSearcher::createIndex(const char *filename, unsigned q) {
  this->q = q;
  struct stat statbuf;
  if (stat(filename, &statbuf) == -1) {
    exit(1);
  }
  this->filesize = (uint32_t) statbuf.st_size;

  this->fd = open(filename, 0);
  if (this->fd < 0) {
    cerr << "Cannot open this file" << endl;
    exit(2);
  }

  this->mem = nullptr;
  if (nullptr == (this->mem = (char*)mmap(nullptr, this->filesize, PROT_READ, MAP_PRIVATE, this->fd, 0))) {
    cerr << "Cannot map this file" << endl;
    exit(2);
  }

  uint32_t last_i = 0;
  this->data_count = 0;
  for (uint32_t i = 0; i < filesize; ++i) {
    if (this->mem[i] == NEW_LINE) {
      this->data[this->data_count].s = &this->mem[last_i];
      this->data[this->data_count].len = i - last_i;

      this->data_count++;
      last_i = i + 1;
    }
  }
  createEDIndex();
  createJaccardIndex();

	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double>> &result) {
	result.clear();
	return SUCCESS;
}

void SimSearcher::createEDIndex() {
  for (uint32_t i = 0; i < this->data_count; ++i) {
    if (this->data[i].len > this->max_len) {
      this->max_len = this->data[i].len;
    }

    for (int j = 0; j < this->data[i].len - this->q + 1; ++j) {
      string gram(&this->data[i].s[j], (uint32_t) this->q);
      if (this->ed_index.find(gram) == this->ed_index.end()) {
        this->ed_index[gram] = vector<uint32_t>();
      }
      this->ed_index[gram].push_back(i);
    }
  }
}

int SimSearcher::searchED(const char *_query, unsigned threshold, vector<pair<unsigned, unsigned>> &final_result) {
  final_result.clear();
  string query(_query);

  uint32_t *results = new uint32_t[this->data_count];
  memset(results, 0, sizeof(uint32_t) * this->data_count);

  for (int i = 0; i < query.size() - this->q + 1; ++i) {
    string subs(query.begin() + i, query.begin() + i + this->q);
    if (this->ed_index.find(subs) != this->ed_index.end()) {
      auto inverted_list = this->ed_index[subs];
      for (auto index : inverted_list) {
        results[index]++;
      }
    }
  }

  uint32_t t = (uint32_t)(this->max_len > query.size() ? this->max_len : query.size())
               - this->q + 1 - threshold * this->q;
  for (uint32_t i = 0; i < this->data_count; ++i) {
    if (results[i] >= t) {
      auto dist = SimSearcher::editDist(
          this->data[i].s,
          query.c_str(),
          this->data[i].len,
          (uint32_t)query.size(),
          threshold
      );
      if (dist <= threshold) {
        final_result.push_back(pair<uint32_t, uint32_t>(i, dist));
      }
    }
  }

  return SUCCESS;
}

void SimSearcher::createJaccardIndex() {

}


