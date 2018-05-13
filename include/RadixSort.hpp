#ifndef RADIXSORT_HPP
#define RADIXSORT_HPP

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <ostream>
#include <type_traits>
#include <vector>

#include "utility.hpp"

template <typename T>
class LSDRadixSort {
    static_assert(std::is_integral<T>::value,
        "Radix sort cannot be applied to non integral number");
    static_assert(std::is_unsigned<T>::value,
        "Radix sort cannot be applied to non unsigned number (for now)");

  public:
    LSDRadixSort() = default;

    template <typename InputIterator>
    static void sort(InputIterator _begin, InputIterator _end) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;

        std::array<std::vector<T>, base> buckets;
        for (int i = 0; i < getMaxSetByte(*std::max_element(_begin, _end));
             ++i) {
            for (auto& b : buckets) {
                b.clear();
            };
            // Put numbers in buckets
            std::for_each(_begin, _end, [&](const T _v) {
                buckets[((_v >> (8 * i)) & baseModulo)].push_back(_v);
            });

            // Transfer from bucket to vect
            auto ite = _begin;
            for (const auto& bucket : buckets) {
                for (auto v : bucket) {
                    *ite++ = v;
                }
            }
        }
    }
};

template <typename T>
class MSDRadixSort {
    static_assert(std::is_integral<T>::value,
        "Radix sort cannot be applied to non integral number");
    static_assert(std::is_unsigned<T>::value,
        "Radix sort cannot be applied to non unsigned number (for now)");

  public:
    MSDRadixSort() = default;

    template <typename RandomIterator>
    static void sort(RandomIterator _begin, RandomIterator _end) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;

        std::array<std::vector<T>, base> buckets;
        std::vector<std::tuple<RandomIterator, RandomIterator, int>> stack = {
            std::make_tuple(
                _begin, _end, getMaxSetByte(*std::max_element(_begin, _end)))};
        while (!stack.empty()) {
            auto [begin, end, i] = stack.back();
            // std::copy(begin, end, std::ostream_iterator<T>(std::cout, ", "));
            // std::cout << ", " << i << '\n';
            stack.pop_back();
            const auto power = 8 * i;
            for (auto& b : buckets) {
                b.clear();
            };
            // Put numbers in buckets
            std::for_each(begin, end, [power, &buckets](const T _v) {
                buckets[((_v >> power) & baseModulo)].push_back(_v);
            });

            // Transfer from bucket to vect
            auto ite = begin;
            for (const auto& bucket : buckets) {
                auto sub_begin = ite;
                for (auto v : bucket) {
                    *ite++ = v;
                }
                auto sub_end = ite;
                if (std::distance(sub_begin, sub_end) > 1) {
                    assert(i > 0);
                    stack.emplace_back(sub_begin, sub_end, i - 1);
                }
            }
            // std::copy(begin, end, std::ostream_iterator<T>(std::cout, ", "));
            // std::cout << i << "\n\n";
        }
    }
};

template <typename T>
class CountingLSDRadixSort {
    static_assert(std::is_integral<T>::value,
        "Radix sort cannot be applied to non integral number");
    static_assert(std::is_unsigned<T>::value,
        "Radix sort cannot be applied to non unsigned number (for now)");

  public:
    CountingLSDRadixSort() = default;

    template <typename RandomIterator>
    static void sort(const RandomIterator _begin, const RandomIterator _end) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;

        std::array<uint64_t, base> nbValue;
        std::array<uint64_t, base> bucketIndices;
        std::vector<char> inRightPlace(std::distance(_begin, _end));

        for (int i = 0; i < sizeof(T); ++i) {
            const auto power = 8 * i;
            std::fill(nbValue.begin(), nbValue.end(), 0);
            std::fill(inRightPlace.begin(), inRightPlace.end(), 0);
            for (auto ite = _begin; ite != _end; ++ite) {
                ++nbValue[(*ite >> power) & baseModulo];
            }

            bucketIndices[0] = 0;
            for (int b = 1; b < base; ++b) {
                bucketIndices[b] = bucketIndices[b - 1] + nbValue[b - 1];
            }

            T buffer = *_begin;
            int bufferIndex = (buffer >> power) & baseModulo;
            auto cycleStart = _begin;
            auto ite = _begin + bucketIndices[(buffer >> power) & baseModulo];
            auto itePlace = inRightPlace.begin()
                            + bucketIndices[(buffer >> power) & baseModulo];

            for (int j = 0; j < std::distance(_begin, _end); ++j) {
                bucketIndices[bufferIndex]++;
                std::swap(buffer, *ite);
                *itePlace = 1;
                bufferIndex = (buffer >> power) & baseModulo;
                if (ite == cycleStart) {
                    do {
                        ++ite;
                        ++itePlace;
                    } while (*itePlace == 1);
                    buffer = *ite;
                    cycleStart = ite;
                } else {
                    ite = _begin + bucketIndices[bufferIndex];
                    itePlace =
                        inRightPlace.begin() + bucketIndices[bufferIndex];
                }
            }
        }
    }

    static void sort(std::vector<T>& _vect) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;
        std::array<int, base> nbValue;
        std::array<int, base> bucketIndices;
        std::vector<char> inRightPlace(_vect.size());

        for (int i = 0; i < sizeof(T); ++i) {
            const auto power = 8 * i;
            std::fill(nbValue.begin(), nbValue.end(), 0);
            std::fill(inRightPlace.begin(), inRightPlace.end(), 0);
            for (const T v : _vect) {
                ++nbValue[(v >> power) & baseModulo];
            }

            bucketIndices[0] = 0;
            for (int b = 1; b < base; ++b) {
                bucketIndices[b] = bucketIndices[b - 1] + nbValue[b - 1];
            }

            T buffer = _vect[0];
            int bufferIndex = (buffer >> power) & baseModulo;
            int cycleStart = 0;
            int ind = bucketIndices[(buffer >> power) & baseModulo];

            for (int j = 0; j < _vect.size(); ++j) {
                bucketIndices[bufferIndex]++;
                std::swap(buffer, _vect[ind]);
                inRightPlace[ind] = 1;
                bufferIndex = (buffer >> power) & baseModulo;
                if (ind == cycleStart) {
                    do {
                        ++ind;
                    } while (inRightPlace[ind] == 1);
                    buffer = _vect[ind];
                    cycleStart = ind;
                } else {
                    ind = bucketIndices[bufferIndex];
                }
            }
        }
    }
};

template <typename T>
class CountingMSDRadixSort {
    static_assert(std::is_integral<T>::value,
        "Radix sort cannot be applied to non integral number");
    static_assert(std::is_unsigned<T>::value,
        "Radix sort cannot be applied to non unsigned number (for now)");

  public:
    CountingMSDRadixSort() = default;

    template <typename RandomIterator>
    static void sort(const RandomIterator _begin, const RandomIterator _end) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;
        // std::vector<T> bucketPlace(std::distance(_begin, _end));
        std::array<int, base> nbValue;
        std::array<int, base> bucketIndices;

        std::vector<std::tuple<RandomIterator, RandomIterator, int>> stack = {
            std::make_tuple(_begin, _end, sizeof(T) - 1)};
        while (!stack.empty()) {
            auto [begin, end, i] = stack.back();
            stack.pop_back();
            const auto power = 8 * i;
            std::fill(nbValue.begin(), nbValue.end(), 0);

            // Count number of element for each digit
            for (auto ite = begin; ite != end; ++ite) {
                ++nbValue[(*ite >> power) & baseModulo];
            }
            // And there emplacement
            bucketIndices[0] = 0;
            for (int b = 1; b < base; ++b) {
                bucketIndices[b] = bucketIndices[b - 1] + nbValue[b - 1];
            }

            // Add subsection to stack
            for (int b = 0; b < base - 1; ++b) {
                if (nbValue[b] > 1) {
                    stack.emplace_back(begin + bucketIndices[b],
                        begin + bucketIndices[b + 1], i - 1);
                }
            }
            if (nbValue[base - 1] > 1) {
                stack.emplace_back(begin + bucketIndices[base - 1], end, i - 1);
            }

            std::vector<char> inRightPlace(std::distance(begin, end), 0);
            T buffer = *begin;
            int bufferIndex;
            RandomIterator cycleStart;
            RandomIterator ite;
            std::vector<char>::iterator itePlace;
            for (int j = 0; j < std::distance(begin, end); ++j) {
                if (ite == cycleStart) { // Permutation cycle ended
                    ite = begin;
                    itePlace = inRightPlace.begin(); // Searching for new cycle
                    while (*itePlace == 1) {
                        ++ite;
                        ++itePlace;
                        assert(ite
                               != end); // Must find a number in the wrong place
                    }
                    buffer = *ite;
                    cycleStart = ite;
                }
                bufferIndex = (buffer >> power) & baseModulo;
                ite = begin + bucketIndices[bufferIndex];
                itePlace = inRightPlace.begin() + bucketIndices[bufferIndex];
                bucketIndices[bufferIndex]++;
                std::swap(buffer, *ite);
                *itePlace = 1;
            }
        }
    }

    static void sort(std::vector<T>& _vect) {
        constexpr uint16_t base = 1 << 8;
        constexpr uint16_t baseModulo = base - 1;
        std::array<int, base> nbValue;
        std::array<int, base> bucketIndices;
        std::vector<char> inRightPlace(_vect.size());

        for (int i = 0; i < sizeof(T); ++i) {
            const auto power = 8 * i;
            std::fill(nbValue.begin(), nbValue.end(), 0);
            std::fill(inRightPlace.begin(), inRightPlace.end(), 0);
            for (const T v : _vect) {
                ++nbValue[(v >> power) & baseModulo];
            }

            bucketIndices[0] = 0;
            for (int b = 1; b < base; ++b) {
                bucketIndices[b] = bucketIndices[b - 1] + nbValue[b - 1];
            }

            T buffer = _vect[0];
            int bufferIndex = (buffer >> power) & baseModulo;
            int cycleStart = 0;
            int ind = bucketIndices[(buffer >> power) & baseModulo];

            for (int j = 0; j < _vect.size(); ++j) {
                bucketIndices[bufferIndex]++;
                std::swap(buffer, _vect[ind]);
                inRightPlace[ind] = 1;
                bufferIndex = (buffer >> power) & baseModulo;
                if (ind == cycleStart) {
                    do {
                        ++ind;
                    } while (inRightPlace[ind] == 1);
                    buffer = _vect[ind];
                    cycleStart = ind;
                } else {
                    ind = bucketIndices[bufferIndex];
                }
            }
        }
    }
};

#endif
