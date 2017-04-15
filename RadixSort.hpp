#ifndef RADIXSORT_HPP
#define RADIXSORT_HPP

#include <iostream>
#include <type_traits>
#include <vector>
#include <ostream>
#include <iterator>
#include <algorithm>

#include "utility.hpp"

template<typename T>
class RightRadixSort {
	static_assert(std::is_integral<T>::value && std::is_unsigned<T>::value, "Radix sort cannot be applied to non integral number");
	public:
		RightRadixSort() = default;

		template<typename InputIterator>
		static void sort(InputIterator _begin, InputIterator _end) {
			constexpr int base = 1 << 8;
			constexpr int baseModulo = base - 1;

			std::array<std::vector<T>, base> buckets;
			for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
				for(auto& b : buckets) {
					b.clear();
				};
				// Put numbers in buckets
				std::for_each(_begin, _end, [&](const T _v){
					buckets[((_v >> (8 * i)) & baseModulo)].push_back(_v);
				});

				// Transfer from bucket to vect
				auto ite = _begin;
				for(const auto& bucket : buckets) {					
					for(auto v : bucket) {
						*ite++ = v;
					}
				}
			}
		}
};

template<typename T>
class CountingRightRadixSort {
	static_assert(std::is_integral<T>::value, "Radix sort can only be applied to integral number");
	public:
		CountingRightRadixSort() = default;

		template<typename RandomIterator>
		static void sort(const RandomIterator _begin, const RandomIterator _end) {
			constexpr int base = 1 << 8;
		    constexpr int baseModulo = base - 1;
		    // std::vector<T> bucketPlace(std::distance(_begin, _end));
		    std::array<int, base> nbValue;
		    std::array<int, base> bucketIndices;
		    std::vector<char> inRightPlace(std::distance(_begin, _end)+1);

		    for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
		        const int power = 8 * i;
		        std::fill(nbValue.begin(), nbValue.end(), 0);
		        std::fill(inRightPlace.begin(), inRightPlace.end(), 0);
		        for(auto ite = _begin; ite != _end; ++ite) {
		        	++nbValue[(*ite >> power) & baseModulo];
		        }

		        bucketIndices[0] = 0;
		        for(int b = 1; b < base; ++b) {
		            bucketIndices[b] = bucketIndices[b-1] + nbValue[b-1];
		        }

		        T buffer = *_begin;
		        int bufferIndex = (buffer >> power) & baseModulo;
		        auto cycleStart = _begin;
		        auto ite = _begin + bucketIndices[(buffer >> power) & baseModulo];
	        	auto itePlace = inRightPlace.begin() + bucketIndices[(buffer >> power) & baseModulo];

		        for(int j = 0; j < static_cast<int>(std::distance(_begin, _end)); ++j) {
	        		bucketIndices[bufferIndex]++;
	        		std::swap(buffer, *ite);
	        		*itePlace = 1;
	        		bufferIndex = (buffer >> power) & baseModulo;
	        		if(ite == cycleStart) {  
	        			do {
	        				++ite;
	        				++itePlace;
	        			} while(*itePlace == 1);
	        			buffer = *ite;
	        			cycleStart = ite;
	        		} else {
	        			ite = _begin + bucketIndices[bufferIndex];
	        			itePlace = inRightPlace.begin() + bucketIndices[bufferIndex];
	        		}
			    }
			}
	    }

		static void sort(std::vector<T>& _vect) {
			constexpr int base = 1 << 8;
		    constexpr int baseModulo = base - 1;
		    std::array<int, base> nbValue;
		    std::array<int, base> bucketIndices;
		    std::vector<char> inRightPlace(_vect.size());

		    for(int i = 0; i < static_cast<int>(sizeof(T)); ++i) {
		        const int power = 8 * i;
		        std::fill(nbValue.begin(), nbValue.end(), 0);
		        std::fill(inRightPlace.begin(), inRightPlace.end(), 0);
		        for(const T v : _vect) {
		        	++nbValue[(v >> power) & baseModulo];
		        }

		        bucketIndices[0] = 0;
		        for(int b = 1; b < base; ++b) {
		            bucketIndices[b] = bucketIndices[b-1] + nbValue[b-1];
		        }

		        T buffer = _vect[0];
		        int bufferIndex = (buffer >> power) & baseModulo;
		        int cycleStart = 0;
		        int ind = bucketIndices[(buffer >> power) & baseModulo];

		        for(int j = 0; j < static_cast<int>(_vect.size()); ++j) {
	        		bucketIndices[bufferIndex]++;
	        		std::swap(buffer, _vect[ind]);
	        		inRightPlace[ind] = 1;
	        		bufferIndex = (buffer >> power) & baseModulo;
	        		if(ind == cycleStart) {
	        			do {
	        				++ind;
	        			} while(inRightPlace[ind] == 1);
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