#ifndef TINY16BASEINFO_H
#define TINY16BASEINFO_H

#include "Tiny16MCTargetDesc.h"
#include "llvm/Support/ErrorHandling.h"

namespace llvm {

/// Tiny16II - This namespace holds all of the target specific flags that
/// instruction info tracks.
///
namespace Tiny16II {

  /// Target Operand Flag enum.
  enum TOF {
    //===------------------------------------------------------------------===//
    // Tiny16-Specific MachineOperand flags.

    MO_NO_FLAG = 0,

    /// MO_LO8 - On a symbol operand, this represents a relocation containing
    /// lower 8 bit of the address.
    MO_LO8 = 0x1,

    /// MO_HI8 - On a symbol operand, this represents a relocation containing
    /// higher 8 bit of the address.
    MO_HI8 = 0x2,

    /// MO_OPTION_MASK - Most flags are mutually exclusive; this mask selects
    /// just that part of the flag set.
    MO_OPTION_MASK = 0x7f,

    // It's undefined behaviour if an enum overflows the range between its
    // smallest and largest values, but since these are |ed together, it can
    // happen. Put a sentinel in (values of this enum are stored as "unsigned
    // char").
    MO_UNUSED_MAXIMUM = 0xff
  };
} // end namespace Tiny16II

} // end namespace llvm;

#endif
