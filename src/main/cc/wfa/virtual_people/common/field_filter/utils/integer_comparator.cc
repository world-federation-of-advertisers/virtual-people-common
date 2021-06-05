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

#include "wfa/virtual_people/common/field_filter/utils/integer_comparator.h"

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "wfa/measurement/common/macros.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"
#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

namespace wfa_virtual_people {

template <typename IntegerType>
class IntegerComparatorImpl : public IntegerComparator {
 public:
  IntegerComparatorImpl(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      IntegerType value
  ): IntegerComparator(std::move(field_descriptors)), value_(value) {}

  IntegerCompareResult Compare(
      const google::protobuf::Message& message) const override {
    IntegerType field_value =
        GetValueFromProto<IntegerType>(message, field_descriptors_);
    if (field_value > value_) {
      return IntegerCompareResult::GREATER_THAN;
    }
    if (field_value < value_) {
      return IntegerCompareResult::LESS_THAN;
    }
    return IntegerCompareResult::EQUAL;
  }

 private:
  IntegerType value_;
};

template <typename IntegerType>
absl::StatusOr<std::unique_ptr<IntegerComparatorImpl<IntegerType>>>
CreateComparator(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view value) {
  ASSIGN_OR_RETURN(IntegerType int_value, ConvertToNumeric<IntegerType>(value));
  return absl::make_unique<IntegerComparatorImpl<IntegerType>>(
      std::move(field_descriptors), int_value);
}

absl::StatusOr<std::unique_ptr<IntegerComparator>> IntegerComparator::New(
    std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
    absl::string_view value) {
  switch (field_descriptors.back()->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
      return CreateComparator<int32_t>(std::move(field_descriptors), value);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
      return CreateComparator<int64_t>(std::move(field_descriptors), value);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
      return CreateComparator<uint32_t>(std::move(field_descriptors), value);
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
      return CreateComparator<uint64_t>(std::move(field_descriptors), value);
    default:
      return absl::InvalidArgumentError(
          "The given field is not integer when building IntegerComparator.");
  }
}

}  // namespace wfa_virtual_people
