#ifndef TINY16MCTARGETDESC_H
#define TINY16MCTARGETDESC_H

#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class Target;
class MCInstrInfo;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCContext;
class MCCodeEmitter;
class MCAsmInfo;
class MCCodeGenInfo;
class MCInstPrinter;
class MCObjectTargetWriter;
class MCAsmBackend;
class MCTargetOptions;

class StringRef;
class raw_ostream;
class raw_pwrite_stream;
class Triple;

extern Target TheTiny16Target;

MCCodeEmitter *createTiny16MCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);

MCAsmBackend *createTiny16AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createTiny16ELFObjectWriter(uint8_t OSABI);

} // End llvm namespace

// Defines symbolic names for Tiny16 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "Tiny16GenRegisterInfo.inc"

// Defines symbolic names for the Tiny16 instructions.
//
#define GET_INSTRINFO_ENUM
#include "Tiny16GenInstrInfo.inc"

#endif