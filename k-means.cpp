#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <random>
#include <chrono>

#include <omp.h>

#define DATASET_PATH "dataset.txt"
#define DIMENSIONS 16

using std::cout;
using std::endl;

struct Point {
	std::vector<double> coordinates;
	int cluster;
	double min_distance;

	Point() : coordinates(DIMENSIONS), cluster(-1), min_distance(__DBL_MAX__) {}

	Point(const std::vector<double>& coordinates)
		: coordinates(coordinates), cluster(-1), min_distance(__DBL_MAX__) {}
};

void read_dataset(std::vector<Point>& p) {

	std::ifstream file(DATASET_PATH);
	if(!file.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	while(getline(file, line, '\n')) {

		std::vector<double> coordinates(DIMENSIONS);
		std::stringstream ss(line);

		for(int i = 0; i < DIMENSIONS; i++) {
			ss >> coordinates[i];
		}

		p.push_back(Point(coordinates));
	}

	file.close();
}

void write_points(std::vector<Point>& p) {

	std::ofstream file("points.txt");
	if(!file.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		exit(EXIT_FAILURE);
	}

	for(size_t i = 0; i < p.size(); i++) {

		for(int j = 0; j < DIMENSIONS; j++) {
			file << p[i].coordinates[j] << " ";
		}

		file << p[i].cluster << '\n';
	}
}

void write_centroids(std::vector<Point>& c) {

	std::ofstream file("centroids.txt");
	if(!file.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		exit(EXIT_FAILURE);
	}

	for(size_t i = 0; i < c.size(); i++) {

		for(int j = 0; j < DIMENSIONS; j++) {
			file << c[i].coordinates[j] << " ";
		}

		file << '\n';
	}
}

void get_random_centroids(std::vector<Point>& p, std::vector<Point>& c) {

	std::mt19937 gen((42)); // random fixed seed
	std::uniform_int_distribution<int> dist(0, p.size() - 1);

	for(Point& centroid : c) {
		int random_index{dist(gen)};
		centroid = p[random_index];
	}
}

double calculate_distance(Point& p, Point& c) {

	double distance{0};

	for(int i = 0; i < DIMENSIONS; i++) {
		double diff = p.coordinates[i] - c.coordinates[i];
		distance += diff * diff;
	}
	return distance;
}

void k_means(std::vector<Point>& p, std::vector<Point>& c) {

	int iterations{0};
	bool changed{true};

	while(changed) {

		changed = false;
		iterations++;

		#pragma omp parallel for reduction(||:changed)
		for(size_t i = 0; i < p.size(); i++) {

			p[i].min_distance = __DBL_MAX__;
			int prev_cluster{p[i].cluster};

			for(size_t j = 0; j < c.size(); j++) {

				double curr_distance{calculate_distance(p[i], c[j])};

				if(curr_distance < p[i].min_distance) {
					p[i].min_distance = curr_distance;
					p[i].cluster = static_cast<int>(j);
				}
			}

			if(prev_cluster != p[i].cluster)
				changed = true;
		}

		for(size_t k = 0; k < c.size(); k++) {

			std::vector<double> sums(DIMENSIONS, 0.0);
			int count{0};

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
	std::cout << "Iterations: " << iterations << '\n';
}

int main(int argc, char** argv)
{
	if(argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <K>" << std::endl;
		exit(1);
	}

	int K{std::stoi(argv[1])};

	std::vector<Point> centroids(K);
	std::vector<Point> points;

	auto start_load = std::chrono::high_resolution_clock::now();
	read_dataset(points);
	auto after_load = std::chrono::high_resolution_clock::now();

	get_random_centroids(points, centroids);

	auto start_kmeans = std::chrono::high_resolution_clock::now();
	k_means(points, centroids);
	auto after_kmeans = std::chrono::high_resolution_clock::now();

	std::cout << "Load: "
			  << std::chrono::duration<double>(after_load - start_load).count()
			  << " s\n";

	std::cout << "Kmeans: "
			  << std::chrono::duration<double>(after_kmeans - start_kmeans).count()
			  << " s\n";

	return 0;
}