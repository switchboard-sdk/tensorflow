/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef MLIR_HLO_DIALECT_MHLO_TRANSFORMS_MAP_CHLO_TO_HLO_OP_H
#define MLIR_HLO_DIALECT_MHLO_TRANSFORMS_MAP_CHLO_TO_HLO_OP_H

#include <type_traits>

#include "mlir-hlo/Dialect/mhlo/IR/chlo_ops.h"
#include "mlir-hlo/Dialect/mhlo/IR/hlo_ops.h"
#include "mlir/IR/PatternMatch.h"

namespace mlir {
namespace chlo {

template <typename FromOpTy, typename ToOpTy>
struct HloNaryElementwiseAdaptor {
  static ToOpTy createOp(FromOpTy fromOp, Type resultType,
                         ValueRange broadcastedOperands, OpBuilder &builder) {
    return builder.create<ToOpTy>(fromOp.getLoc(), resultType,
                                  broadcastedOperands);
  }
};
struct HloCompareAdaptor {
  static mhlo::CompareOp createOp(BroadcastCompareOp fromOp, Type resultType,
                                  ValueRange broadcastedOperands,
                                  OpBuilder &builder) {
    return builder.create<mhlo::CompareOp>(
        fromOp.getLoc(), resultType, broadcastedOperands[0],
        broadcastedOperands[1], fromOp.comparison_direction(),
        fromOp.compare_typeAttr());
  }
};

// Populate a pattern for each Broadcasting CHlo op. This requires the pattern
// to take a ChloOpTy, NonBroadcastingOpTy, and an Adaptor as templated values.
template <template <typename, typename, typename> class Pattern,
          typename... ConstructorArgs>
void populateForBroadcastingBinaryOp(MLIRContext *context,
                                     RewritePatternSet *patterns,
                                     ConstructorArgs &&...args) {
#define POPULATE_BCAST(ChloOp, HloOp)                                          \
  patterns                                                                     \
      ->add<Pattern<ChloOp, HloOp, HloNaryElementwiseAdaptor<ChloOp, HloOp>>>( \
          context, args...);

  POPULATE_BCAST(BroadcastAddOp, mhlo::AddOp);
  POPULATE_BCAST(BroadcastAndOp, mhlo::AndOp);
  POPULATE_BCAST(BroadcastAtan2Op, mhlo::Atan2Op);
  POPULATE_BCAST(BroadcastComplexOp, mhlo::ComplexOp);
  POPULATE_BCAST(BroadcastDivOp, mhlo::DivOp);
  POPULATE_BCAST(BroadcastMaxOp, mhlo::MaxOp);
  POPULATE_BCAST(BroadcastMinOp, mhlo::MinOp);
  POPULATE_BCAST(BroadcastMulOp, mhlo::MulOp);
  POPULATE_BCAST(BroadcastNextAfterOp, NextAfterOp);
  POPULATE_BCAST(BroadcastOrOp, mhlo::OrOp);
  POPULATE_BCAST(BroadcastPolygammaOp, PolygammaOp);
  POPULATE_BCAST(BroadcastPowOp, mhlo::PowOp);
  POPULATE_BCAST(BroadcastRemOp, mhlo::RemOp);
  POPULATE_BCAST(BroadcastShiftLeftOp, mhlo::ShiftLeftOp);
  POPULATE_BCAST(BroadcastShiftRightArithmeticOp, mhlo::ShiftRightArithmeticOp);
  POPULATE_BCAST(BroadcastShiftRightLogicalOp, mhlo::ShiftRightLogicalOp);
  POPULATE_BCAST(BroadcastSubOp, mhlo::SubtractOp);
  POPULATE_BCAST(BroadcastXorOp, mhlo::XorOp);
  POPULATE_BCAST(BroadcastZetaOp, ZetaOp);

  // Broadcasting ops requiring special construction.
  patterns
      ->insert<Pattern<BroadcastCompareOp, mhlo::CompareOp, HloCompareAdaptor>>(
          context, args...);

#undef POPULATE_BCAST
}

}  // namespace chlo
}  // namespace mlir

#endif  // MLIR_HLO_DIALECT_MHLO_TRANSFORMS_MAP_CHLO_TO_HLO_OP_H
