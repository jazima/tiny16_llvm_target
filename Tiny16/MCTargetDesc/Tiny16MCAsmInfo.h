#ifndef TINY16TARGETASMINFO_H
#define TINY16TARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class StringRef;
class Target;
class Triple;

class Tiny16MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit Tiny16MCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
