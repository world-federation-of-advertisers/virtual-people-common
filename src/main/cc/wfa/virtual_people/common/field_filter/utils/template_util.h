// Copyright 2021 The Cross-Media Measurement Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TEMPLATE_UTIL_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TEMPLATE_UTIL_H_

#include "absl/meta/type_traits.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

namespace wfa_virtual_people {

template <typename ValueType>
using IsIntegerType = absl::disjunction<
    std::is_same<ValueType, int32_t>,
    std::is_same<ValueType, int64_t>,
    std::is_same<ValueType, uint32_t>,
    std::is_same<ValueType, uint64_t>>;

template <typename ValueType>
using EnableIfIntegerType = absl::enable_if_t<
    IsIntegerType<ValueType>::value, bool>;

template <typename ValueType>
using IsNumericType = absl::disjunction<
    IsIntegerType<ValueType>,
    std::is_same<ValueType, float>,
    std::is_same<ValueType, double>,
    std::is_same<ValueType, bool>>;

template <typename ValueType>
using EnableIfNumericType = absl::enable_if_t<
    IsNumericType<ValueType>::value, bool>;

template <typename ValueType>
using IsProtoValueType = absl::disjunction<
    IsNumericType<ValueType>,
    std::is_same<ValueType, const google::protobuf::EnumValueDescriptor*>,
    std::is_same<ValueType, const std::string&>>;

template <typename ValueType>
using EnableIfProtoType = absl::enable_if_t<absl::disjunction<
    IsProtoValueType<ValueType>,
    std::is_same<ValueType, const google::protobuf::Message&>
>::value, bool>;

template <typename ValueType>
using EnableIfProtoValueType = absl::enable_if_t<
    IsProtoValueType<ValueType>::value, bool>;

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TEMPLATE_UTIL_H_
