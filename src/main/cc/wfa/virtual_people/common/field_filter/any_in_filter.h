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

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_ANY_IN_FILTER_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_ANY_IN_FILTER_H_

#include <memory>
#include <utility>
#include <vector>

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is ANY_IN in @config.
class AnyInFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call AnyInFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  // * @config.op is not ANY_IN.
  // * @config.name is not set.
  // * Except the last field, any other field of the path represented by
  //   @config.name is repeated field.
  // * The last field of the path represented by @config.name is not repeated
  //   field.
  // * @config.value is not set.
  // * Any entry in @config.value (split by comma) cannot be casted to the type
  //   of the field represented by @config.name.
  static absl::StatusOr<std::unique_ptr<AnyInFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  AnyInFilter(const AnyInFilter&) = delete;
  AnyInFilter& operator=(const AnyInFilter&) = delete;

  virtual ~AnyInFilter() = default;

  // Returns true when any value of the repeated field represented by
  // @config.name in @message equals to any value in @config.value.
  // Otherwise, returns false.
  bool IsMatch(const google::protobuf::Message& message) const override = 0;

 protected:
  AnyInFilter(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors)
      : field_descriptors_(std::move(field_descriptors)) {}

  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
};

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_ANY_IN_FILTER_H_
