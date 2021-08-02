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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_VALUES_PARSER_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_VALUES_PARSER_H_

#include "absl/container/flat_hash_set.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/strings/str_split.h"
#include "common_cpp/macros/macros.h"
#include "google/protobuf/descriptor.h"
#include "wfa/virtual_people/common/field_filter/utils/template_util.h"
#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

namespace wfa_virtual_people {

template <typename ValueType, EnableIfNonFloatProtoValueType<ValueType> = true>
struct ParsedValues {
  absl::flat_hash_set<ValueType> values;
};

template <>
struct ParsedValues<const google::protobuf::EnumValueDescriptor*> {
  absl::flat_hash_set<int> values;
};

template <>
struct ParsedValues<const std::string&> {
  absl::flat_hash_set<std::string> values;
};

// A helper function to parse and store @values_str as a set of ValueType.
// @values_str is a string represents a list of ValueType entities separated by
// comma.
// The supported ValueType are:
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//   bool
//   const std::string&
template <typename ValueType, EnableIfIntBoolStrType<ValueType> = true>
absl::StatusOr<ParsedValues<ValueType>> ParseValues(
    absl::string_view values_str) {
  std::vector<std::string> values_list = absl::StrSplit(values_str, ',');
  ParsedValues<ValueType> parsed_values;
  for (const std::string& value_str : values_list) {
    ASSIGN_OR_RETURN(
        ValueType value, ConvertToNumeric<ValueType>(value_str));
    parsed_values.values.insert(value);
  }
  return parsed_values;
}

template <> inline
absl::StatusOr<ParsedValues<const std::string&>>
ParseValues<const std::string&>(absl::string_view values_str) {
  std::vector<std::string> values_list = absl::StrSplit(values_str, ',');
  ParsedValues<const std::string&> parsed_values;
  parsed_values.values = absl::flat_hash_set<std::string>(
      values_list.begin(), values_list.end());
  return parsed_values;
}

// A helper function to parse and store @values_str as a set of protobuf enums.
// @values_str is a string represents a list of protobuf enum values (enum
// number or enum name) separated by comma.
absl::StatusOr<ParsedValues<const google::protobuf::EnumValueDescriptor*>>
ParseEnumValues(
    const google::protobuf::EnumDescriptor* descriptor,
    absl::string_view values_str);

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_VALUES_PARSER_H_
