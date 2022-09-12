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

package org.wfanet.virtualpeople.common.fieldfilter

import com.google.protobuf.MessageOrBuilder
import org.wfanet.virtualpeople.common.FieldFilterProto

/**
 * The implementation of [FieldFilter] when op is TRUE in config.
 *
 * Always use [FieldFilter.create]. Users should never construct a [TrueFilter] directly.
 */
internal class TrueFilter(config: FieldFilterProto) : FieldFilter {
  init {
    if (config.op != FieldFilterProto.Op.TRUE) {
      error("Op must be TRUE. Input FieldFilterProto: $config")
    }
    if (config.hasName()) {
      error("Name should not be set. Input FieldFilterProto: $config")
    }
    if (config.hasValue()) {
      error("Value should not be set. Input FieldFilterProto: $config")
    }
    if (config.subFiltersCount > 0) {
      error("subFilters must be empty. Input FieldFilterProto: $config")
    }
  }

  override fun matches(messageOrBuilder: MessageOrBuilder): Boolean {
    return true
  }
}
