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

SimSearcher::SimSearcher() {
}

SimSearcher::~SimSearcher() {
}

int SimSearcher::createIndex(const char *filename, unsigned q) {
  struct stat statbuf;
  if (stat(filename, &statbuf) == -1) {
    exit(1);
  }
  this->filesize = statbuf.st_size;

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
  for (uint32_t i = 0; i < filesize; ++i) {
    if (this->mem[i] == NEW_LINE) {
      this->data[this->data_count] = &this->mem[last_i];
      this->data_lengths[this->data_count] = i - last_i;

      this->data_count++;
      last_i = i + 1;
    }
  }
  
	return SUCCESS;
}

int SimSearcher::searchJaccard(const char *query, double threshold, vector<pair<unsigned, double>> &result) {
	result.clear();
	return SUCCESS;
}

int SimSearcher::searchED(const char *query, unsigned threshold, vector<pair<unsigned, unsigned>> &result) {
  result.clear();

  for (int i = 0; i < this->data_count; ++i) {
    if (strcmp(this->data[i], query) == 0) {
      result.push_back(pair<uint32_t, uint32_t>(i, 0));
    }
  }
  return SUCCESS;
}

