//
// Created by ahmed on 2025-09-18.
//

#ifndef EULER_POKER_H
#define EULER_POKER_H
#include <cassert>

unordered_map<char, int> valueMap = {
    {'2', 2}, {'3', 3}, {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7}, {'8', 8}, {'9', 9}, {'T', 10}, {'J', 11}, {'Q', 12},
    {'K', 13}, {'A', 14}
};

unordered_map<char, int> suitMap = {
    {'C', 1}, {'H', 2}, {'D', 3}, {'S', 4}
};

struct HandRank {
    int rank = 0;
    vector<int> tiebreakers;
};

bool is_flush(const vector<char> &suits) {
    return ranges::all_of(suits, [&](const char suit) {
        if (suit != suits[0]) return false;
        return true;
    });
};

bool is_straight(vector<int> values) {
    ranges::sort(values);

    // if (values == vector{2, 3, 4, 5, 14})
    //     return true;

    return ranges::adjacent_find(values, [&](const int a, const int b) { return b != a + 1; }) == values.end();
}

bool has_count(unordered_map<int, int> freq, int count) {
    return ranges::any_of(freq | views::values, [&](auto &c) { return c == count; });
}

bool has_two_pairs(unordered_map<int, int> freq) {
    int ret = 0;
    for (const auto value: freq | views::values) { if (value == 2) ret++; }
    return ret == 2;
}

vector<int> get_distinct_values_ordered(unordered_map<int, int> freq) {
    auto elements_view = freq | views::keys;
    auto values = vector(elements_view.begin(), elements_view.end());

    ranges::sort(values, [&](const int a, const int b) {
        if (freq[a] == freq[b]) return a > b;
        return freq[a] > freq[b];
    });

    return values;
}

HandRank evaluateHand(const vector<string> &hand) {
    vector<int> values;
    vector<char> suits;
    unordered_map<int, int> freq;

    for (auto card: hand) {
        values.push_back(valueMap[card[0]]);
        suits.push_back(card[1]);
        freq[values.back()]++;
    }

    sort(values.rbegin(), values.rend());

    const bool flush = is_flush(suits);
    const bool straight = is_straight(values);

    int rank = 10;
    if (flush && straight && values[0] == 14) return HandRank{rank, values};
    rank--;

    if (flush && straight) return HandRank{rank, values};
    rank--;

    if (has_count(freq, 4)) return HandRank{rank, get_distinct_values_ordered(freq)};
    rank--;

    if (has_count(freq, 3) && has_count(freq, 2)) return HandRank{rank, get_distinct_values_ordered(freq)};
    rank--;

    if (flush) return HandRank{rank, values};
    rank--;

    if (straight) return HandRank{rank, values};
    rank--;

    if (has_count(freq, 3)) return HandRank{rank, get_distinct_values_ordered(freq)};
    rank--;

    if (has_two_pairs(freq)) return HandRank{rank, get_distinct_values_ordered(freq)};
    rank--;

    if (has_count(freq, 2)) return HandRank{rank, get_distinct_values_ordered(freq)};
    rank--;

    assert(rank==1);
    return HandRank{rank, values};
}

#endif //EULER_POKER_H