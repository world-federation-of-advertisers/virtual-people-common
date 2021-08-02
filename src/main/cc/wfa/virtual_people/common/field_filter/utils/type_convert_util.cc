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

#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"
#include "wfa/virtual_people/common/field_filter/utils/template_util.h"

namespace wfa_virtual_people {

template <typename ValueType, EnableIfNumericType<ValueType>>
absl::StatusOr<ValueType> ConvertToNumeric(absl::string_view input);

template <>
absl::StatusOr<int32_t> ConvertToNumeric<int32_t>(absl::string_view input) {
  int32_t output;
  if (absl::SimpleAtoi(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to int32_t. Input: ", input));
}

template <>
absl::StatusOr<int64_t> ConvertToNumeric<int64_t>(absl::string_view input) {
  int64_t output;
  if (absl::SimpleAtoi(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to int64_t. Input: ", input));
}

template <>
absl::StatusOr<uint32_t> ConvertToNumeric<uint32_t>(absl::string_view input) {
  uint32_t output;
  if (absl::SimpleAtoi(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to uint32_t. Input: ", input));
}

template <>
absl::StatusOr<uint64_t> ConvertToNumeric<uint64_t>(absl::string_view input) {
  uint64_t output;
  if (absl::SimpleAtoi(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to uint64_t. Input: ", input));
}

template <>
absl::StatusOr<float> ConvertToNumeric<float>(absl::string_view input) {
  float output;
  if (absl::SimpleAtof(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to float. Input: ", input));
}

template <>
absl::StatusOr<double> ConvertToNumeric<double>(absl::string_view input) {
  double output;
  if (absl::SimpleAtod(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to double. Input: ", input));
}

template <>
absl::StatusOr<bool> ConvertToNumeric<bool>(absl::string_view input) {
  bool output;
  if (absl::SimpleAtob(input, &output)) {
    return output;
  }
  return absl::InvalidArgumentError(
      absl::StrCat("Input cannot be converted to bool. Input: ", input));
}

absl::StatusOr<const google::protobuf::EnumValueDescriptor*> ConvertToEnum(
    const google::protobuf::EnumDescriptor* descriptor,
    absl::string_view input) {
  // Try to get the enum by enum name.
  const google::protobuf::EnumValueDescriptor* enum_value_descriptor =
      descriptor->FindValueByName(std::string(input));

  if (enum_value_descriptor == nullptr) {
    // Did not find the enum by enum name. Try to find the enum by enum number.
    int enum_number;
    if (absl::SimpleAtoi(input, &enum_number)) {
      enum_value_descriptor = descriptor->FindValueByNumber(enum_number);
    }
  }

  if (enum_value_descriptor == nullptr) {
    return absl::InvalidArgumentError(
        absl::StrCat("Input cannot be converted to enum. Input: ", input));
  }
  return enum_value_descriptor;
}

}  // namespace wfa_virtual_people
