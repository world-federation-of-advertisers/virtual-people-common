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

#include "wfa/virtual_people/common/field_filter/utils/field_util.h"

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "google/protobuf/descriptor.h"

namespace wfa_virtual_people {

absl::StatusOr<std::vector<const google::protobuf::FieldDescriptor*>>
GetFieldFromProto(
    const google::protobuf::Descriptor* descriptor,
    absl::string_view full_field_name,
    bool allow_repeated) {
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  std::vector<std::string> field_names = absl::StrSplit(full_field_name, ".");
  for (const std::string& field_name : field_names) {
    if (descriptor == nullptr) {
      return absl::InvalidArgumentError(absl::StrCat(
          "The field name is invalid: ", full_field_name));
    }
    const google::protobuf::FieldDescriptor* field_descriptor =
        descriptor->FindFieldByName(field_name);
    if (field_descriptor == nullptr) {
      return absl::InvalidArgumentError(absl::StrCat(
          "The field name is invalid: ", full_field_name));
    }
    field_descriptors.emplace_back(field_descriptor);
    descriptor = field_descriptor->message_type();
  }
  for (auto it = field_descriptors.begin(), j = field_descriptors.end() - 1;
       it != j; ++it) {
    if ((*it)->is_repeated()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Repeated field is not allowed in the path: ", full_field_name));
    }
  }
  if (!allow_repeated && field_descriptors.back()->is_repeated()) {
      return absl::InvalidArgumentError(absl::StrCat(
          "Repeated field is not allowed in the path: ", full_field_name));
  }
  return field_descriptors;
}

const google::protobuf::Message& GetParentMessageFromProto(
    const google::protobuf::Message& message,
    const std::vector<const google::protobuf::FieldDescriptor*>&
        field_descriptors) {
  const google::protobuf::Message* tmp_message = &message;
  for (auto it = field_descriptors.begin(), j = field_descriptors.end() - 1;
       it != j; ++it) {
    tmp_message = &(tmp_message->GetReflection()->GetMessage(*tmp_message,
                                                             *it));
  }
  return *tmp_message;
}

template <typename ValueType>
ValueType GetImmediateValueFromProto(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor);

template <>
int32_t GetImmediateValueFromProto<int32_t>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetInt32(message, field_descriptor);
}

template <>
int64_t GetImmediateValueFromProto<int64_t>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetInt64(message, field_descriptor);
}

template <>
uint32_t GetImmediateValueFromProto<uint32_t>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetUInt32(message, field_descriptor);
}

template <>
uint64_t GetImmediateValueFromProto<uint64_t>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetUInt64(message, field_descriptor);
}

template <>
float GetImmediateValueFromProto<float>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetFloat(message, field_descriptor);
}

template <>
double GetImmediateValueFromProto<double>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetDouble(message, field_descriptor);
}

template <>
bool GetImmediateValueFromProto<bool>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetBool(message, field_descriptor);
}

template <>
const google::protobuf::EnumValueDescriptor*
GetImmediateValueFromProto<const google::protobuf::EnumValueDescriptor*>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetEnum(message, field_descriptor);
}

// The field referred by @field_descriptor must be a string field.
template <>
const std::string& GetImmediateValueFromProto<const std::string&>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  // As long as the @field_descriptor refers to a string field, scratch is not
  // used.
  std::string scratch;
  return message.GetReflection()->GetStringReference(
      message, field_descriptor, &scratch);
}

template <>
const google::protobuf::Message&
GetImmediateValueFromProto<const google::protobuf::Message&>(
    const google::protobuf::Message& message,
    const google::protobuf::FieldDescriptor* field_descriptor) {
  return message.GetReflection()->GetMessage(message, field_descriptor);
}

}  // namespace wfa_virtual_people
