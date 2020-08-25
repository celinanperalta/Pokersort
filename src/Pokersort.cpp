using namespace std;

#include <iostream>
#include <cassert>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <map>
#include <random>
#include <set>

typedef pair<int_fast32_t, int_fast32_t> P;
typedef vector<P> VP;
typedef vector<int_fast32_t> VI;
typedef unordered_map<int_fast32_t, int_fast32_t> u_map;

random_device seed;
mt19937 gen;

template<class T>

void swap(T *x, T *y) {
	T temp = *x;
	*x = *y;
	*y = temp;
}

/*
 * testing things
 */

void insertion_sort(VI& a) {
	int i, j;
	i = 0;
	while (i != 5) {
		j = i;
		while (j != 0 && a[j - 1] > a[j]) {
			swap(&a[j - 1], &a[j]);
			--j;
		}
		++i;
	}
}

int encode_hand(VI h) {
	return h[0] + h[1] * 52 + h[2] * 52 * 52 + h[3] * 52 * 52 * 52
			+ h[4] * 52 * 52 * 52 * 52;
}

//encoded int input -> sorted V2D {rank, suit}
VP decode_hand(const int& b) {
	int h = b;

	VP a(5);
	int i, c;
	for (i = 0; i != 5; ++i) {
		c = h % 52;
		a[i].first = c / 4;
		a[i].second = c % 4;
		h /= 52;
	}

	int j;
	i = 0;
	while (i != 5) {
		j = i;
		while (j != 0 && a[j - 1].first > a[j].first) {
			swap(&a[j - 1], &a[j]);
			--j;
		}
		++i;
	}

	return a;
}

VI create_hands(int n) {
	VI h(n);
	u_map m;
	for (int i = 0; i != n; ++i) {
		VI t(5);
		for (int j = 0; j != 5; ++j) {
			uniform_int_distribution<int> u(0, 51);
			int p = u(gen);
			while (m[p] != 0)
				p = u(gen);
			t[j] = p;
			m[p] = 100;
		}
		h[i] = encode_hand(t);
		m.clear();
		t.clear();
	}
	return h;
}

void print_hand(VP h) {
	for (int i = 0; i != 5; ++i) {
		cout << "(" << h[i].first << "," << h[i].second << ")";
	}
	cout << " ";
}

/*
 * checking hand type
 */

int_fast32_t rank_hand(int_fast32_t h) {

	unsigned int bits = 0, flush = (1 << ((h % 52) & 3));
	int_fast32_t i;
	bool pair = 0;
	vector<int_fast32_t> r(13);

	for (i = 0; i != 5; ++i) {
		int_fast32_t tmp = (h % 52) >> 2;
		++r[tmp];
		pair |= r[tmp] > 1;
		flush &= 1 << ((h % 52) & 3);
		bits |= 1 << tmp;
		h /= 52;
	}

	if ((bits / (bits & -bits) == 31) || (bits == 0x100F)) {
		return flush ? (bits == 0x100F) ? 8000000 : 8000000 + bits:
		(bits == 0x100F) ? 4000000 : 4000000 + bits;
	} else if (flush) {
		return 5000000 + bits;
	} else if (!pair) {
		return bits;
	} else {

		int_fast32_t i = 0, p1 = 0, p2 = 0, t = -1, hc = -1;

		for (; i != 13; ++i) {
			if (!r[i])
				continue;
			switch (r[i]) {
			case 1:
				if (i > hc)
					hc = i;
				break;
			case 2:
				if (p1)
					p2 = i + 1;
				else
					p1 = i + 1;
				break;
			case 3:
				t = i;
				if (p1)
					return 6000000 + t * 10000 + p1;
				break;
			case 4:
				return 7000000 + i * 10000 + bits;
			}
		}

		if (t != -1)
			return (p1 ^ p2) ?
					6000000 + t * 10000 + p1 :
					3000000 + t * 10000 + hc * 10 + bits;

		if (p1) {
			if (p2)
				return 2000000
						+ (p1 > p2 ?
								p1 * 10000 + p2 * 100 : p1 * 100 + p2 * 10000)
						+ hc;
			return 1000000 + p1 * 10000 + bits;
		}
	}

	return bits;
}

/*
 * quicksort/insertion sort
 */

void insertion_sort(VP& a, const int& l, const int& r) {
	int i = l + 1, j;
	for (; i != r + 1; ++i) {
		j = i;
		while (j != l && a[j - 1] > a[i]) {
			a[j] = a[j - 1];
			--j;
		}
		a[j] = a[i];
	}
}

int partition(VP& a, const int& l, const int& r) {

	const P pivot = a[r];
	int p_ind = l, j = l;
	for (; j != r; ++j) {
		if (a[j] <= pivot) {
			swap(&a[p_ind], &a[j]);
			++p_ind;
		}
	}
	swap(&a[p_ind], &a[r]);
	return p_ind;
}

void quick_sort(VP& a, int l, int r) {
	for (; l < r;) {
		if (r - l < 16) {
			insertion_sort(a, l, r);
			break;
		} else {
			int p = partition(a, l, r);
			if (p - l < r - p) {
				quick_sort(a, l, p - 1);
				l = p + 1;
			} else {
				quick_sort(a, p + 1, r);
				r = p - 1;
			}
		}
	}
}

void poker_sort(VI& b) {
	int i = 0, n = b.size();
	VP a(n);

	for (; i != n; ++i) {
		a[i] = make_pair(rank_hand(b[i]), b[i]);
	}

	quick_sort(a, 0, n - 1);

	for (i = 0; i != n; ++i) {
		b[i] = a[i].second;
	}
}


int main() {
 	ifstream in("/Users/celinaperalta/School/DataStructures/Pokersort/in.txt");
 	int m, n, rr, v;
 	in >> m;
 	for (int mm = 0; mm < m; mm++) {
 		in >> n;
 		cout << n << " elements... ";
 		vector<int> a(n);
 		map<int, int> r;
 		for (int i = 0; i < n; i++) {
 			in >> a[i] >> rr;
 			r[a[i]] = rr;
 		}
 		clock_t start = std::clock();
 		poker_sort(a);
 		cout << "\t" << (std::clock() - start) / (double) CLOCKS_PER_SEC
 				<< " seconds\n";
 		cout << "\tvalidating...\t";
 		for (int i = 0; i < n - 1; i++)
 			if (r[a[i]] > r[a[i + 1]]) {
 				cout << "r[a[i]]" << r[a[i]] << "\n";
 				cout << "r[a[i + 1]]" << r[a[i + 1]] << "\n";
 				cout << "\n\thand " << a[i] << " = (";
 				v = a[i];
 				for (int j = 0; j < 5; j++) {
 					if (j)
 						cout << "\t";
 					cout << (v % 52) / 4 << " "
 							<< ((v % 4) == 0 ? 'A' : (v % 4) == 1 ? 'B' :
 								(v % 4) == 2 ? 'C' : 'D');
 					v /= 52;
 				}
 				cout << ") is better than hand " << a[i + 1] << " = (";
 				v = a[i + 1];
 				for (int j = 0; j < 5; j++) {
 					if (j)
 						cout << "\t";
 					cout << (v % 52) / 4 << " "
 							<< ((v % 4) == 0 ? 'A' : (v % 4) == 1 ? 'B' :
 								(v % 4) == 2 ? 'C' : 'D');
 					v /= 52;
 				}
 				cout
 						<< ") but you put these in the sorted array in the opposite order\n";
 				return -1;
 			}
 		cout << "valid!\n";
 	}
}

