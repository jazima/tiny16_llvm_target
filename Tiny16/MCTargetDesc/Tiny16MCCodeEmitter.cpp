#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/Tiny16FixupKinds.h"
#include "MCTargetDesc/Tiny16MCTargetDesc.h"
#include "MCTargetDesc/Tiny16BaseInfo.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

using namespace llvm;

STATISTIC(MCNumEmitted, "Number of MC instructions emitted.");

namespace {
class Tiny16MCCodeEmitter : public MCCodeEmitter {
  Tiny16MCCodeEmitter(const Tiny16MCCodeEmitter &) = delete;
  void operator=(const Tiny16MCCodeEmitter &) = delete;
  const MCInstrInfo &MCII;
  const MCContext &CTX;

public:
  Tiny16MCCodeEmitter(const MCInstrInfo &mcii, MCContext &ctx)
      : MCII(mcii), CTX(ctx) {}

  ~Tiny16MCCodeEmitter() {}

  // getBinaryCodeForInstr - TableGen'erated function for getting the
  // binary encoding for an instruction.
  uint64_t getBinaryCodeForInstr(const MCInst &MI,
                                 SmallVectorImpl<MCFixup> &Fixups,
                                 const MCSubtargetInfo &STI) const;

  /// getMachineOpValue - Return binary encoding of operand. If the machine
  /// operand requires relocation, record the relocation and return zero.
  unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                             SmallVectorImpl<MCFixup> &Fixups,
                             const MCSubtargetInfo &STI) const;

  unsigned parseMEMOperand(const MCInst &MI, unsigned OpIdx,
                           SmallVectorImpl<MCFixup> &Fixups,
                           const MCSubtargetInfo &STI) const;

  void EmitByte(unsigned char C, SmallVectorImpl<char> &CB) const {
    CB.push_back((char)C);
  }

  void EmitConstant(uint64_t Val, unsigned Size,
                    SmallVectorImpl<char> &CB) const {
    // Output the constant in little endian byte order.
    for (unsigned i = 0; i != Size; ++i) {
      EmitByte(Val & 255, CB);
      Val >>= 8;
    }
  }

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;
};

} // end anonymous namespace

MCCodeEmitter *llvm::createTiny16MCCodeEmitter(const MCInstrInfo &MCII,
                                               MCContext &Ctx) {
  return new Tiny16MCCodeEmitter(MCII, Ctx);
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned
Tiny16MCCodeEmitter::getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                                       SmallVectorImpl<MCFixup> &Fixups,
                                       const MCSubtargetInfo &STI) const {
  std::cout << "[TINY16] Tiny16MCCodeEmitter::getMachineOpValue called\n";

  if (MO.isReg()) {
    return CTX.getRegisterInfo()->getEncodingValue(MO.getReg());
  }

  if (MO.isImm()) {
    return static_cast<unsigned>(MO.getImm());
  }

  assert(MO.isExpr() && "unknown operand kind in printOperand");

  const MCExpr *Expr = MO.getExpr();
  MCExpr::ExprKind Kind = Expr->getKind();

  if (Kind == MCExpr::Binary) {
    Expr = static_cast<const MCBinaryExpr *>(Expr)->getLHS();
    Kind = Expr->getKind();
  }

  assert(Kind == MCExpr::SymbolRef);

  unsigned FixupKind;
  switch (cast<MCSymbolRefExpr>(Expr)->getSpecifier()) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case Tiny16II::TOF::MO_LO8: {
    FixupKind = Tiny16::Fixups::fixup_tiny16_mov_lo8_abs;
    break;
  }
  case Tiny16II::TOF::MO_HI8: {
    FixupKind = Tiny16::Fixups::fixup_tiny16_mov_hi8_abs;
    break;
  }
  }

  Fixups.push_back(MCFixup::create(0, MO.getExpr(), MCFixupKind(FixupKind)));
  return 0;
}

unsigned
Tiny16MCCodeEmitter::parseMEMOperand(const MCInst &MI, unsigned OpIdx,
                                     SmallVectorImpl<MCFixup> &Fixups,
                                     const MCSubtargetInfo &STI) const {
  std::cout << "[TINY16] Tiny16MCCodeEmitter::parseMEMOperand called\n";
  unsigned Bits = 0;
  const MCOperand &RegMO = MI.getOperand(OpIdx);
  const MCOperand &ImmMO = MI.getOperand(OpIdx + 1);
  assert(ImmMO.getImm() >= 0);
  Bits |= (getMachineOpValue(MI, RegMO, Fixups, STI) << 16);
  Bits |= (unsigned)ImmMO.getImm() & 0xffff;
  return Bits;
}

void Tiny16MCCodeEmitter::encodeInstruction(const MCInst &MI,
                                            SmallVectorImpl<char> &CB,
                                            SmallVectorImpl<MCFixup> &Fixups,
                                            const MCSubtargetInfo &STI) const {
  std::cout << "[TINY16] Tiny16MCCodeEmitter::encodeInstruction called\n";
  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  // if (Desc.getSize() != 4) {
  //   llvm_unreachable("Unexpected instruction size!");
  // }

  const uint32_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);

  EmitConstant(Binary, Desc.getSize(), CB);
  ++MCNumEmitted;
}

#include "Tiny16GenMCCodeEmitter.inc"
