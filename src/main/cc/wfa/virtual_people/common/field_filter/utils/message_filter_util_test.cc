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

#include "wfa/virtual_people/common/field_filter/utils/message_filter_util.h"

#include "absl/status/status.h"
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/main/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::EqualsProto;
using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

TEST(ConvertMessageToFilterTest, FloatNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      a {
        b {
          float_value: 1.0
        }
      }
  )PROTO", &filter_message));
  EXPECT_THAT(
      ConvertMessageToFilter(filter_message).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertMessageToFilterTest, DoubleNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      a {
        b {
          double_value: 1.0
        }
      }
  )PROTO", &filter_message));
  EXPECT_THAT(
      ConvertMessageToFilter(filter_message).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertMessageToFilterTest, RepeatedNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      int32_values: 1
  )PROTO", &filter_message));
  EXPECT_THAT(
      ConvertMessageToFilter(filter_message).status(),
      StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(ConvertMessageToFilterTest, Successful) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      a {
        b {
          int32_value: 1
          int64_value: 1
          uint32_value: 1
          uint64_value: 1
          enum_value: TEST_ENUM_1
          string_value: "string1"
        }
      }
  )PROTO", &filter_message));
  ASSERT_OK_AND_ASSIGN(
      FieldFilterProto filter, ConvertMessageToFilter(filter_message));

  FieldFilterProto expected_filter;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      op: AND
      sub_filters {
        op: PARTIAL
        name: "a"
        sub_filters {
          op: AND
          sub_filters {
            op: PARTIAL
            name: "b"
            sub_filters {
              op: AND
              sub_filters {
                op: EQUAL
                name: "int32_value"
                value: "1"
              }
              sub_filters {
                op: EQUAL
                name: "int64_value"
                value: "1"
              }
              sub_filters {
                op: EQUAL
                name: "uint32_value"
                value: "1"
              }
              sub_filters {
                op: EQUAL
                name: "uint64_value"
                value: "1"
              }
              sub_filters {
                op: EQUAL
                name: "enum_value"
                value: "TEST_ENUM_1"
              }
              sub_filters {
                op: EQUAL
                name: "string_value"
                value: "string1"
              }
            }
          }
        }
      }
  )PROTO", &expected_filter));
  EXPECT_THAT(filter, EqualsProto(expected_filter));
}

TEST(ConvertMessageToFilterTest, BoolTrue_Successful) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      a {
        b {
          bool_value: true
        }
      }
  )PROTO", &filter_message));
  ASSERT_OK_AND_ASSIGN(
      FieldFilterProto filter, ConvertMessageToFilter(filter_message));

  FieldFilterProto expected_filter;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      op: AND
      sub_filters {
        op: PARTIAL
        name: "a"
        sub_filters {
          op: AND
          sub_filters {
            op: PARTIAL
            name: "b"
            sub_filters {
              op: AND
              sub_filters {
                op: EQUAL
                name: "bool_value"
                value: "true"
              }
            }
          }
        }
      }
  )PROTO", &expected_filter));
  EXPECT_THAT(filter, EqualsProto(expected_filter));
}

TEST(ConvertMessageToFilterTest, BoolFalse_Successful) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      a {
        b {
          bool_value: false
        }
      }
  )PROTO", &filter_message));
  ASSERT_OK_AND_ASSIGN(
      FieldFilterProto filter, ConvertMessageToFilter(filter_message));

  FieldFilterProto expected_filter;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"PROTO(
      op: AND
      sub_filters {
        op: PARTIAL
        name: "a"
        sub_filters {
          op: AND
          sub_filters {
            op: PARTIAL
            name: "b"
            sub_filters {
              op: AND
              sub_filters {
                op: EQUAL
                name: "bool_value"
                value: "false"
              }
            }
          }
        }
      }
  )PROTO", &expected_filter));
  EXPECT_THAT(filter, EqualsProto(expected_filter));
}

}  // namespace
}  // namespace wfa_virtual_people
