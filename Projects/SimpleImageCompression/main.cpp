#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

/**
 * Simple image compression-decompression program using run-length-encoding
 */
struct Pixel {
    unsigned char r, g, b;

    bool operator==(const Pixel &value) const {
        return r == value.r && g == value.g && b == value.b;
    }
};

void compress(const vector<Pixel> &image, int width, int height, const string &filename) {
    int count = 0;
    ofstream output(filename.c_str(), ios::out | ios::binary);

    output.write(reinterpret_cast<const char *>(&width), sizeof(width));
    output.write(reinterpret_cast<const char *>(&height), sizeof(height));

    for (int i = 0; i < image.size(); i++) {
        count = 1;
        while (i + count < image.size() && count < 255 && image[i] == image[i + count]) {
            count++;
        }
        output.put(static_cast<char>(count));
        output.write(reinterpret_cast<const char *>(&image[i]), sizeof(Pixel));
        i += count;
    }
    output.close();
}

void decompress(const string &inputPath, const string &outputPath) {
    ifstream input(inputPath.c_str(), ios::in | ios::binary);

    if (!input) {
        cerr << "Error opening file for reading!" << endl;
        return;
    }

    int width, height;
    input.read(reinterpret_cast<char *>(&width), sizeof(width));
    input.read(reinterpret_cast<char *>(&height), sizeof(height));

    vector<Pixel> image;

    while (!input.eof()) {
        char count = 0;
        Pixel pixel{};

        input.get(count);
        input.read(reinterpret_cast<char *>(&pixel), sizeof(pixel));
        for (int i = 0; i < static_cast<unsigned char>(count); i++) {
            image.push_back(pixel);
        }
    }

    input.close();

    ofstream output(outputPath.c_str(), ios::out | ios::binary);
    output << "P6\n" << width << " " << height << "\n255\n"; // PPM header
    output.write(reinterpret_cast<const char *>(image.data()), image.size() * sizeof(Pixel));
    output.close();
}

int main() {
    // Example: Creating a simple test image (100x100 solid color)
    int width = 100, height = 100;
    vector<Pixel> image(width * height, {255, 100, 0}); // Solid Red

    compress(image, width, height, "compressed.rle");
    decompress("compressed.rle", "output.ppm");
    cout << "Compressed image!" << endl;
    return 0;
}
