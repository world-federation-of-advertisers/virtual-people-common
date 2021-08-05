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

#ifndef SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_MESSAGE_FILTER_UTIL_H_
#define SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_MESSAGE_FILTER_UTIL_H_

#include "absl/status/statusor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"

namespace wfa_virtual_people {

// Converts @message to a FieldFilterProto.
//
// The output represents a field filter, which checks the equality of all the
// existing fields, including nested fields, of @message.
//
// Float and double fields are not supported.
// Any repeated field is not supported.
absl::StatusOr<FieldFilterProto> ConvertMessageToFilter(
    const google::protobuf::Message& message);

}  // namespace wfa_virtual_people

#endif  // SRC_MAIN_CC_WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_MESSAGE_FILTER_UTIL_H_
