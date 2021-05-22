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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_FIELD_FILTER_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_FIELD_FILTER_H_

#include "absl/status/statusor.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"

namespace wfa_virtual_people {

// This is the C++ implementation of FieldFilterProto.
// @descriptor defines the target protobuf message type this FieldFilter checks.
// @config defines the checks that will be performed when calling IsMatch.
//
// This is the interface for all FieldFilter classes. Never add any behavior
// here.
class FieldFilter {
 public:
  // Always use FieldFilter::New to get a FieldFilter object.
  // Users should never call the factory functions or the constructors of the
  // derived classes.
  static absl::StatusOr<std::unique_ptr<FieldFilter>> New(
      const google::protobuf::Descriptor* descriptor,
      const FieldFilterProto& config);

  FieldFilter(const FieldFilter&) = delete;
  FieldFilter& operator=(const FieldFilter&) = delete;

  virtual ~FieldFilter() = default;

  // Returns true if the @message satisfies the condition given by the @config.
  // The type of @message must match the corresponding message type of
  // @descriptor.
  virtual bool IsMatch(const google::protobuf::Message& message) const = 0;

 protected:
  FieldFilter() = default;
};

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_FIELD_FILTER_H_
