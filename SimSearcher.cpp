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
    return FAILURE;
  }

  this->mem = nullptr;
  if (nullptr == (this->mem = (char*)mmap(nullptr, this->filesize, PROT_READ, MAP_PRIVATE, this->fd, 0))) {
    cerr << "Cannot map this file" << endl;
    return FAILURE;
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

void SimSearcher::createEDIndex() {
  for (uint32_t i = 0; i < this->data_count; ++i) {
    if (this->data[i].len > this->max_len) {
      this->max_len = this->data[i].len;
    }

    if ((int)this->data[i].len + 1 < this->q)
      continue;
    for (int j = 0; j < (int)this->data[i].len - this->q + 1; ++j) {
      string gram(&this->data[i].s[j], (uint32_t) this->q);
      if (this->ed_index.find(gram) == this->ed_index.end()) {
        this->ed_index[gram] = vector<uint32_t>();
      }
      this->ed_index[gram].push_back(i);
    }
  }
}

void SimSearcher::createJaccardIndex() {
  this->min_len = 0;
  for (uint32_t i = 0; i < this->data_count; ++i) {
    if (this->data[i].len < this->min_len) {
      this->min_len = this->data[i].len;
    }
    int last_j = 0;
    for (int j = 0; j < (int)this->data[i].len + 1; ++j) {
      if (this->data[i].s[j] == ' ' || j == (int)this->data[i].len) {
        string word(&this->data[i].s[last_j], (uint32_t)(j - last_j));

        // insert word to inverted list
        if (this->jaccard_index.find(word) == this->jaccard_index.end()) {
          this->jaccard_index[word] = vector<uint32_t>();
        }
        this->jaccard_index[word].push_back(i);

        last_j = j + 1;
      }
    }
  }
}

int SimSearcher::searchED(const char *_query, unsigned threshold, vector<pair<unsigned, unsigned>> &final_result) {
  final_result.clear();
  string query(_query);

  uint32_t *results = new uint32_t[this->data_count];
  memset(results, 0, sizeof(uint32_t) * this->data_count);

  for (int i = 0; i < (int)query.size() - this->q + 1; ++i) {
    string subs(query.begin() + i, query.begin() + i + this->q);
    if (this->ed_index.find(subs) != this->ed_index.end()) {
      auto inverted_list = this->ed_index[subs];
      for (auto index : inverted_list) {
        results[index]++;
      }
    }
  }

  int t = (int)(this->max_len > query.size() ? this->max_len : query.size()) -
      this->q + 1 - (int)threshold * this->q;
  for (uint32_t i = 0; i < this->data_count; ++i) {
    if ((int)results[i] >= t) {
      auto dist = SimSearcher::editDist(
          this->data[i].s,
          query.c_str(),
          this->data[i].len,
          (uint32_t)query.size(),
          threshold
      );
      if (dist <= (int)threshold) {
        final_result.push_back(pair<uint32_t, uint32_t>(i, dist));
      }
    }
  }

  return SUCCESS;
}


int SimSearcher::searchJaccard(const char *_query, double threshold, vector<pair<unsigned, double>> &final_result) {
  final_result.clear();
  string query(_query);

  // calculate grams of query string
  vector<string> words;
  int last_j = 0;
  for (int j = 0; j < (int)query.size() + 1; ++j) {
    if (query[j] == ' ' || j == (int)query.size()) {
      string word(query.begin() + last_j, query.begin() + j);
      words.push_back(word);
      last_j = j + 1;
    }
  }

  // calculate
  uint32_t *results = new uint32_t[this->data_count];
  memset(results, 0, sizeof(uint32_t) * this->data_count);

  for (auto word : words) {
    if (this->jaccard_index.find(word) != this->jaccard_index.end()) {
      auto inverted_list = this->jaccard_index[word];
      for (auto index : inverted_list) {
        results[index]++;
      }
    }
  }

  double t1 = threshold * words.size();
  double t2 = (this->min_len + words.size()) * threshold / (1 + threshold);
  double t = t1 > t2 ? t1 : t2;

  for (uint32_t i = 0; i < this->data_count; ++i) {
    if (results[i] >= t) {
      auto jac = SimSearcher::jaccard(
          this->data[i].s,
          this->data[i].len,
          words
      );
      if (jac >= threshold) {
        final_result.push_back(pair<uint32_t, uint32_t>(i, jac));
      }
    }
  }

  return SUCCESS;
}
