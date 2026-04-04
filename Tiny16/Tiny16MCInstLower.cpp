#include "Tiny16MCInstLower.h"
#include "MCTargetDesc/Tiny16BaseInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include <iostream>

using namespace llvm;

Tiny16MCInstLower::Tiny16MCInstLower(class AsmPrinter &asmprinter)
    : Printer(asmprinter) {
  std::cout << "[TINY16] Tiny16MCInstLower constructor called\n";
}

// Debug: constructor
// (no additional logic beyond initialization)

void Tiny16MCInstLower::Initialize(MCContext *C) {
  std::cout << "[TINY16] Tiny16MCInstLower::Initialize called\n";
  Ctx = C;
}

MCOperand Tiny16MCInstLower::LowerSymbolOperand(const MachineOperand &MO,
                                                MachineOperandType MOTy,
                                                unsigned Offset) const {
  std::cout << "[TINY16] Tiny16MCInstLower::LowerSymbolOperand called\n";
  const MCSymbol *Symbol;

  switch (MOTy) {
  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_GlobalAddress:
    Symbol = Printer.getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_BlockAddress:
    Symbol = Printer.GetBlockAddressSymbol(MO.getBlockAddress());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_ExternalSymbol:
    Symbol = Printer.GetExternalSymbolSymbol(MO.getSymbolName());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_JumpTableIndex:
    Symbol = Printer.GetJTISymbol(MO.getIndex());
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    Symbol = Printer.GetCPISymbol(MO.getIndex());
    Offset += MO.getOffset();
    break;
  default:
    llvm_unreachable("<unknown operand type>");
  }

  const unsigned Option = MO.getTargetFlags() & Tiny16II::MO_OPTION_MASK;
  uint16_t Kind = Tiny16II::MO_NO_FLAG;

  switch (Option) {
    default:
      break;
    case Tiny16II::MO_LO8:
      Kind = Tiny16II::MO_LO8;
      break;
    case Tiny16II::MO_HI8:
      Kind = Tiny16II::MO_HI8;
      break;
  }
  const MCSymbolRefExpr *MCSym = MCSymbolRefExpr::create(Symbol, Kind, *Ctx);

  if (!Offset) {
    return MCOperand::createExpr(MCSym);
  }

  // Assume offset is never negative.
  assert(false && "Unimplemented");
  assert(Offset > 0);

  const MCConstantExpr *OffsetExpr = MCConstantExpr::create(Offset, *Ctx);
  const MCBinaryExpr *Add = MCBinaryExpr::createAdd(MCSym, OffsetExpr, *Ctx);
  return MCOperand::createExpr(Add);
}

MCOperand Tiny16MCInstLower::LowerOperand(const MachineOperand &MO,
                                          unsigned offset) const {
  MachineOperandType MOTy = MO.getType();

  std::cout << "[TINY16] Tiny16MCInstLower::LowerOperand called\n";

  switch (MOTy) {
  default:
    llvm_unreachable("unknown operand type");
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit()) {
      break;
    }
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + offset);
  case MachineOperand::MO_MachineBasicBlock:
  case MachineOperand::MO_GlobalAddress:
  case MachineOperand::MO_ExternalSymbol:
  case MachineOperand::MO_JumpTableIndex:
  case MachineOperand::MO_ConstantPoolIndex:
  case MachineOperand::MO_BlockAddress:
    return LowerSymbolOperand(MO, MOTy, offset);
  case MachineOperand::MO_RegisterMask:
    break;
  }

  return MCOperand();
}

void Tiny16MCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
  std::cout << "[TINY16] Tiny16MCInstLower::Lower called\n";
  OutMI.setOpcode(MI->getOpcode());

  for (auto &MO : MI->operands()) {
    const MCOperand MCOp = LowerOperand(MO);

    if (MCOp.isValid()) {
      OutMI.addOperand(MCOp);
    }
  }
}
