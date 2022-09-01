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

package org.wfanet.virtualpeople.common.fieldfilter.utils

import com.google.common.truth.extensions.proto.ProtoTruth.assertThat
import java.lang.IllegalStateException
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.runners.JUnit4
import org.wfanet.virtualpeople.common.FieldFilterProto.Op
import org.wfanet.virtualpeople.common.fieldFilterProto
import org.wfanet.virtualpeople.common.test.TestProtoB
import org.wfanet.virtualpeople.common.test.testProto
import org.wfanet.virtualpeople.common.test.testProtoA
import org.wfanet.virtualpeople.common.test.testProtoB

@RunWith(JUnit4::class)
class MessageFilterUtilTest {

  @Test
  fun `float not supported should throw`() {
    val filterMessage = testProto { a = testProtoA { b = testProtoB { floatValue = 1.0f } } }
    assertFailsWith<IllegalStateException> { convertMessageToFilter(filterMessage) }
  }

  @Test
  fun `double not supported should throw`() {
    val filterMessage = testProto { a = testProtoA { b = testProtoB { doubleValue = 1.0 } } }
    assertFailsWith<IllegalStateException> { convertMessageToFilter(filterMessage) }
  }

  @Test
  fun `repeated not supported should throw`() {
    val filterMessage = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int32Values.add(1)
        }
      }
    }
    assertFailsWith<IllegalStateException> { convertMessageToFilter(filterMessage) }
  }

  @Test
  fun `convert message to filter`() {
    val filterMessage = testProto {
      a = testProtoA {
        b = testProtoB {
          int32Value = 1
          int64Value = 1
          uint32Value = 1
          uint64Value = 1
          enumValue = TestProtoB.TestEnum.TEST_ENUM_1
          stringValue = "string1"
        }
      }
    }

    val expectedFilter = fieldFilterProto {
      op = Op.AND
      subFilters.add(
        fieldFilterProto {
          op = Op.PARTIAL
          name = "a"
          subFilters.add(
            fieldFilterProto {
              op = Op.AND
              subFilters.add(
                fieldFilterProto {
                  op = Op.PARTIAL
                  name = "b"
                  subFilters.add(
                    fieldFilterProto {
                      op = Op.AND
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "int32_value"
                          value = "1"
                        }
                      )
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "int64_value"
                          value = "1"
                        }
                      )
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "uint32_value"
                          value = "1"
                        }
                      )
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "uint64_value"
                          value = "1"
                        }
                      )
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "enum_value"
                          value = "TEST_ENUM_1"
                        }
                      )
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "string_value"
                          value = "string1"
                        }
                      )
                    }
                  )
                }
              )
            }
          )
        }
      )
    }
    assertThat(expectedFilter).isEqualTo(convertMessageToFilter(filterMessage))
  }

  @Test
  fun `bool True suported`() {
    val filter = fieldFilterProto {
      op = Op.AND
      subFilters.add(
        fieldFilterProto {
          op = Op.PARTIAL
          name = "a"
          subFilters.add(
            fieldFilterProto {
              op = Op.AND
              subFilters.add(
                fieldFilterProto {
                  op = Op.PARTIAL
                  name = "b"
                  subFilters.add(
                    fieldFilterProto {
                      op = Op.AND
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "bool_value"
                          value = "true"
                        }
                      )
                    }
                  )
                }
              )
            }
          )
        }
      )
    }

    val filterMessage = testProto { a = testProtoA { b = testProtoB { boolValue = true } } }
    assertEquals(filter, convertMessageToFilter(filterMessage))
  }

  @Test
  fun `bool False suported`() {
    val filter = fieldFilterProto {
      op = Op.AND
      subFilters.add(
        fieldFilterProto {
          op = Op.PARTIAL
          name = "a"
          subFilters.add(
            fieldFilterProto {
              op = Op.AND
              subFilters.add(
                fieldFilterProto {
                  op = Op.PARTIAL
                  name = "b"
                  subFilters.add(
                    fieldFilterProto {
                      op = Op.AND
                      subFilters.add(
                        fieldFilterProto {
                          op = Op.EQUAL
                          name = "bool_value"
                          value = "false"
                        }
                      )
                    }
                  )
                }
              )
            }
          )
        }
      )
    }

    val filterMessage = testProto { a = testProtoA { b = testProtoB { boolValue = false } } }
    assertEquals(filter, convertMessageToFilter(filterMessage))
  }
}
