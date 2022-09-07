// Copyright 2022 The Cross-Media Measurement Authors
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

package org.wfanet.virtualpeople.common.fieldfilter

import kotlin.test.assertFailsWith
import kotlin.test.assertFalse
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.test.TestProtoB.TestEnum
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class FieldFilterTest {

  @Test
  fun `from message float not supported`() {
    val filterMessage = testProto { a = testProtoA { b = testProtoB { floatValue = 1.0f } } }
    val exception = assertFailsWith<IllegalStateException> { FieldFilter.create(filterMessage) }
    assertTrue(exception.message!!.contains("Unsupported field type"))
  }

  @Test
  fun `from message double not supported`() {
    val filterMessage = testProto { a = testProtoA { b = testProtoB { doubleValue = 1.0 } } }
    val exception = assertFailsWith<IllegalStateException> { FieldFilter.create(filterMessage) }
    assertTrue(exception.message!!.contains("Unsupported field type"))
  }

  @Test
  fun `from message repeated not supported`() {
    val filterMessage = testProto { a = testProtoA { b = testProtoB { int32Values.add(1) } } }
    val exception = assertFailsWith<IllegalStateException> { FieldFilter.create(filterMessage) }
    assertTrue(exception.message!!.contains("Repeated field"))
  }

  @Test
  fun `from message successful`() {
    val filterMessage = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 1
          uint32Value = 1
          uint64Value = 1
          boolValue = true
          enumValue = TestEnum.TEST_ENUM_1
          stringValue = "string1"
        }
      }
    }
    val filter = FieldFilter.create(filterMessage)

    val textproto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 1
          uint32Value = 1
          uint64Value = 1
          boolValue = true
          enumValue = TestEnum.TEST_ENUM_1
          stringValue = "string1"
        }
      }
      int32Values.add(1)
      int32Values.add(2)
    }
    assertTrue(filter.matches(textproto1))

    /** a.b.int32_value does not match. */
    val textproto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 2
          int64Value = 1
          uint32Value = 1
          uint64Value = 1
          boolValue = true
          enumValue = TestEnum.TEST_ENUM_1
          stringValue = "string1"
        }
      }
      int32Values.add(1)
      int32Values.add(2)
    }
    assertFalse(filter.matches(textproto2))
  }
}
