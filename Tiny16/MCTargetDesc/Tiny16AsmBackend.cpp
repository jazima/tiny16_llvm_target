#include "MCTargetDesc/Tiny16MCTargetDesc.h"
#include "MCTargetDesc/Tiny16FixupKinds.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
// class Tiny16ELFObjectWriter : public MCELFObjectTargetWriter {
// public:
//   Tiny16ELFObjectWriter(uint8_t OSABI)
//       : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, /*ELF::EM_Tiny16*/ ELF::EM_ARM,
//                                 /*HasRelocationAddend*/ false) {}
// };

class Tiny16AsmBackend : public MCAsmBackend {
public:
  Tiny16AsmBackend() : MCAsmBackend(llvm::endianness::little) {} // TODO: verify endianness

  ~Tiny16AsmBackend() {}

  unsigned getNumFixupKinds() const {
    return Tiny16::NumTargetFixupKinds;
  }

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[Tiny16::NumTargetFixupKinds] = {
      // This table *must* be in the order that the fixup_* kinds are defined in
      // Tiny16FixupKinds.h.
      //
      // Name                      Offset (bits) Size (bits)     Flags
      { "fixup_tiny16_mov_hi8_pcrel", 0, 8, 0 },
      { "fixup_tiny16_mov_lo8_pcrel", 0, 8, 0 },
    };
    return Infos[0];
  }

  void applyFixup(const MCFragment &, const MCFixup &,
                  const MCValue &Target, uint8_t *Data, uint64_t Value,
                  bool IsResolved) override;

  bool mayNeedRelaxation(unsigned Opcode, ArrayRef<MCOperand> Operands,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  bool fixupNeedsRelaxationAdvanced(const MCFragment &, const MCFixup &,
                                    const MCValue &, uint64_t,
                                    bool Resolved) const override {
    return false;
  }

  void relaxInstruction(MCInst &Inst, const MCSubtargetInfo &STI) const override {}

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                          const MCSubtargetInfo *STI) const override {
    if (Count == 0) {
      return true;
    }
    return false;
  }

  unsigned getPointerSize() const { return 4; }
};
} // end anonymous namespace

static unsigned adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext *Ctx = NULL) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case Tiny16::Fixups::fixup_tiny16_mov_hi8_abs:
    Value >>= 8;
  case Tiny16::Fixups::fixup_tiny16_mov_lo8_abs:
    Value >>= 1; // Shift by one to align address to 16 bits
    Value = (Value & 0xFF) << 4; // {4'{reg}, 8'{imm}, 4'{opcode}}
    return Value;
  }
}

void Tiny16AsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                 const MCValue &Target, uint8_t *Data,
                                 uint64_t Value, bool IsResolved) {
  
  // // Force reloc emission for symbolic Tiny16 target fixups.
  // if (Target.getAddSym() || Target.getSubSym())
  //   IsResolved = false;

  maybeAddReloc(F, Fixup, Target, Value, IsResolved);

  const uint32_t DataSize = F.getFixedSize();
  unsigned NumBytes = 2;
  Value = adjustFixupValue(Fixup, Value);
  if (!Value) {
    return; // Doesn't change encoding.
  }

  //unsigned Offset = Fixup.getOffset();
  //assert(Offset + NumBytes <= DataSize && "Invalid fixup offset!");

  for (unsigned i = 0; i != NumBytes; ++i) {
    Data[i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

namespace {

class ELFTiny16AsmBackend : public Tiny16AsmBackend {
public:
  Triple::OSType OSType;
  ELFTiny16AsmBackend(const MCSubtargetInfo &STI, Triple::OSType OSType)
      : Tiny16AsmBackend(), OSType(OSType) {}

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(OSType);
    return createTiny16ELFObjectWriter(OSABI);
  }
};

} // end anonymous namespace

MCAsmBackend *llvm::createTiny16AsmBackend(const Target &T,
                                     const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options) {
  return new ELFTiny16AsmBackend(STI, STI.getTargetTriple().getOS());
}