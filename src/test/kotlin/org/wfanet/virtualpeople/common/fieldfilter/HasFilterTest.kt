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
import org.wfanet.virtualpeople.common.FieldFilterProto.Op
import org.wfanet.virtualpeople.common.fieldFilterProto
import org.wfanet.virtualpeople.common.test.TestProto
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class HasFilterTest {

  @Test
  fun `filter without name should fail`() {
    val fieldFilter = fieldFilterProto { op = Op.HAS }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Name must be set"))
  }

  @Test
  fun `filter with invalid name should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.invalid_field"
      op = Op.HAS
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("The field name is invalid"))
  }

  @Test
  fun `filter with repeated field in the path should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "repeated_proto_a.b.int32_value"
      op = Op.HAS
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed in the path"))
  }

  @Test
  fun `has nonrepeated field`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.HAS
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int64Value = 1 } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto {}
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))
  }

  @Test
  fun `has repeated field`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.HAS
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Values.add(1) } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Values.add(1)
          int32Values.add(2)
        }
      }
    }
    assertTrue(filter.matches(testProto2))
    assertTrue(filter.matches(testProto2.toBuilder()))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int32Value = 1 } } }
    assertFalse(filter.matches(testProto3))
    assertFalse(filter.matches(testProto3.toBuilder()))

    val testProto4 = testProto {}
    assertFalse(filter.matches(testProto4))
    assertFalse(filter.matches(testProto4.toBuilder()))
  }
}
