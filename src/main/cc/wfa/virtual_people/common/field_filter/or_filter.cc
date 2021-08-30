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

#include "wfa/virtual_people/common/field_filter/or_filter.h"

#include <memory>
#include <utility>
#include <vector>

#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "common_cpp/macros/macros.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<OrFilter>> OrFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::OR) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be OR. Input FieldFilterProto: ", config.DebugString()));
  }
  if (config.sub_filters_size() == 0) {
    return absl::InvalidArgumentError(absl::StrCat(
        "sub_filters must be set when op is OR. Input FieldFilterProto: ",
        config.DebugString()));
  }

  std::vector<std::unique_ptr<FieldFilter>> sub_filters;
  for (const FieldFilterProto& sub_filter_proto : config.sub_filters()) {
    sub_filters.emplace_back();
    ASSIGN_OR_RETURN(sub_filters.back(),
                     FieldFilter::New(descriptor, sub_filter_proto));
  }

  return absl::make_unique<OrFilter>(std::move(sub_filters));
}

bool OrFilter::IsMatch(const google::protobuf::Message& message) const {
  for (auto& filter : sub_filters_) {
    if (filter->IsMatch(message)) {
      return true;
    }
  }
  return false;
}

}  // namespace wfa_virtual_people
