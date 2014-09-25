/**
 *  Container for generic multi-dimensional array.
 */

#pragma once
#ifndef ISMRMRD_NDARRAY_H
#define ISMRMRD_NDARRAY_H

#include <valarray>

namespace ISMRMRD {

template <typename T> class NDArrayContainer {

public:
    NDArrayContainer() {}

    /**
     * @brief Construct with dimensions and data
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions) {
        dimensions_ = dimensions;
        data_.resize(elements());
    }

    /**
     * @brief Construct with dimensions and data
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions, const T* d) {
        dimensions_ = dimensions;
        data_.resize(elements());
        memcpy(&data_[0],d,sizeof(T)*elements());
    }

    /**
     * @brief Construct with dimensions and preset value
     */
    NDArrayContainer(const std::vector<unsigned int>& dimensions, const std::valarray<T>& t) {
        dimensions_ = dimensions;
        data_.resize(elements());
        data_ = t;
    }

    virtual ~NDArrayContainer() {}

    std::vector<unsigned int> dimensions_; /**< Array with dimensions of the array. First dimension is fastest moving in the array */
    std::valarray<T> data_;               /**< The data itself. A vector is used here for easy memory management                  */

    size_t elements() const {
        if (dimensions_.size() == 0) {
            return 0;
        }
        size_t elements = 1;
        std::vector<unsigned int>::const_iterator it = dimensions_.begin();
        while (it != dimensions_.end()) {
            elements *= *(it++);
        }
        return elements;
    }

    T& operator[] (const size_t& p) {
        return data_[p];
    }

    T operator[] (const size_t& p) const {
        return data_[p];
    }

    void resize (const size_t& s) {
        data_.resize(s);
    }

    void resize (const size_t& s, const T& t) {
        data_.resize(s,t);
    }

    bool is_consistent() const {
        return (elements() == data_.size());
    }

    size_t ndims() const {
        size_t i = 1, n_dimensions = 1;
        for (; i < dimensions_.size(); i++)
            n_dimensions += (size_t) (dimensions_[i] > 1);
        return n_dimensions;
    }

};

} // namespace ISMRMRD
#endif // ISMRMRD_NDARRAY_H
