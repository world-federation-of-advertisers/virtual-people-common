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

package src.main.kotlin.org.wfanet.virtualpeople.common.fieldfilter

import com.google.protobuf.Descriptors.Descriptor
import com.google.protobuf.Message
import org.wfanet.virtualpeople.common.FieldFilterProto
import org.wfanet.virtualpeople.common.FieldFilterProto.Op

// This is the Kotlin implementation of FieldFilterProto.
// @descriptor defines the target protobuf message type this FieldFilter checks.
// @config defines the checks that will be performed when calling IsMatch.
//
// This is the interface for all FieldFilter classes. Never add any behavior
// here.
interface FieldFilter {
  // Returns true if the @message satisfies the condition described by the FieldFilter
  fun isMatch(message: Message)
}

// Always use the constructor of FieldFilter to get a FieldFilter object.
// Users should never call the factory functions or the constructors of the
// derived classes.
fun newFieldFilter(descriptor: Descriptor, config: FieldFilterProto): FieldFilter {
  @Suppress("WHEN_ENUM_CAN_BE_NULL_IN_JAVA") // Proto enum fields are never null.
  return when (config.op) {
    Op.HAS -> HasFilter(descriptor, config)
    Op.EQUAL,
    Op.GT,
    Op.LT,
    Op.IN,
    Op.REGEXP,
    Op.OR,
    Op.AND,
    Op.NOT,
    Op.PARTIAL,
    Op.TRUE,
    Op.ANY_IN -> TODO("Not yet implemented")
    Op.UNRECOGNIZED,
    Op.INVALID -> error("Invalid OP")
  }
}

// Creates a FieldFilter, which checks the equality of all the fields set in
// the input @message, including nested fields.
//
// Float and double fields are not supported.
// Any repeated field is not supported.
fun newFieldFilter(message: Message): FieldFilter {
  TODO("Not yet implemented")
}
