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

#include "wfa/virtual_people/common/field_filter/partial_filter.h"

#include "absl/memory/memory.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "common_cpp/macros/macros.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<PartialFilter>> PartialFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::PARTIAL) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be PARTIAL. Input FieldFilterProto: ", config.DebugString()));
  }
  if (!config.has_name()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Name must be set. Input FieldFilterProto: ", config.DebugString()));
  }
  if (config.sub_filters_size() == 0) {
    return absl::InvalidArgumentError(absl::StrCat(
        "sub_filters must be set when op is PARTIAL. Input FieldFilterProto: ",
        config.DebugString()));
  }

  // Get the FieldDescriptors to the field represented by @config.name.
  ASSIGN_OR_RETURN(
      std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
      GetFieldFromProto(descriptor, config.name()));
  if (field_descriptors.back()->cpp_type() !=
      google::protobuf::FieldDescriptor::CppType::CPPTYPE_MESSAGE) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Name must refer to a message type field. Input FieldFilterProto: ",
        config.DebugString()));
  }

  // Build all the sub filters.
  const google::protobuf::Descriptor* sub_descriptor =
      field_descriptors.back()->message_type();
  std::vector<std::unique_ptr<FieldFilter>> sub_filters;
  for (const FieldFilterProto& sub_filter_proto : config.sub_filters()) {
    sub_filters.emplace_back();
    ASSIGN_OR_RETURN(sub_filters.back(),
                     FieldFilter::New(sub_descriptor, sub_filter_proto));
  }

  return absl::make_unique<PartialFilter>(std::move(field_descriptors),
                                          std::move(sub_filters));
}

bool PartialFilter::IsMatch(const google::protobuf::Message& message) const {
  const google::protobuf::Message& sub_message =
      GetValueFromProto<const google::protobuf::Message&>(message,
                                                          field_descriptors_);
  for (auto& filter : sub_filters_) {
    if (!filter->IsMatch(sub_message)) {
      return false;
    }
  }
  return true;
}

}  // namespace wfa_virtual_people
