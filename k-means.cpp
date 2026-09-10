#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <random>
#include <chrono>

#define DATASET_PATH "dataset.txt"
 
using std::cout;
using std::endl;

struct Point {
	double x, y, z;
	int cluster;
	double min_distance;

	Point() = default;

	Point(double x, double y, double z) : x(x), y(y), z(z), cluster(-1), min_distance(__DBL_MAX__) {} 
};

void read_dataset(std::vector<Point>& p) {

	std::ifstream file(DATASET_PATH);
	if(!file.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::string line;
	while(getline(file, line, '\n')) {

		double x{}, y{}, z{};
		std::stringstream ss(line);
		
		ss >> x >> y >> z;
		p.push_back(Point(x, y, z));
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
		file << p[i].x << " " << p[i].y << " " << p[i].z << " " << p[i].cluster << '\n';
	}
}

void write_centroids(std::vector<Point>& c) {

	std::ofstream file("centroids.txt");
	if(!file.is_open()) {
		std::cerr << "Error opening file" << std::endl;
		exit(EXIT_FAILURE);
	}

	for(size_t i = 0; i < c.size(); i++) {
		file << c[i].x << " " << c[i].y << " " << c[i].z << '\n';
	}
}

void get_random_centroids(std::vector<Point>& p, std::vector<Point>& c) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(0, p.size() - 1);  // random number from 0 until points vector size (-1 because the index)
	            
	for(Point& centroid : c) {
		int random_index{dist(gen)}; // get a random index from all points vector  
		centroid = p[random_index];
	}
}

double calculate_distance(Point& p, Point& c) {
	return std::sqrt( (p.x - c.x) * (p.x - c.x) + (p.y - c.y) * (p.y - c.y) + (p.z - c.z) * (p.z - c.z) );
}

void k_means(std::vector<Point>& p, std::vector<Point>& c) {

	int iterations{0};
	bool changed{true};

	while(changed) {

		changed = false;
		iterations++;

		for(size_t i = 0; i < p.size(); i++) {
			p[i].min_distance = __DBL_MAX__;
			int prev_cluster{p[i].cluster};

			for(size_t j = 0; j < c.size(); j++) {
				double curr_distance{calculate_distance(p[i], c[j])};

				if(curr_distance < p[i].min_distance) {
					p[i].min_distance = curr_distance;
					p[i].cluster      = static_cast<int>(j); // centroid index
				}
			}
			if(prev_cluster != p[i].cluster) changed = true;
		}

		for(size_t k = 0; k < c.size(); k++) {
			double x_sum{0};
			double y_sum{0};
			double z_sum{0};
			int    count{0};

			for(size_t m = 0; m < p.size(); m++) {
				if(p[m].cluster == static_cast<int>(k)) {
					x_sum += p[m].x;
					y_sum += p[m].y;
					z_sum += p[m].z;
					count++;
				}
			}
			if(count > 0) {
				double x_mean{x_sum / count};
				double y_mean{y_sum / count};
				double z_mean{z_sum / count};
				c[k] = {x_mean, y_mean, z_mean};	
			}	
		}
	}
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

	write_points(points);
	write_centroids(centroids);

	std::cout << "Load: " << std::chrono::duration<double>(after_load - start_load).count() << " s\n";

	std::cout << "Kmeans: " << std::chrono::duration<double>(after_kmeans - start_kmeans).count() << " s\n";

	return 0;
}

