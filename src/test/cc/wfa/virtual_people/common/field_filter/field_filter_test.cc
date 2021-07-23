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

#include "wfa/virtual_people/common/field_filter/field_filter.h"

#include "absl/status/status.h"
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "gtest/gtest.h"
#include "src/test/cc/testutil/matchers.h"
#include "src/test/cc/testutil/status_macros.h"
#include "src/test/cc/wfa/virtual_people/common/field_filter/test/test.pb.h"

namespace wfa_virtual_people {
namespace {

using ::wfa::StatusIs;
using ::wfa_virtual_people::test::TestProto;

TEST(FieldFilterTest, FromMessageFloatNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { float_value: 1.0 } }
      )pb",
      &filter_message));
  EXPECT_THAT(FieldFilter::New(filter_message).status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldFilterTest, FromMessageDoubleNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a { b { double_value: 1.0 } }
      )pb",
      &filter_message));
  EXPECT_THAT(FieldFilter::New(filter_message).status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldFilterTest, FromMessageRepeatedNotSupported) {
  TestProto filter_message;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(R"pb(
                                                              int32_values: 1
                                                            )pb",
                                                            &filter_message));
  EXPECT_THAT(FieldFilter::New(filter_message).status(),
              StatusIs(absl::StatusCode::kInvalidArgument, ""));
}

TEST(FieldFilterTest, FromMessageSuccessful) {
  TestProto filter_message, test_proto_1, test_proto_2;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a {
          b {
            int32_value: 1
            int64_value: 1
            uint32_value: 1
            uint64_value: 1
            bool_value: true
            enum_value: TEST_ENUM_1
            string_value: "string1"
          }
        }
      )pb",
      &filter_message));
  ASSERT_OK_AND_ASSIGN(std::unique_ptr<FieldFilter> filter,
                       FieldFilter::New(filter_message));

  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a {
          b {
            int32_value: 1
            int64_value: 1
            uint32_value: 1
            uint64_value: 1
            float_value: 1.0
            double_value: 1.0
            bool_value: true
            enum_value: TEST_ENUM_1
            string_value: "string1"
          }
        }
        int32_values: 1
        int32_values: 2
      )pb",
      &test_proto_1));
  EXPECT_TRUE(filter->IsMatch(test_proto_1));

  // a.b.int32_value does not match.
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(
      R"pb(
        a {
          b {
            int32_value: 2
            int64_value: 1
            uint32_value: 1
            uint64_value: 1
            float_value: 1.0
            double_value: 1.0
            bool_value: true
            enum_value: TEST_ENUM_1
            string_value: "string1"
          }
        }
        int32_values: 1
        int32_values: 2
      )pb",
      &test_proto_2));
  EXPECT_FALSE(filter->IsMatch(test_proto_2));
}

}  // namespace
}  // namespace wfa_virtual_people
