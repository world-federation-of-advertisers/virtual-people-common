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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_PARTIAL_FILTER_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_PARTIAL_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

// The implementation of field filter when op is PARTIAL in @config.
class PartialFilter : public FieldFilter {
 public:
  // Always use FieldFilter::New.
  // Users should never call PartialFilter::New or any constructor directly.
  //
  // Returns error status when any of the following happens:
  //   @config.op is not PARTIAL.
  //   @config.name is not set.
  //   @config.name does not refer to a message type.
  //   @config.sub_filters is empty.
  //   Any of @config.sub_filters is invalid to create a FieldFilter.
  static absl::StatusOr<std::unique_ptr<PartialFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  explicit PartialFilter(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      std::vector<std::unique_ptr<FieldFilter>>&& sub_filters):
      field_descriptors_(std::move(field_descriptors)),
      sub_filters_(std::move(sub_filters)) {}

  PartialFilter(const PartialFilter&) = delete;
  PartialFilter& operator=(const PartialFilter&) = delete;

  // Returns true when all the @config.sub_filters pass. Otherwise, returns
  // false.
  // The @config.sub_filters are applied to the message object refered by
  // @config.name.
  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
  std::vector<std::unique_ptr<FieldFilter>> sub_filters_;
};

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_PARTIAL_FILTER_H_
