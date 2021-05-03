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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TYPE_CONVERT_UTIL_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TYPE_CONVERT_UTIL_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "google/protobuf/descriptor.h"

namespace wfa_virtual_people {

// Convert the string @input to a numeric type. The allowed output types are
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//   float
//   double
//   bool
template <typename ValueType>
absl::StatusOr<ValueType> ConvertToNumeric(const std::string& input);

// Convert the string @input to a enum type descriptor.
// The enum is referred by @field_descriptor.
absl::StatusOr<const google::protobuf::EnumValueDescriptor*> ConvertToEnum(
    const google::protobuf::FieldDescriptor* field_descriptor,
    const std::string& input);

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_TYPE_CONVERT_UTIL_H_
