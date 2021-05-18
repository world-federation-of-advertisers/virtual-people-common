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

#include "wfa/virtual_people/common/field_filter/equal_filter.h"

#include "absl/memory/memory.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "src/main/proto/wfa/virtual_people/common/field_filter.pb.h"
#include "wfa/virtual_people/common/field_filter/field_filter.h"
#include "wfa/virtual_people/common/field_filter/utils/field_util.h"
#include "wfa/virtual_people/common/field_filter/utils/type_convert_util.h"

namespace wfa_virtual_people {

// The implementation of EqualFilter based on the type of field represented by
// @field_descriptors. The supported ValueTypes are
//   int32_t
//   int64_t
//   uint32_t
//   uint64_t
//   bool
//   const google::protobuf::EnumValueDescriptor*
//   std::string
template <typename ValueType>
class EqualFilterImpl : public EqualFilter {
 public:
  explicit EqualFilterImpl(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      ValueType value):
      field_descriptors_(std::move(field_descriptors)),
      value_(value) {}

  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
  ValueType value_;
};

template <>
class EqualFilterImpl<std::string> : public EqualFilter {
 public:
  explicit EqualFilterImpl(
      std::vector<const google::protobuf::FieldDescriptor*>&& field_descriptors,
      absl::string_view value):
      field_descriptors_(std::move(field_descriptors)),
      value_(value) {}

  bool IsMatch(const google::protobuf::Message& message) const override;

 private:
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors_;
  std::string value_;
};

template <typename ValueType>
bool EqualFilterImpl<ValueType>::IsMatch(
    const google::protobuf::Message& message) const {
  return value_ == GetValueFromProto<ValueType>(message, field_descriptors_);
}

template <>
bool EqualFilterImpl<const google::protobuf::EnumValueDescriptor*>::IsMatch(
    const google::protobuf::Message& message) const {
  return (
      value_->number() ==
      GetValueFromProto<const google::protobuf::EnumValueDescriptor*>(
          message, field_descriptors_)->number());
}

bool EqualFilterImpl<std::string>::IsMatch(
    const google::protobuf::Message& message) const {
  return (
      value_ ==
      GetValueFromProto<const std::string&>(message, field_descriptors_));
}

absl::StatusOr<std::unique_ptr<EqualFilter>> EqualFilter::New(
    const google::protobuf::Descriptor* descriptor,
    const FieldFilterProto& config) {
  if (config.op() != FieldFilterProto::EQUAL) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Op must be EQUAL. Input FieldFilterProto: ", config.DebugString()));
  }
  if (!config.has_name()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Name must be set. Input FieldFilterProto: ", config.DebugString()));
  }
  if (!config.has_value()) {
    return absl::InvalidArgumentError(absl::StrCat(
        "Value must be set. Input FieldFilterProto: ", config.DebugString()));
  }
  std::vector<const google::protobuf::FieldDescriptor*> field_descriptors;
  absl::Status status =
      GetFieldFromProto(descriptor, config.name(), &field_descriptors);
  if (!status.ok()) {
    return status;
  }

  switch (field_descriptors.back()->cpp_type()) {
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
      {
        absl::StatusOr<int32_t> value =
            ConvertToNumeric<int32_t>(config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not int32_t. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<int32_t>>(
            std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT64:
      {
        absl::StatusOr<int64_t> value =
            ConvertToNumeric<int64_t>(config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not int64_t. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<int64_t>>(
            std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT32:
      {
        absl::StatusOr<uint32_t> value =
            ConvertToNumeric<uint32_t>(config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not uint32_t. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<uint32_t>>(
            std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_UINT64:
      {
        absl::StatusOr<uint64_t> value =
            ConvertToNumeric<uint64_t>(config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not uint64_t. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<uint64_t>>(
            std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_BOOL:
      {
        absl::StatusOr<bool> value =
            ConvertToNumeric<bool>(config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not bool. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<bool>>(
            std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_ENUM:
      {
        absl::StatusOr<const google::protobuf::EnumValueDescriptor*> value =
            ConvertToEnum(field_descriptors.back(), config.value());
        if (!value.ok()) {
          return absl::InvalidArgumentError(absl::StrCat(
              "Value is not enum. Input FieldFilterProto: ",
              config.DebugString()));
        }
        return absl::make_unique<EqualFilterImpl<
            const google::protobuf::EnumValueDescriptor*>>(
                std::move(field_descriptors), value.value());
      }
    case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
      {
        // ValueType must be "std::string" rather than "const std::string&".
        // Otherwise, value_ would be a reference to config.value(), and when
        // config is out of scope, value_ would refer to nothing.
        return absl::make_unique<EqualFilterImpl<std::string>>(
            std::move(field_descriptors), config.value());
      }
    default:
      return absl::InvalidArgumentError(absl::StrCat(
          "Unsupported field type for EQUAL filter. Input FieldFilterProto: ",
          config.DebugString()));
  }
}

}  // namespace wfa_virtual_people
