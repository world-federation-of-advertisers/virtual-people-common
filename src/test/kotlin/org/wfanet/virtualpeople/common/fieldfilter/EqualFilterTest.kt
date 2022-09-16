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
import org.wfanet.virtualpeople.common.test.TestProtoB.TestEnum
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class EqualFilterTest {

  @Test
  fun `filter without name should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.EQUAL
      value = "1"
    }
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
      op = Op.EQUAL
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("The field name is invalid"))
  }

  @Test
  fun `filter with repeated field at the end should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = Op.EQUAL
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed"))
  }

  @Test
  fun `filter with repeated field in the path should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "repeated_proto_a.b.int32_value"
      op = Op.EQUAL
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed in the path"))
  }

  @Test
  fun `int32 equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.EQUAL
      value = "1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int32Value = 2 } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `int32 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = Op.EQUAL
      value = "0"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `int64 equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = Op.EQUAL
      value = "1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { int64Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int64Value = 2 } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `int64 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = Op.EQUAL
      value = "0"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `uint32 equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = Op.EQUAL
      value = "1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint32Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint32Value = 2 } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `uint32 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = Op.EQUAL
      value = "0"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `uint64 equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = Op.EQUAL
      value = "1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint64Value = 1 } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint64Value = 2 } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `uint64 not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = Op.EQUAL
      value = "0"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `boolean equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_value"
      op = Op.EQUAL
      value = "false"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { boolValue = false } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { boolValue = true } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `boolean not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.bool_value"
      op = Op.EQUAL
      value = "false"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `enum equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.EQUAL
      value = "TEST_ENUM_1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestEnum.TEST_ENUM_1 } }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto {
      a = testProtoA { b = testProtoB { enumValue = TestEnum.TEST_ENUM_2 } }
    }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `enum not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.enum_value"
      op = Op.EQUAL
      value = "INVALID"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `string equal`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_value"
      op = Op.EQUAL
      value = "string1"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto { a = testProtoA { b = testProtoB { stringValue = "string1" } } }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { stringValue = "string2" } } }
    assertFalse(filter.matches(testProto2))
    assertFalse(filter.matches(testProto2.toBuilder()))
  }

  @Test
  fun `string not set`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.string_value"
      op = Op.EQUAL
      value = ""
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {}
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }
}
