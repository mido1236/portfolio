#include <cmath>
#include <vector>
#include "common.h"

using namespace std;

const int JPEG_QM[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

vector<vector<int> > quantize(const vector<vector<double> > &dctMatrix) {
    vector result(dctMatrix.size(), vector(dctMatrix[0].size(), 0));

    for (int u = 0; u < dctMatrix.size(); u++) {
        for (int v = 0; v < dctMatrix[0].size(); v++) {
            result[u][v] = static_cast<int>(round(dctMatrix[u][v] / JPEG_QM[u % 8][v % 8]));
        }
    }

    return result;
}

vector<vector<int> > dequantize(const vector<vector<int> > &quantized) {
    vector result(quantized.size(), vector(quantized[0].size(), 0));

    for (int u = 0; u < quantized.size(); u++) {
        for (int v = 0; v < quantized[0].size(); v++) {
            result[u][v] = quantized[u][v] * JPEG_QM[u % 8][v % 8];
        }
    }

    return result;
}
