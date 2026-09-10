#include <iostream>
#include <random>
#include <fstream>

int main(int argc, char** argv) {

    if(argc != 4) {
        std::cerr << 
        "Usage: " << argv[0] << " <lines> <lowest_num> <biggest_num>\n\n" <<
        "<lines>        - Number of coordinates to generate.          \n" <<
        "<lowest_num>   - Minimum value that a coordinate can have.   \n" <<
        "<biggest_num>  - Maximum value that a coordinate can have.   \n" <<
        std::endl;

        return 1;
    }

    int LINES      {std::stoi(argv[1])};
    int LOWEST_NUM {std::stoi(argv[2])};
    int BIGGEST_NUM{std::stoi(argv[3])};

    std::ofstream file("dataset.txt");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(LOWEST_NUM, BIGGEST_NUM);

    for (int i = 0; i < LINES; i++) {
        double x{dist(gen)};
        double y{dist(gen)};
        double z{dist(gen)};

        file << x << " " << y << " " << z << '\n';
    }

    file.close();

    return 0;
}