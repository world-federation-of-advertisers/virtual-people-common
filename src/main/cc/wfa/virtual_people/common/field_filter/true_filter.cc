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

#include "wfa/virtual_people/common/field_filter/true_filter.h"

#include <memory>

#include "absl/memory/memory.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"

namespace wfa_virtual_people {

absl::StatusOr<std::unique_ptr<TrueFilter>> TrueFilter::New(
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::TRUE) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be TRUE. Input FieldFilterProto: ", config.DebugString()));
  }
  if (config.has_name()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Name should not be set. Input FieldFilterProto: ",
                     config.DebugString()));
  }
  if (config.has_value()) {
    return absl::InvalidArgumentError(
        absl::StrCat("Value should not be set. Input FieldFilterProto: ",
                     config.DebugString()));
  }
  if (config.sub_filters_size() > 0) {
    return absl::InvalidArgumentError(
        absl::StrCat("sub_filters must be empty. Input FieldFilterProto: ",
                     config.DebugString()));
  }
  return absl::make_unique<TrueFilter>();
}

bool TrueFilter::IsMatch(const google::protobuf::Message&) const {
  return true;
}

}  // namespace wfa_virtual_people
