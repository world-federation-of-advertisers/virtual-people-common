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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_IN_FILTER_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_IN_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is IN in @config.
class InFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call InFilter::New or any constructor directly.
  //
  // Returns error status if any of the following happens:
  // * @config.op is not IN.
  // * @config.name is not set.
  // * Any field of the path represented by @config.name is repeated field.
  // * @config.value is not set.
  // * Any entry in @config.value (split by comma) cannot be casted to the type
  //   of the field represented by @config.name.
  static absl::StatusOr<std::unique_ptr<InFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  InFilter(const InFilter&) = delete;
  InFilter& operator=(const InFilter&) = delete;

  virtual ~InFilter() = default;

  // Returns true when the value of the field represented by @config.name in
  // @message equals to any value in @config.value.
  // Otherwise, returns false.
  virtual bool IsMatch(
      const google::protobuf::Message& message) const override = 0;

 protected:
  InFilter(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors)
      : field_descriptors_(std::move(field_descriptors)) {}

  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
};

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_IN_FILTER_H_
