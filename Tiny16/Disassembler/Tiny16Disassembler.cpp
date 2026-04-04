//===- Tiny16Disassembler.cpp - Disassembler for Tiny16 -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is part of the Tiny16 Disassembler.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Tiny16MCTargetDesc.h"
#include "TargetInfo/Tiny16TargetInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoder.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::MCD;

#define DEBUG_TYPE "tiny16-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// A disassembler class for Tiny16.
class Tiny16Disassembler : public MCDisassembler {
public:
  Tiny16Disassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  ~Tiny16Disassembler() override = default;

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
}

static MCDisassembler *createTiny16Disassembler(const Target &T,
                                               const MCSubtargetInfo &STI,
                                               MCContext &Ctx) {
  return new Tiny16Disassembler(STI, Ctx);
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeTiny16Disassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(getTheTiny16Target(),
                                         createTiny16Disassembler);
}

// clang-format off
static constexpr unsigned GPRRegDecoderTable[] = {
  Tiny16::R0,  Tiny16::R1,  Tiny16::R2,  Tiny16::R3,
  Tiny16::R4,  Tiny16::R5,  Tiny16::R6,  Tiny16::R7,
  Tiny16::R8,  Tiny16::R9,  Tiny16::R10, Tiny16::RR,
  Tiny16::RCMP, Tiny16::FP, Tiny16::RA, Tiny16::PC};
// clang-format on

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, unsigned RegNo,
                                            uint64_t Address,
                                            const MCDisassembler *Decoder) {
  if (RegNo >= std::size(GPRRegDecoderTable))
    return MCDisassembler::Fail;
  unsigned Reg = GPRRegDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

#include "Tiny16GenDisassemblerTables.inc"

/// Read two bytes from the ArrayRef and return 16 bit word.
static DecodeStatus readInstruction16(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint16_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 2 Bytes of data.
  if (Bytes.size() < 2) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Size = 2;
  Insn = support::endian::read<uint16_t>(
      Bytes.data(), IsLittleEndian ? endianness::little : endianness::big);
  return MCDisassembler::Success;
}

DecodeStatus Tiny16Disassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                               ArrayRef<uint8_t> Bytes,
                                               uint64_t Address,
                                               raw_ostream &CStream) const {
  uint16_t Insn;
  bool isLittleEndian = getContext().getAsmInfo()->isLittleEndian();
  DecodeStatus Result =
      readInstruction16(Bytes, Address, Size, Insn, isLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Calling the auto-generated decoder function.
  Result = decodeInstruction(DecoderTable16, Instr, Insn, Address, this, STI);

  return Result;
}
