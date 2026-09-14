#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <random>
#include <chrono>

#include <omp.h>

#define DATASET_PATH "dataset.txt"
#define DIMENSIONS 8
#define K 4

using namespace std;

struct Point {
	vector<double> coordinates;
	int cluster;
	double min_distance;

	Point() : coordinates(DIMENSIONS), cluster(-1), min_distance(__DBL_MAX__) {}

	Point(const vector<double>& coordinates)
		: coordinates(coordinates), cluster(-1), min_distance(__DBL_MAX__) {}
};

void read_dataset(vector<Point>& p) {

	ifstream file(DATASET_PATH);
	if(!file.is_open()) {
		cerr << "Error opening file" << endl;
		exit(EXIT_FAILURE);
	}

	string line;
	while(getline(file, line, '\n')) {
		vector<double> coordinates(DIMENSIONS);
		stringstream ss(line);

		for(int i = 0; i < DIMENSIONS; i++) {
			ss >> coordinates[i];
		}
		p.push_back(Point(coordinates));
	}
	file.close();
}

/*---------------------------------------*/
/* Unnecessary functions for OMP version */
/*---------------------------------------*/
// void write_points(vector<Point>& p) {

// 	ofstream file("points.txt");
// 	if(!file.is_open()) {
// 		cerr << "Error opening file" << endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	for(size_t i = 0; i < p.size(); i++) {
// 		for(int j = 0; j < DIMENSIONS; j++) {
// 			file << p[i].coordinates[j] << " ";
// 		}
// 		file << p[i].cluster << '\n';
// 	}
// }

// void write_centroids(vector<Point>& c) {

// 	ofstream file("centroids.txt");
// 	if(!file.is_open()) {
// 		cerr << "Error opening file" << endl;
// 		exit(EXIT_FAILURE);
// 	}

// 	for(size_t i = 0; i < c.size(); i++) {
// 		for(int j = 0; j < DIMENSIONS; j++) {
// 			file << c[i].coordinates[j] << " ";
// 		}
// 		file << '\n';
// 	}
// }

// void get_random_centroids(vector<Point>& p, vector<Point>& c) {

// 	mt19937 gen((42)); // random fixed seed
// 	uniform_int_distribution<int> dist(0, p.size() - 1);

// 	for(Point& centroid : c) {
// 		int random_index{dist(gen)};
// 		centroid = p[random_index];
// 	}
// }

/*-----------------------------------------------------------------------------------------*/
/* Since uniform_int_distribution may produce different result across environments, this   */ 
/* version uses fixed centroids to ensure that the number of iterations is always the same */
/*-----------------------------------------------------------------------------------------*/
void get_fixed_centroids(vector<Point>& c) {
    c.push_back(Point({ -68682.0,  -56833.8, -25487.4, -73152.3, -64344.7,  96992.1,  54315.7, -86193.7 }));
    c.push_back(Point({  55180.2,  -51465.5, -73974.0, -41586.0, -47131.0, -79470.5, -93493.7,  50520.6 }));
    c.push_back(Point({  68409.4,  -84103.8, -40779.3,  37496.9,  39312.1, -68692.3, -49687.9,  77242.7 }));
    c.push_back(Point({  21227.2,   34145.5, -42130.6, -51642.9,  7168.2 ,  38847.5, -5204.22, -82477.1 }));
}

/*--------------------------------------------------------------------------------------------*/
/* Calculating the Euclidean distance, but without the sqrt, it is not necessary in this case */
/*--------------------------------------------------------------------------------------------*/
double calculate_distance(Point& p, Point& c) {

	double distance{0};

	for(int i = 0; i < DIMENSIONS; i++) {
		double diff = p.coordinates[i] - c.coordinates[i];
		distance += diff * diff;
	}
	return distance;
}

/* K-means algorithm */
void k_means(vector<Point>& p, vector<Point>& c) {

    int iterations{0};
    bool changed{true};

    while(changed) {

        changed = false;
        iterations++;
		//cout << iterations << endl;

        #pragma omp parallel for reduction(||:changed) schedule(static)
        for(size_t i = 0; i < p.size(); i++) {

            p[i].min_distance = __DBL_MAX__;
            int prev_cluster{p[i].cluster};

            for(size_t j = 0; j < c.size(); j++) {

                double curr_distance{calculate_distance(p[i], c[j])};

                if(curr_distance < p[i].min_distance) {
                    p[i].min_distance = curr_distance;
                    p[i].cluster = static_cast<int>(j); // j = centroid index
                }
            }
            if(prev_cluster != p[i].cluster) changed = true;
        }

        for(size_t k = 0; k < c.size(); k++) {

			double sums[DIMENSIONS]{0};
            int count{0};

			#pragma omp parallel for reduction(+:sums[:DIMENSIONS], count) schedule(static)
            for(size_t m = 0; m < p.size(); m++) {

                if(p[m].cluster == static_cast<int>(k)) {

                    for(int d = 0; d < DIMENSIONS; d++) {
                        sums[d] += p[m].coordinates[d];
                    }
                    count++;
                }
            }

            if(count > 0) {
                for(int d = 0; d < DIMENSIONS; d++) {
                    c[k].coordinates[d] = sums[d] / count;
                }
            }
        }
    }
    cout << "N iterations: " << iterations << '\n';
}

int main(void)
{
	vector<Point> centroids;
	vector<Point> points;

	auto start_load = chrono::high_resolution_clock::now();
	read_dataset(points);
	auto after_load = chrono::high_resolution_clock::now();

	get_fixed_centroids(centroids);

	/* initials centroids print */
	// for(auto c : centroids) {
	// 	 for(auto p : c.coordinates) {
	// 		cout << p << "     ";
	// 	 }
	// 	 cout << "\n";
	// }

	auto start_kmeans = chrono::high_resolution_clock::now();
	k_means(points, centroids);
	auto after_kmeans = chrono::high_resolution_clock::now();

	cout << "Load: " << chrono::duration<double>(after_load - start_load).count() << " s\n";
	cout << "Kmeans: " << chrono::duration<double>(after_kmeans - start_kmeans).count() << " s\n";

	return 0;
}
