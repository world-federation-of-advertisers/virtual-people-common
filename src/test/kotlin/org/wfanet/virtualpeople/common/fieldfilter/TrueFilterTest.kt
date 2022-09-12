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
class TrueFilterTest {

  @Test
  fun `is match`() {
    val fieldFilter = fieldFilterProto { op = Op.TRUE }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto {}

    assertTrue(filter.matches(testProto))
    assertTrue(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `is match with not empty event`() {
    val fieldFilter = fieldFilterProto { op = Op.TRUE }
    val filter = FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
    val testProto = testProto { a = testProtoA { b = testProtoB { stringValue = "a" } } }

    assertTrue(filter.matches(testProto))
    assertTrue(filter.matches(testProto.toBuilder()))
  }

  @Test
  fun `filter with name should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.TRUE
      name = "a"
    }

    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Name should not be set"))
  }

  @Test
  fun `filter with value should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.TRUE
      value = "1"
    }

    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("Value should not be set"))
  }

  @Test
  fun `filter with subfilters should fail`() {
    val fieldFilter = fieldFilterProto {
      op = Op.TRUE
      subFilters.add(
        fieldFilterProto {
          op = Op.EQUAL
          name = "a"
          value = "1"
        }
      )
    }

    val exception =
      assertFailsWith<IllegalStateException> {
        FieldFilter.create(TestProto.getDescriptor(), fieldFilter)
      }
    assertTrue(exception.message!!.contains("subFilters must be empty"))
  }
}
