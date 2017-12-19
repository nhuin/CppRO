#ifndef MULTIDIMARRAY_HPP
#define MULTIDIMARRAY_HPP

#include <algorithm>
#include <array>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

#include "utility.hpp"

template <typename T, std::size_t N, typename Container = std::vector<T>>
class MultiDimArray {
  public:
    template <std::size_t D>
    class DimensionView {
      public:
        explicit DimensionView(MultiDimArray& _arr, std::array<std::size_t, N> _indexes)
            : m_arr(_arr)
            , m_indexes(std::move(_indexes)) {}

        class iterator {
            friend DimensionView;

          public:
            ~iterator() = default;

            iterator(const iterator&) = default;
            iterator& operator=(const iterator&) = default;
            iterator(iterator&&) noexcept = default;
            iterator& operator=(iterator&&) noexcept = default;

            T& operator*() {
                return m_arr(m_indexes);
            }

            T& operator++() {
                m_indexes[D]++;
                return m_arr(m_indexes);
            }

            T& operator->() {
                return m_arr(m_indexes);
            }

            const T operator++(int /*unused*/) {
                auto retval = m_arr(m_indexes);
                m_indexes[D]++;
                return retval;
            }

            T& operator--() {
                m_indexes[D]++;
                return m_arr(m_indexes);
            }

            const T operator--(int /*unused*/) {
                auto retval = m_arr(m_indexes);
                m_indexes[D]++;
                return retval;
            }

            bool operator==(iterator& _ite) {
                return std::mismatch(m_indexes.begin(), m_indexes.end(), _ite.m_indexes.begin()).first == m_indexes.end();
            }

            bool operator!=(iterator& _ite) {
                return std::mismatch(m_indexes.begin(), m_indexes.end(), _ite.m_indexes.begin()).first != m_indexes.end();
            }

          private:
            iterator(MultiDimArray& _arr, std::array<std::size_t, N> _indexes)
                : m_arr(_arr)
                , m_indexes(std::move(_indexes)) {}

            MultiDimArray& m_arr;
            std::array<std::size_t, N> m_indexes;
        };

        iterator begin() {
            auto retval = iterator(m_arr, m_indexes);
            retval.m_indexes[D] = 0;
            return retval;
        }
        iterator end() {
            auto retval = iterator(m_arr, m_indexes);
            retval.m_indexes[D] = m_arr.getDim<D>() + 1;
            return retval;
        }
        ~DimensionView() = default;

        DimensionView(const DimensionView&) = default;
        DimensionView& operator=(const DimensionView&) = default;
        DimensionView(DimensionView&&) noexcept = default;
        DimensionView& operator=(DimensionView&&) noexcept = default;

      private:
        MultiDimArray& m_arr;
        std::array<std::size_t, N> m_indexes;
    };

    explicit MultiDimArray(std::array<std::size_t, N> _arr, const T& _value = T());

    template <typename... Args>
    typename std::enable_if<(sizeof...(Args) == N), T&>::type
    operator()(const Args... _args);

    template <typename... Args>
    typename std::enable_if<(sizeof...(Args) == N), const T&>::type
    operator()(const Args... _args) const;

    T& operator()(const std::size_t (&_il)[N]);

    const T& operator()(const std::size_t (&_il)[N]) const;

    T& operator()(const std::array<std::size_t, N>& _arr);

    const T& operator()(const std::array<std::size_t, N>& _arr) const;

    std::size_t getTotalSize() const {
        return m_array.size();
    }

    template <std::size_t D>
    std::size_t getDim() {
        return m_dim[D];
    }

    const Container& getContainer() const {
        return m_array;
    }

    ~MultiDimArray() = default;

    MultiDimArray(const MultiDimArray&) = default;
    MultiDimArray& operator=(const MultiDimArray&) = default;
    MultiDimArray(MultiDimArray&&) noexcept = default;
    MultiDimArray& operator=(MultiDimArray&&) noexcept = default;

  private:
    template <typename... Args, std::size_t... Is>
    std::size_t getValue_impl(std::tuple<Args...> _args, std::index_sequence<Is...> /*unused*/);

    std::array<std::size_t, N> m_dim;
    Container m_array;
};

template <typename T, std::size_t N, typename Container>
MultiDimArray<T, N, Container>::MultiDimArray(std::array<std::size_t, N> _arr, const T& _value)
    : m_dim(std::move(_arr))
    , m_array(std::accumulate(std::begin(m_dim), std::end(m_dim), 1, std::multiplies<std::size_t>()), _value) {}

template <typename T, std::size_t N, typename Container>
template <typename... Args, std::size_t... Is>
std::size_t MultiDimArray<T, N, Container>::getValue_impl(std::tuple<Args...> _args, std::index_sequence<Is...> /*unused*/) {
    // x + xSize * (y + ySize * ())
    std::size_t retval = 0;
    return (..., (retval *= m_dim[Is], retval += std::get<Is>(_args)));
    return retval;
}

template <typename T, std::size_t N, typename Container>
template <typename... Args>
typename std::enable_if<(sizeof...(Args) == N), T&>::type
MultiDimArray<T, N, Container>::operator()(const Args... _args) {
    // std::cout << std::make_tuple(_args...) << '\t' << getValue_impl(std::make_tuple(_args...), std::index_sequence_for<Args...>{}) << '\n';
    return m_array[getValue_impl(std::make_tuple(_args...), std::index_sequence_for<Args...>{})];
}

template <typename T, std::size_t N, typename Container>
template <typename... Args>
typename std::enable_if<(sizeof...(Args) == N), const T&>::type
MultiDimArray<T, N, Container>::operator()(const Args... _args) const {
    // std::cout << std::make_tuple(_args...) << '\t' << getValue_impl(std::make_tuple(_args...), std::index_sequence_for<Args...>{}) << '\n';
    return m_array[getValue_impl(std::make_tuple(_args...), std::index_sequence_for<Args...>{})];
}

template <typename T, std::size_t N, typename Container>
T& MultiDimArray<T, N, Container>::operator()(const std::size_t (&_il)[N]) {
    std::size_t in = 0;
    std::size_t i = 0;
    for (const auto& _v : _il) {
        in *= m_dim[i];
        in += _v;
        ++i;
    }
    return m_array[in];
}

template <typename T, std::size_t N, typename Container>
const T& MultiDimArray<T, N, Container>::operator()(const std::size_t (&_il)[N]) const {
    std::size_t in = 0;
    std::size_t i = 0;
    for (const auto& _v : _il) {
        in *= m_dim[i];
        in += _v;
        ++i;
    }
    return m_array[in];
}

template <typename T, std::size_t N, typename Container>
T& MultiDimArray<T, N, Container>::operator()(const std::array<std::size_t, N>& _il) {
    std::size_t in = 0;
    std::size_t i = 0;
    for (const auto& _v : _il) {
        in *= m_dim[i];
        in += _v;
        ++i;
    }
    return m_array[in];
}

template <typename T, std::size_t N, typename Container>
const T& MultiDimArray<T, N, Container>::operator()(const std::array<std::size_t, N>& _il) const {
    std::size_t in = 0;
    std::size_t i = 0;
    for (const auto& _v : _il) {
        in *= m_dim[i];
        in += _v;
        ++i;
    }
    return m_array[in];
}

#endif
