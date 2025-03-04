#include <bitset>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>
#include "dct.cpp"
#include "huffmannode.h"
#include "pixel.h"
#include "quantize.cpp"

using namespace std;

/**
 * JPEG style image compressor using Huffman Compression, Quantization, and
 * Discrete Cosine Transform
 */

enum class ColorChannel { RED, GREEN, BLUE };

void generateHuffmanCodes(const unique_ptr<HuffmanNode> &node, const string &code,
                          unordered_map<int, string> &codeMap) {
    if (node == nullptr) return;
    if (node->left == nullptr && node->right == nullptr) {
        codeMap[node->channelValue] = code;
    }
    generateHuffmanCodes(node->left, code + "0", codeMap);
    generateHuffmanCodes(node->right, code + "1", codeMap);
}

unordered_map<int, string> generateHuffmanCodes(const unique_ptr<HuffmanNode> &node, const string &code) {
    unordered_map<int, string> codeMap;
    generateHuffmanCodes(node, code, codeMap);
    return codeMap;
}

unique_ptr<HuffmanNode> deserializeTree(ifstream &inputFile) {
    const auto nodeType = inputFile.get();

    if (inputFile.eof()) { return nullptr; }

    if (nodeType) {
        int p;

        inputFile.read(reinterpret_cast<char *>(&p), sizeof(int));
        return make_unique<HuffmanNode>(p, 0);
    }

    int p;
    auto root = make_unique<HuffmanNode>(p, 0);
    root->left = deserializeTree(inputFile);
    root->right = deserializeTree(inputFile);
    return root;
}

vector<int> decompress(const unique_ptr<HuffmanNode> &root, const vector<bitset<8> > &bitset_seq) {
    const unique_ptr<HuffmanNode> *node_ptr = &root;
    vector<int> image;

    for (auto byte: bitset_seq) {
        for (int i = 7; i >= 0; --i) {
            node_ptr = byte[i] == 0 ? &(node_ptr->get()->left) : &(node_ptr->get()->right);
            if (node_ptr->get()->isLeaf()) {
                image.push_back(node_ptr->get()->channelValue);
                node_ptr = &root;
            }
        }
    }
    return image;
}

vector<vector<int> > deserializeChannel(ifstream &inputFile, const size_t height, const size_t width) {
    vector<bitset<8> > bitset_seq;
    const auto root = deserializeTree(inputFile);
    char byte;
    size_t bitStringLength = 0;

    inputFile.read(reinterpret_cast<char *>(&bitStringLength), sizeof(bitStringLength));
    while (inputFile.get(byte) && bitStringLength--) {
        bitset_seq.emplace_back(byte);
    }

    vector<int> values = decompress(root, bitset_seq);
    vector image(height, vector<int>(width));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            image[i][j] = values[i * width + j];
        }
    }
    while (values.size() > height * width) { values.pop_back(); }

    return image;
}

void decompress(const string &inputPath, const string &outputPath) {
    ifstream inputFile(inputPath, ios::binary);
    size_t height, width;

    inputFile.read(reinterpret_cast<char *>(&height), sizeof(height));
    inputFile.read(reinterpret_cast<char *>(&width), sizeof(width));
    vector<vector<int> > channel[3] = {
        deserializeChannel(inputFile, height, width),
        deserializeChannel(inputFile, height, width),
        deserializeChannel(inputFile, height, width)
    };

    inputFile.close();

    vector<vector<int> > dequantized[3] = {dequantize(channel[0]), dequantize(channel[1]), dequantize(channel[2])};

    vector<vector<unsigned char> > inverseDCTMatrices[3] = {
        fullInverseDCT(dequantized[0]),
        fullInverseDCT(dequantized[1]),
        fullInverseDCT(dequantized[2])
    };

    vector image(height, vector<Pixel>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            image[i][j] = Pixel(inverseDCTMatrices[0][i][j], inverseDCTMatrices[1][i][j], inverseDCTMatrices[2][i][j]);
        }
    }

    ofstream ofs(outputPath, ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (auto &v: image) {
        for (auto &p: v) {
            ofs.write(reinterpret_cast<char *>(&p), sizeof(Pixel));
        }
    }
    ofs.flush();
    ofs.close();
}

void serializeTree(ofstream &os, const unique_ptr<HuffmanNode> &node) {
    if (node->isLeaf()) {
        os.put(1);
        os.write(reinterpret_cast<char *>(&node->channelValue), sizeof(int));
        return;
    }

    os.put(0);
    serializeTree(os, node->left);
    serializeTree(os, node->right);
}

unique_ptr<HuffmanNode> buildHuffmanTree(const vector<vector<int> > &image) {
    unordered_map<int, int> huffmanMap;

    for (const auto &v: image) {
        for (const auto &pixel: v) {
            huffmanMap[pixel]++;
        }
    }

    priority_queue<unique_ptr<HuffmanNode>, vector<unique_ptr<HuffmanNode> >, Compare> nodes;
    for (const auto &[pixel, freq]: huffmanMap) {
        nodes.push(make_unique<HuffmanNode>(pixel, freq));
    };

    // Handle trivial case
    if (nodes.size() == 1) {
        auto root = make_unique<HuffmanNode>(0, nodes.top().get()->freq);
        root->left = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
        root->right = make_unique<HuffmanNode>(0, 0);
        nodes.pop();
        return root;
    }

    while (nodes.size() > 1) {
        auto left = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
        nodes.pop();
        auto right = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
        nodes.pop();
        auto node = make_unique<HuffmanNode>(0, left->freq + right->freq);
        node->left = move(left);
        node->right = move(right);
        nodes.push(move(node));
    }

    auto root = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
    nodes.pop();

    return root;
}

string generateBitString(const vector<vector<int> > &image, unordered_map<int, string> codeMap) {
    string bitString;

    for (const auto &v: image) {
        for (const auto &pixel: v) { bitString += codeMap[pixel]; }
    }
    while (bitString.size() % 8) { bitString += '0'; }

    return bitString;
}

void compressChannel(const vector<vector<int> > &channel, ofstream &os) {
    const unique_ptr<HuffmanNode> root = buildHuffmanTree(channel);
    const unordered_map<int, string> codeMap = generateHuffmanCodes(root, "");
    const string bitString = generateBitString(channel, codeMap);
    serializeTree(os, root);

    size_t bitStringLength = bitString.size() / 8;
    os.write(reinterpret_cast<char *>(&bitStringLength), sizeof(bitStringLength));
    for (int i = 0; i < bitString.size(); i += 8) {
        os.put(static_cast<char>(bitset<8>(bitString.substr(i, 8)).to_ulong()));
    }
    os.put('\n');
}

vector<vector<double> > extractChannel(const vector<vector<Pixel> > &imageMatrix, ColorChannel channel) {
    const size_t N = imageMatrix.size();
    const size_t M = imageMatrix[0].size();
    vector channelMatrix(N, vector<double>(M, 0));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            switch (channel) {
                case ColorChannel::RED:
                    channelMatrix[i][j] = imageMatrix[i][j].r;
                    break;
                case ColorChannel::GREEN:
                    channelMatrix[i][j] = imageMatrix[i][j].g;
                    break;
                case ColorChannel::BLUE:
                    channelMatrix[i][j] = imageMatrix[i][j].b;
                    break;
            }
        }
    }

    return channelMatrix;
}

void compress(vector<vector<Pixel> > image2d, const string &outputPath) {
    size_t height = image2d.size();
    size_t width = image2d[0].size();
    ofstream os(outputPath, ios::binary);

    os.write(reinterpret_cast<char *>(&height), sizeof(height));
    os.write(reinterpret_cast<char *>(&width), sizeof(width));

    const vector<vector<double> > channelMatrices[3] = {
        extractChannel(image2d, ColorChannel::RED),
        extractChannel(image2d, ColorChannel::GREEN),
        extractChannel(image2d, ColorChannel::BLUE)
    };

    const vector<vector<double> > dctChannels[3] = {
        fullDCT(channelMatrices[0]), fullDCT(channelMatrices[1]), fullDCT(channelMatrices[2])
    };

    const vector<vector<int> > quantizedChannels[3] = {
        quantize(dctChannels[0]), quantize(dctChannels[1]), quantize(dctChannels[2])
    };

    compressChannel(quantizedChannels[0], os);
    compressChannel(quantizedChannels[1], os);
    compressChannel(quantizedChannels[2], os);
    os.close();
}

// Function to read PPM width, height, and pixel data
bool loadPPM(const string &filename, vector<vector<Pixel> > &imageMatrix) {
    ifstream file(filename, ios::binary);
    int height, width;

    if (!file) {
        cerr << "Error: Unable to open file " << filename << endl;
        return false;
    }

    string format;
    file >> format; // Read "P6"
    if (format != "P6") {
        cerr << "Error: Not a valid P6 PPM file!" << endl;
        return false;
    }

    file >> width >> height; // Read width and height

    int maxColorValue;
    file >> maxColorValue; // Read max color value (should be 255)
    file.ignore(); // Ignore the newline after max color value

    // Resize the image matrix
    imageMatrix.resize(height, vector<Pixel>(width));

    // Read pixel data (binary format)
    for (int i = 0; i < height; i++) {
        file.read(reinterpret_cast<char *>(imageMatrix[i].data()), width * sizeof(Pixel));
    }

    file.close();
    return true;
}

int main() {
    vector<vector<Pixel> > imageMatrix;

    // Use a sample ppm file for testing
    loadPPM("test.ppm", imageMatrix);
    compress(imageMatrix, "huffman.bin");
    decompress("huffman.bin", "huffman.ppm");

    return 0;
}
