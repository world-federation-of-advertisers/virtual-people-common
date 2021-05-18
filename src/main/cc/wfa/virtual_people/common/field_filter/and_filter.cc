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

#include "wfa/virtual_people/common/field_filter/and_filter.h"

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/measurement/common/macros.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<AndFilter>> AndFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::AND) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be AND. Input FieldFilterProto: ",
        config.DebugString()));
  }
  if (config.sub_filters_size() == 0) {
    return absl::InvalidArgumentError(absl::StrCat(
        "sub_filters must be set when op is AND. Input FieldFilterProto: ",
        config.DebugString()));
  }

  std::unique_ptr<std::vector<std::unique_ptr<FieldFilter>>> sub_filters =
      absl::make_unique<std::vector<std::unique_ptr<FieldFilter>>>();
  for (const FieldFilterProto& sub_filter_proto : config.sub_filters()) {
    sub_filters->emplace_back();
    ASSIGN_OR_RETURN(
        sub_filters->back(),
        FieldFilter::New(descriptor, sub_filter_proto));
  }

  return absl::make_unique<AndFilter>(std::move(sub_filters));
}

bool AndFilter::IsMatch(const google::protobuf::Message& message) const {
  for (auto it = sub_filters_->begin(); it != sub_filters_->end(); ++it) {
    if (!(*it)->IsMatch(message)) {
      return false;
    }
  }
  return true;
}

}  // namespace wfa_virtual_people
