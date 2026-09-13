#include <iostream>
#include <random>
#include <fstream>

int main(int argc, char** argv) {

    if (argc != 5) {
        std::cerr <<
        "Usage: " << argv[0]
        << " <lines> <lowest_num> <biggest_num> <dimensions>\n\n"
        << "<lines>        - Number of points to generate.\n"
        << "<lowest_num>   - Minimum coordinate value.\n"
        << "<biggest_num>  - Maximum coordinate value.\n"
        << "<dimensions>   - Number of dimensions.\n";

        return 1;
    }

    int LINES       {std::stoi(argv[1])};
    int LOWEST_NUM  {std::stoi(argv[2])};
    int BIGGEST_NUM {std::stoi(argv[3])};
    int DIMENSIONS  {std::stoi(argv[4])};

    std::ofstream file("dataset.txt");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(LOWEST_NUM, BIGGEST_NUM);

    for (int i = 0; i < LINES; i++) {

        for (int j = 0; j < DIMENSIONS; j++) {
            double coordinate = dist(gen);

            file << coordinate;

            if (j < DIMENSIONS - 1)
                file << " ";
        }
        file << '\n';
    }
    return 0;
}