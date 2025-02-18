#include <bitset>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <queue>

using namespace std;

/**
 * Simple program for compressing and decompressing a PPM image using the huffman tree
 */

struct Pixel {
    unsigned char r, g, b;

    Pixel() = default;

    Pixel(const unsigned char r, const unsigned char g, const unsigned char b): r(r), g(g), b(b) {}

    bool operator==(const Pixel &p) const {
        return r == p.r && g == p.g && b == p.b;
    }
};

struct HuffmanNode {
    Pixel pixel;
    int freq;
    unique_ptr<HuffmanNode> left, right;

    HuffmanNode(const Pixel pixel, const int freq): pixel(pixel), freq(freq), left(nullptr), right(nullptr) {}

    HuffmanNode(const Pixel pixel, const int freq, HuffmanNode *left, HuffmanNode *right): pixel(pixel), freq(freq),
        left(left), right(right) {}

    [[nodiscard]] bool isLeaf() const { return left == nullptr && right == nullptr; }
};

// Hash function for Pixel struct
struct PixelHash {
    size_t operator()(const Pixel &p) const {
        return hash<int>()(p.r * 256 * 256 + p.g * 256 + p.b);
    }
};

// Compare function for priority queue
struct Compare {
    bool operator()(const unique_ptr<HuffmanNode> &l, const unique_ptr<HuffmanNode> &r) {
        return l->freq > r->freq;
    }
};

void generateHuffmanCodes(const unique_ptr<HuffmanNode> &node, const string &code,
                          unordered_map<Pixel, string, PixelHash> &codeMap) {
    if (node == nullptr) return;
    if (node->left == nullptr && node->right == nullptr) {
        codeMap[node->pixel] = code;
    }
    generateHuffmanCodes(node->left, code + "0", codeMap);
    generateHuffmanCodes(node->right, code + "1", codeMap);
}

unordered_map<Pixel, string, PixelHash> generateHuffmanCodes(const unique_ptr<HuffmanNode> &node, const string &code) {
    unordered_map<Pixel, string, PixelHash> codeMap;
    generateHuffmanCodes(node, code, codeMap);
    return codeMap;
}

unique_ptr<HuffmanNode> deserializeTree(ifstream &inputFile) {
    const char nodeType = inputFile.get();

    if (inputFile.eof()) { return nullptr; }

    if (nodeType) {
        Pixel p{};

        inputFile.read(reinterpret_cast<char *>(&p), sizeof(Pixel));
        return make_unique<HuffmanNode>(p, 0);
    }

    Pixel p{};
    auto root = make_unique<HuffmanNode>(p, 0);
    root->left = deserializeTree(inputFile);
    root->right = deserializeTree(inputFile);
    return root;
}

vector<Pixel> decompress(const unique_ptr<HuffmanNode> &root, const vector<bitset<8> > &bitset_seq) {
    const unique_ptr<HuffmanNode> *node_ptr = &root;
    vector<Pixel> image;

    for (auto byte: bitset_seq) {
        for (int i = 7; i >= 0; --i) {
            node_ptr = byte[i] == 0 ? &(node_ptr->get()->left) : &(node_ptr->get()->right);
            if (node_ptr->get()->isLeaf()) {
                image.push_back(node_ptr->get()->pixel);
                node_ptr = &root;
            }
        }
    }
    return image;
}

void decompress(const string &inputPath, const string &outputPath) {
    ifstream inputFile(inputPath, ios::binary);
    int height, width;
    vector<bitset<8> > bitset_seq;

    inputFile.read(reinterpret_cast<char *>(&height), sizeof(height));
    inputFile.read(reinterpret_cast<char *>(&width), sizeof(width));
    const auto root = deserializeTree(inputFile);
    char byte;
    while (inputFile.get(byte)) {
        bitset_seq.emplace_back(byte);
    }
    inputFile.close();

    auto image = decompress(root, bitset_seq);
    while (image.size() > height * width) { image.pop_back(); }

    ofstream ofs(outputPath, ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    ofs.write(reinterpret_cast<char *>(image.data()), image.size() * sizeof(Pixel));
    ofs.close();
}

void serializeTree(ofstream &os, const unique_ptr<HuffmanNode> &node) {
    if (node == nullptr) return;

    if (node->isLeaf()) {
        os.put(1);
        os.write(reinterpret_cast<char *>(&node->pixel), sizeof(Pixel));
        return;
    }

    os.put(0);
    serializeTree(os, node->left);
    serializeTree(os, node->right);
}

unique_ptr<HuffmanNode> buildHuffmanTree(const vector<Pixel> &image) {
    unordered_map<Pixel, int, PixelHash> huffmanMap;

    for (const auto &pixel: image) { huffmanMap[pixel]++; }

    priority_queue<unique_ptr<HuffmanNode>, vector<unique_ptr<HuffmanNode> >, Compare> nodes;
    for (const auto &[pixel, freq]: huffmanMap) { nodes.push(make_unique<HuffmanNode>(pixel, freq)); };
    while (nodes.size() > 1) {
        auto left = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
        nodes.pop();
        auto right = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
        nodes.pop();
        auto node = make_unique<HuffmanNode>(Pixel{0, 0, 0}, left->freq + right->freq);
        node->left = move(left);
        node->right = move(right);
        nodes.push(move(node));
    }

    auto root = move(const_cast<unique_ptr<HuffmanNode> &>(nodes.top()));
    nodes.pop();

    return root;
}

string generateBitString(const vector<Pixel> &image, unordered_map<Pixel, string, PixelHash> codeMap) {
    string bitString;

    for (auto pixel: image) { bitString += codeMap[pixel]; }
    while (bitString.size() % 8) { bitString += '0'; }

    return bitString;
}

void compress(const vector<Pixel> &image, int height, int width, const string &outputPath) {
    const unique_ptr<HuffmanNode> root = buildHuffmanTree(image);
    const unordered_map<Pixel, string, PixelHash> codeMap = generateHuffmanCodes(root, "");
    const string bitString = generateBitString(image, codeMap);

    ofstream os(outputPath, ios::binary);

    os.write(reinterpret_cast<char *>(&height), sizeof(height));
    os.write(reinterpret_cast<char *>(&width), sizeof(width));
    serializeTree(os, root);
    for (int i = 0; i < bitString.size(); i += 8) {
        os.put(static_cast<char>(bitset<8>(bitString.substr(i, 8)).to_ulong()));
    }
    os.close();
}

int main() {
    vector<Pixel> image;

    for (int i = 0; i < 30000; i++) image.emplace_back(255, 0, 0);
    for (int i = 0; i < 30000; i++) image.emplace_back(255, 255, 0);
    for (int i = 0; i < 30000; i++) image.emplace_back(255, 255, 255);
    compress(image, 300, 300, "huffman.bin");
    decompress("huffman.bin", "image.ppm");
    return 0;
}
