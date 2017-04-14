#pragma once
#include <vector>
#include <map>
#include <utility>
#include <cstring>

const int SUCCESS = 0;
const int FAILURE = 1;
constexpr int MAX_DATA = 1000000;
const char NEW_LINE = '\n';

typedef struct {
	char *s;
	uint32_t len;
} str;

class SimSearcher {
  static inline int min3(int x, int y, int z) {
		if (x < y)
      return x < z ? x : z;
		else
      return y < z ? y : z;
	}
  static inline int editDist1(const char *str1 , const char *str2 , uint32_t m , uint32_t n, uint32_t tau) {
		if (m == 0) return n;
		if (n == 0) return m;
		if (str1[m - 1] == str2[n - 1])
			return editDist(str1, str2, m-1, n-1, tau);

		return 1 + min3(editDist(str1,  str2, m, n-1, tau),
										editDist(str1,  str2, m-1, n, tau),
										editDist(str1,  str2, m-1, n-1, tau)
		);
	}
	static int editDist(const char *str1 , const char *str2 , uint32_t m , uint32_t n, uint32_t tau) {

    int tmp[m+1][n+1];
		memset(tmp, 0, (m+1)*(n+1) * sizeof(int));

		int i = 0;
		int j = 0;
		while (true) {
			if (i == 0 && j == 0) {
				tmp[i][j] = 0;
			}
			else if (i == 0) {
				tmp[i][j] = tmp[i][j-1] + 1;
			}
			else if (j == 0) {
				tmp[i][j] = tmp[i-1][j] + 1;
			}
			else {
				int a = tmp[i][j - 1] + 1;
				int b = tmp[i-1][j] + 1;
				int c = tmp[i-1][j-1] + (str1[i - 1] == str2[j - 1] ? 0 : 1);
				tmp[i][j] = min3(a, b, c);
			}

			if (i == (int)m && j == (int)n) {
				break;
			}

			if (j == 0 || i == (int)m) {
				if (i + j + 1 <= (int)n) {
					j = i + j + 1;
					i = 0;
				}
        else {
					i = i + j + 1 - (int)n;
					j = (int)n;
				}
			}
      else {
				i++;
				j--;
			}
		}
    auto result = tmp[m][n];
//		printf("%d x %d\n", m+1, n+1);
//    for (int i = 0; i <= m; ++i) {
//			for (int j = 0; j <= n; ++j) {
//				printf("%d ", tmp[i][j]);
//			}
//			printf("\n");
//		}
		return result;
	}

  // s2 is sorted list
	static inline double jaccard(const char *s1, uint32_t len, const std::vector<std::string> &s2) {
		std::map<std::string, int> s1_words;
		int last_j = 0;
		for (int j = 0; j < (int)len + 1; ++j) {
			if (s1[j] == ' ' || (int)len == j) {
				std::string word(s1 + last_j, (uint32_t)(j - last_j));
				s1_words[word] = 1;
				last_j = j + 1;
			}
		}

		auto it1 = s1_words.begin();
		auto it2 = s2.begin();
		int same_count = 0;
		while (it1 != s1_words.end() && it2 != s2.end()) {
      if (it1->first == *it2) {
				same_count++;
				it1++;
				it2++;
			}
			else if (it1->first < *it2) {
				it1++;
			}
			else {
				it2++;
			}
		}

		return ((double)same_count) / (s1_words.size() + s2.size() - same_count);
	}
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
      std::vector<std::pair<unsigned, unsigned>> &final_result
  );

private:
	void createEDIndex();
	void createJaccardIndex();
private:
	int q;

  int fd;
  uint32_t filesize;
  char *mem;

  uint32_t data_count;
  str *data;

	std::map<std::string, std::vector<uint32_t>> ed_index;
	uint32_t max_len;

  std::map<std::string, std::vector<uint32_t>> jaccard_index;
  uint32_t min_len;
};

