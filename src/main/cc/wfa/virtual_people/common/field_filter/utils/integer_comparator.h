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

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_INTEGER_COMPARATOR_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_INTEGER_COMPARATOR_H_

#include <memory>
#include <utility>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

namespace wfa_virtual_people {

enum IntegerCompareResult {
  GREATER_THAN = 1,
  EQUAL = 2,
  LESS_THAN = 3,
};

// A helper class to do integer comparison between a field in a protobuf message
// and a given integer represented by a string. The supported integer types are
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//
// The path represented by @field_descriptors must be a valid path to an integer
// field in @message.
//
// Usage example:
// If we have protobuf message:
// message A {
//   message B {
//     optional int32 c = 1;
//   }
//   optional B b = 1;
// }
//
// To get the field_descriptors:
// ASSIGN_OR_RETURN(
//     std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
//     GetFieldFromProto(A().GetDescriptor(), "b.c"));
//
// To build an IntegerComparator which compares the field b.c in message A with
// value 10:
// ASSIGN_OR_RETURN(
//   std::unique_ptr<IntegerComparator> comparator,
//   IntegerComparator::New(std::move(field_descriptors), "10"));
//
// Compare examples:
// A a_1;
// a_1.mutable_b()->set_c(11);
// IntegerCompareResult result_1 = comparator.Compare(a_1);  // GREATER_THAN
//
// A a_2;
// a_2.mutable_b()->set_c(10);
// IntegerCompareResult result_2 = comparator.Compare(a_2);  // EQUAL
//
// A a_3;
// a_3.mutable_b()->set_c(9);
// IntegerCompareResult result_3 = comparator.Compare(a_3);  // LESS_THAN
class IntegerComparator {
 public:
  // Always use IntegerComparator::New to get an IntegerComparator object.
  //
  // Returns error status if any of the following happens:
  // * The last entry of @field_descriptors refers to a non-integer field.
  // * The type of @value does not match the type of the field represented by
  //   the last entry of @field_descriptors.
  //
  // No repeated field is allowed in @field_descriptors. This will not be caught
  // in this class. But when following the examples above, the repeated field
  // error will be caught before calling this library.
  static absl::StatusOr<std::unique_ptr<IntegerComparator>> New(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      absl::string_view value);

  IntegerComparator(const IntegerComparator&) = delete;
  IntegerComparator& operator=(const IntegerComparator&) = delete;

  virtual ~IntegerComparator() = default;

  // Compares the field represented by @field_descriptors in @message and
  // @value.
  // Returns GREATER_THAN if the field in @message is greater than @value.
  // Returns EQUAL if the field in @message is equal to @value.
  // Returns LESS_THAN if the field in @message is less than @value.
  //
  // @field_descriptors must represent a valid path in @message to an integer
  // field.
  virtual IntegerCompareResult Compare(
      const google::protobuf::Message& message) const = 0;

 protected:
  IntegerComparator(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors)
      : field_descriptors_(std::move(field_descriptors)) {}

  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
};

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_INTEGER_COMPARATOR_H_
