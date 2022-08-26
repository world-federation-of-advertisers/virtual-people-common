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

import com.google.protobuf.Descriptors
import com.google.protobuf.Descriptors.FieldDescriptor
import com.google.protobuf.Message
import org.wfanet.virtualpeople.common.FieldFilterProto

internal class HasFilter(descriptor: Descriptors.Descriptor, config: FieldFilterProto) :
  FieldFilter {

  private val fieldDescriptor: List<FieldDescriptor>

  init {
    if (config.op != FieldFilterProto.Op.HAS) {
      error("Op must be HAS. Input FieldFilterProto: $config")
    }
    if (!config.hasName()) {
      error("Name must be set. Input FieldFilterProto: $config")
    }
    // TODO(@wangyaopw): Build fieldDescriptor from descriptor and config
    fieldDescriptor = listOf()
  }

  override fun matches(message: Message) {
    TODO("Not yet implemented")
  }
}
