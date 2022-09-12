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
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.fieldFilterProto
import org.wfanet.virtualpeople.common.test.TestProto
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class LtFilterTest {
  @Test
  fun `no name should fail`() {
    val fieldFilter = fieldFilterProto {
      op = FieldFilterProto.Op.LT
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Name must be set"))
  }

  @Test
  fun `no integer field should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.float_value"
      op = FieldFilterProto.Op.LT
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("The given field is not integer"))
  }

  @Test
  fun `disallowed reppeated should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_values"
      op = FieldFilterProto.Op.LT
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed"))
  }

  @Test
  fun `disallowed reppeated in the path should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "repeated_proto_a.b.int32_value"
      op = FieldFilterProto.Op.LT
      value = "1"
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Repeated field is not allowed in the path"))
  }

  @Test
  fun `no value should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = FieldFilterProto.Op.LT
    }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Value must be set"))
  }

  @Test
  fun `value not integer should fail`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = FieldFilterProto.Op.LT
      value = "10.5"
    }
    val exception =
      assertFailsWith<NumberFormatException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("For input string: \"10.5\""))
  }

  @Test
  fun `int32 should pass`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Value = 11 } } }
    assertFalse(filter.matches(testProto1))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int32Value = 10 } } }
    assertFalse(filter.matches(testProto2))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int32Value = 9 } } }
    assertTrue(filter.matches(testProto3))
  }

  @Test
  fun `int32 not set should return false`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int32_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
  }

  @Test
  fun `int64 should pass`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int64Value = 11 } } }
    assertFalse(filter.matches(testProto1))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int64Value = 10 } } }
    assertFalse(filter.matches(testProto2))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int64Value = 9 } } }
    assertTrue(filter.matches(testProto3))
  }

  @Test
  fun `int64 not set should return false`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.int64_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
  }

  @Test
  fun `uint32 should pass`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint32Value = 11 } } }
    assertFalse(filter.matches(testProto1))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint32Value = 10 } } }
    assertFalse(filter.matches(testProto2))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint32Value = 9 } } }
    assertTrue(filter.matches(testProto3))
  }

  @Test
  fun `uint32 not set should return false`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint32_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
  }

  @Test
  fun `uint64 should pass`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint64Value = 11 } } }
    assertFalse(filter.matches(testProto1))

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint64Value = 10 } } }
    assertFalse(filter.matches(testProto2))

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint64Value = 9 } } }
    assertTrue(filter.matches(testProto3))
  }

  @Test
  fun `uint64 not set should return false`() {
    val fieldFilter = fieldFilterProto {
      name = "a.b.uint64_value"
      op = FieldFilterProto.Op.LT
      value = "10"
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    /** Return false when the field is not set. */
    assertFalse(filter.matches(testProto))
  }
}
