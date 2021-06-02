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

#ifndef WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_FIELD_UTIL_H_
#define WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_FIELD_UTIL_H_

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"

namespace wfa_virtual_people {

// In the protobuf message represented by @descriptor, get the field descriptors
// of the field, the path of which is represented by @full_field_name.
//
// @full_field_name is separated by ".".
//
// If @allow_repeated is true, only the last field in the path represented by
// @full_field_name is allowed to be a repeated field.
// If @allow_repeated is false, no repeated field is allowed in the path.
// Returns error status if any disallowed repeated field exists.
//
// The returned FieldDescriptors is in the order of the field name to access the
// nested field.
absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
GetFieldFromProto(
    const google::protobuf::Descriptor* descriptor,
    absl::string_view full_field_name,
    bool allow_repeated = false);

// Get the parent message of the field represented by @field_descriptors from
// the @message.
//
// All elements except the last one in @field_descriptors must refer to a
// protobuf Message.
// The first element in @field_descriptors must refer to a field in @message,
// Each of the rest elements must refer to a field in the message referred by
// the previous element.
//
// The typical usage is to first call GetFieldFromProto(see above), to get
// @field_descriptors for the target field in @message, then call this function
// to get the parent message of the target field.
// Example:
// If we have a protobuf
// message MsgA {
//   message MsgB {
//     optional int32 c = 1;
//   }
//   optional MsgB b = 1;
// }
// To get the field descriptors of MsgA.b.c, the call is
// ASSIGN_OR_RETURN(
//     std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
//     GetFieldFromProto(MsgA().GetDescriptor(), "b.c"));
// And if there is an MsgA object obj_a, to get the message obj_a.b, the
// call is
// const google::protobuf::Message& output =
//     GetParentMessageFromProto(obj_a, field_descriptors);
const google::protobuf::Message& GetParentMessageFromProto(
    const google::protobuf::Message& message,
    const std::vector<const google::protobuf::FieldDescriptor*>&
        field_descriptors);

// Get the value from the @message, with field name represented by
// @field_descriptor.
//
// The field must be an immediate field of @message.
// The corresponding C++ type of the field must be @ValueType.
template <typename ValueType>
ValueType GetImmediateValueFromProto(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor);

// Get the value from the @message, with field path represented by
// @field_descriptors.
//
// All elements except the last one in @field_descriptors must refer to a
// protobuf Message. The last one in @field_descriptors must refer to a field
// with @ValueType.
// The first element in @field_descriptors must refer to a field in @message,
// Each of the rest elements must refer to a field in the message referred by
// the previous element.
//
// The typical usage is to first call GetFieldFromProto(see above), to get
// @field_descriptors for the target field in @message, then call this function
// to get the value of the target field.
// Example:
// If we have a protobuf
// message MsgA {
//   message MsgB {
//     optional int32 c = 1;
//   }
//   optional MsgB b = 1;
// }
// To get the field descriptors of MsgA.b.c, the call is
// ASSIGN_OR_RETURN(
//     std::vector<const google::protobuf::FieldDescriptor*> field_descriptors,
//     GetFieldFromProto(MsgA().GetDescriptor(), "b.c"));
// And if there is an MsgA object obj_a, to get the value of obj_a.b.c, the
// call is
// int32_t output = GetValueFromProto(obj_a, field_descriptors);
template <typename ValueType>
ValueType GetValueFromProto(
    const google::protobuf::Message& message,
    const std::vector<const google::protobuf::FieldDescriptor*>&
        field_descriptors) {
  return GetImmediateValueFromProto<ValueType>(
      GetParentMessageFromProto(message, field_descriptors),
      field_descriptors.back());
}

}  // namespace wfa_virtual_people

#endif  // WFA_VIRTUAL_PEOPLE_COMMON_FIELD_FILTER_UTILS_FIELD_UTIL_H_
