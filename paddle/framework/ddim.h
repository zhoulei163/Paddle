/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <boost/variant.hpp>
#include <initializer_list>
#include <stdexcept>
#include <vector>
#include "paddle/framework/dim.h"
#include "paddle/framework/enforce.h"
#include "unsupported/Eigen/CXX11/Tensor"

namespace paddle {
namespace framework {

namespace {
typedef boost::variant<Dim<1>, Dim<2>, Dim<3>, Dim<4>, Dim<5>, Dim<6>, Dim<7>,
                       Dim<8>, Dim<9>>
    DDimVar;
}

/**
 * \brief A dynamically sized dimension.
 *
 * The number of dimensions must be between [1, 9].
 */
struct DDim {
  DDimVar var;

  DDim() : var(Dim<1>()) {}

  template <int D>
  explicit DDim(const Dim<D>& in) : var(in) {}

  template <int D>
  DDim& operator=(const Dim<D>& in) {
    var = in;
    return *this;
  }

  int& operator[](int idx);
  int operator[](int idx) const;

  template <typename Visitor>
  typename Visitor::result_type apply_visitor(Visitor& visitor) {
    return var.apply_visitor(visitor);
  }

  template <typename Visitor>
  typename Visitor::result_type apply_visitor(Visitor& visitor) const {
    return var.apply_visitor(visitor);
  }

  DDimVar getVar() { return var; }

  bool operator==(DDim d) const;

  bool operator!=(DDim d) const;

  DDim operator+(DDim d) const;

  DDim operator*(DDim d) const;
};

/**
 * \brief Make a DDim from std::vector<int>
 *
 * \param dims An vector of ints. Must be sized between [1, 9]
 */
DDim make_ddim(const std::vector<int>& dims);

/**
 * \brief Make a DDim from an initializer list
 *
 * \param dims An initializer list of ints. Must be sized between [1, 9]
 *
 */
DDim make_ddim(std::initializer_list<int> dims);

int get(const DDim& dim, int idx);
void set(DDim& dim, int idx, int val);

std::vector<int> vectorize(const DDim& ddim);

ssize_t product(const DDim& ddim);

/**
 * \brief What is the length of this dimension?
 *
 * \param Dynamic dimension to inspect
 */

int arity(const DDim& ddim);

std::ostream& operator<<(std::ostream&, const DDim&);

template <int NDIMS>
Eigen::DSizes<Eigen::DenseIndex, NDIMS> ToEigenDSizes(const DDim& dims) {
  int rank = arity(dims);
  PADDLE_ENFORCE(rank == NDIMS, "DDim and NDIMS must be same");
  Eigen::DSizes<Eigen::DenseIndex, NDIMS> dsizes;
  for (int d = 0; d < rank; d++) {
    dsizes[d] = dims[d];
  }
  return dsizes;
}

}  // namespace framework
}  // namespace paddle

namespace boost {

template <typename T>
T get(const paddle::framework::DDim& in) {
  return boost::get<T>(in.var);
}

}  // namespace boost
