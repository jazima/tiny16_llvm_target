#ifndef LLVM_TINY16FIXUPKINDS_H
#define LLVM_TINY16FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Tiny16 {
enum Fixups {
  // Currently do not support PC-relative fixups, but these are reserved for future use.
  fixup_tiny16_mov_hi8_pcrel = FirstTargetFixupKind,
  fixup_tiny16_mov_lo8_pcrel,
  // Absolute fixups for movhi/movlo
  fixup_tiny16_mov_hi8_abs,
  fixup_tiny16_mov_lo8_abs,

  // Marker
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
}
}

#endif

