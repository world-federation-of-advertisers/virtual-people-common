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

package org.wfanet.virtualpeople.common.fieldfilter.utils

import java.lang.IllegalStateException
import java.lang.NumberFormatException
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import kotlin.test.assertTrue
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.test.TestProto
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class IntegerComparatorUtilTest {

  @Test
  fun `field not integer should throw`() {
    /** This is a float field. */
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.float_value")
    val exception =
      assertFailsWith<IllegalStateException> { IntegerComparator.new(fieldDescriptors, "1") }
    assertTrue(exception.message!!.contains("field is not integer"))
  }

  @Test
  fun `invalid value should throw`() {
    /** "a" is not a valid integer. */
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")
    val exception =
      assertFailsWith<NumberFormatException> { IntegerComparator.new(fieldDescriptors, "a") }
    assertTrue(exception.message!!.contains("For input string"))
  }

  @Test
  fun `value type not match should throw`() {
    /** "1L" is not a valid int32. */
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")
    val exception =
      assertFailsWith<NumberFormatException> { IntegerComparator.new(fieldDescriptors, "1L") }
    assertTrue(exception.message!!.contains("For input string"))
  }

  @Test
  fun `value type mismatch should throw`() {
    /** "10.5" is not a valid int32. */
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")
    val exception =
      assertFailsWith<NumberFormatException> { IntegerComparator.new(fieldDescriptors, "10.5") }
    assertTrue(exception.message!!.contains("For input string"))
  }

  @Test
  fun `int32 integer comparator`() {
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.int32_value")
    val comparator = IntegerComparator.new(fieldDescriptors, "10")

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int32Value = 11 } } }
    assertEquals(comparator.compare(testProto1), IntegerCompareResult.GREATER_THAN)

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int32Value = 10 } } }
    assertEquals(comparator.compare(testProto2), IntegerCompareResult.EQUAL)

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int32Value = 9 } } }
    assertEquals(comparator.compare(testProto3), IntegerCompareResult.LESS_THAN)

    /** Return INVALID when the field is not set. */
    val testProto4 = testProto {}
    assertEquals(comparator.compare(testProto4), IntegerCompareResult.INVALID)
  }

  @Test
  fun `int64 integer comparator`() {
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.int64_value")
    val comparator = IntegerComparator.new(fieldDescriptors, "10")

    val testProto1 = testProto { a = testProtoA { b = testProtoB { int64Value = 11 } } }
    assertEquals(comparator.compare(testProto1), IntegerCompareResult.GREATER_THAN)

    val testProto2 = testProto { a = testProtoA { b = testProtoB { int64Value = 10 } } }
    assertEquals(comparator.compare(testProto2), IntegerCompareResult.EQUAL)

    val testProto3 = testProto { a = testProtoA { b = testProtoB { int64Value = 9 } } }
    assertEquals(comparator.compare(testProto3), IntegerCompareResult.LESS_THAN)

    /** Return INVALID when the field is not set. */
    val testProto4 = testProto {}
    assertEquals(comparator.compare(testProto4), IntegerCompareResult.INVALID)
  }

  @Test
  fun `uint32 integer comparator`() {
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.uint32_value")
    val comparator = IntegerComparator.new(fieldDescriptors, "10")

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint32Value = 11 } } }
    assertEquals(comparator.compare(testProto1), IntegerCompareResult.GREATER_THAN)

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint32Value = 10 } } }
    assertEquals(comparator.compare(testProto2), IntegerCompareResult.EQUAL)

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint32Value = 9 } } }
    assertEquals(comparator.compare(testProto3), IntegerCompareResult.LESS_THAN)

    /** Return INVALID when the field is not set. */
    val testProto4 = testProto {}
    assertEquals(comparator.compare(testProto4), IntegerCompareResult.INVALID)
  }

  @Test
  fun `uint64 integer comparator`() {
    val fieldDescriptors = getFieldFromProto(TestProto.getDescriptor(), "a.b.uint64_value")
    val comparator = IntegerComparator.new(fieldDescriptors, "10")

    val testProto1 = testProto { a = testProtoA { b = testProtoB { uint64Value = 11 } } }
    assertEquals(comparator.compare(testProto1), IntegerCompareResult.GREATER_THAN)

    val testProto2 = testProto { a = testProtoA { b = testProtoB { uint64Value = 10 } } }
    assertEquals(comparator.compare(testProto2), IntegerCompareResult.EQUAL)

    val testProto3 = testProto { a = testProtoA { b = testProtoB { uint64Value = 9 } } }
    assertEquals(comparator.compare(testProto3), IntegerCompareResult.LESS_THAN)

    /** Return INVALID when the field is not set. */
    val testProto4 = testProto {}
    assertEquals(comparator.compare(testProto4), IntegerCompareResult.INVALID)
  }
}
