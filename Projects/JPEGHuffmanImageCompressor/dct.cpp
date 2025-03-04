#include <cmath>
#include <vector>
#include "common.h"

using namespace std;

double C(const int u) {
    return (u == 0) ? (1.0 / sqrt(2)) : 1.0;
}

vector<vector<double> > dct(const vector<vector<double> > &matrix) {
    vector result(N, vector<double>(N, 0));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                for (int l = 0; l < N; l++) {
                    sum += matrix[k][l] * cos((2 * k + 1) * i * PI / (2 * N)) * cos((2 * l + 1) * j * PI / (2 * N));
                }
            }
            result[i][j] = 0.25 * C(i) * C(j) * sum;
        }
    }

    return result;
}

vector<vector<double> > inverseDct(const vector<vector<double> > &matrix) {
    vector result(N, vector<double>(N, 0));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < N; k++) {
                for (int l = 0; l < N; l++) {
                    sum += C(k) * C(l) * matrix[k][l] * cos((2 * i + 1) * k * PI / (2 * N)) * cos(
                        (2 * j + 1) * l * PI / (2 * N));
                }
            }
            result[i][j] = 0.25 * sum;
        }
    }

    return result;
}

vector<vector<double> > fullDCT(const vector<vector<double> > &matrix) {
    const size_t height = matrix.size();
    const size_t width = matrix[0].size();
    vector result(height, vector<double>(width, 0));

    for (int i = 0; i < height; i += N) {
        for (int j = 0; j < width; j += N) {
            vector imageBlock(N, vector<double>(N, 0));

            for (int k = 0; k < N && i + k < height; k++) {
                for (int l = 0; l < N && j + l < width; l++) {
                    imageBlock[k][l] = matrix[i + k][j + l];
                }
            }

            auto dctBlock = dct(imageBlock);
            for (int k = 0; k < N && i + k < height; k++) {
                for (int l = 0; l < N && j + l < width; l++) {
                    result[i + k][j + l] = dctBlock[k][l];
                }
            }
        }
    }

    return result;
}

vector<vector<unsigned char> > fullInverseDCT(const vector<vector<int> > &matrix) {
    const size_t height = matrix.size();
    const size_t width = matrix[0].size();
    vector result(height, vector<unsigned char>(width, 0));

    for (int i = 0; i < height; i += N) {
        for (int j = 0; j < width; j += N) {
            vector imageBlock(N, vector<double>(N, 0));

            for (int k = 0; k < N && i + k < height; k++) {
                for (int l = 0; l < N && j + l < width; l++) {
                    imageBlock[k][l] = matrix[i + k][j + l];
                }
            }

            auto inverseDctBlock = inverseDct(imageBlock);
            for (int k = 0; k < N && i + k < height; k++) {
                for (int l = 0; l < N && j + l < width; l++) {
                    result[i + k][j + l] = static_cast<char>(inverseDctBlock[k][l]);
                }
            }
        }
    }

    return result;
}
