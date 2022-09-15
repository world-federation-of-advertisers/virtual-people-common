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
import org.wfanet.virtualpeople.common.test.*

@RunWith(JUnit4::class)
class AndFilterTest {

  @Test
  fun `no sub filter should fail`() {
    val fieldFilter = fieldFilterProto { op = Op.AND }
    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("subFilters must be set when op is AND"))
  }

  @Test
  fun `all subfilters match should pass`() {
    val fieldFilter = fieldFilterProto {
      op = Op.AND
      subFilters.add(
        fieldFilterProto {
          name = "a.b.int32_value"
          op = Op.EQUAL
          value = "1"
        }
      )
      subFilters.add(
        fieldFilterProto {
          name = "a.b.int64_value"
          op = Op.EQUAL
          value = "1"
        }
      )
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 1
        }
      }
    }
    assertTrue(filter.matches(testProto1))
    assertTrue(filter.matches(testProto1.toBuilder()))
  }

  @Test
  fun `any subfilter mismatch should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.AND
      subFilters.add(
        fieldFilterProto {
          name = "a.b.int32_value"
          op = Op.EQUAL
          value = "1"
        }
      )
      subFilters.add(
        fieldFilterProto {
          name = "a.b.int64_value"
          op = Op.EQUAL
          value = "1"
        }
      )
    }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto1 = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 2
        }
      }
    }
    assertFalse(filter.matches(testProto1))
    assertFalse(filter.matches(testProto1.toBuilder()))
  }
}
