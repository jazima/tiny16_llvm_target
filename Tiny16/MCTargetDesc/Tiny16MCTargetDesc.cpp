#include "Tiny16MCTargetDesc.h"
#include "InstPrinter/Tiny16InstPrinter.h"
#include "Tiny16MCAsmInfo.h"
#include "TargetInfo/Tiny16TargetInfo.h"

#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "Tiny16GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Tiny16GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Tiny16GenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTiny16MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTiny16MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTiny16MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTiny16MCRegisterInfo(X, Tiny16::RA);
  return X;
}

static MCSubtargetInfo *createTiny16MCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU,
                                                 StringRef FS) {
  return createTiny16MCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCAsmInfo *createTiny16MCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  return new Tiny16MCAsmInfo(TT);
}

// static MCCodeGenInfo *createTiny16MCCodeGenInfo(const Triple &TT, Reloc::Model RM,
//                                              CodeModel::Model CM,
//                                              CodeGenOptLevel OL) {
//   MCCodeGenInfo *X = new MCCodeGenInfo();
//   if (RM == Reloc::Default) {
//     RM = Reloc::Static;
//   }
//   if (CM == CodeModel::Default) {
//     CM = CodeModel::Small;
//   }
//   if (CM != CodeModel::Small && CM != CodeModel::Large) {
//     report_fatal_error("Target only supports CodeModel Small or Large");
//   }

//   X->initMCCodeGenInfo(RM, CM, OL);
//   return X;
// }

static MCInstPrinter *
createTiny16MCInstPrinter(const Triple &TT, unsigned SyntaxVariant,
                       const MCAsmInfo &MAI, const MCInstrInfo &MII,
                       const MCRegisterInfo &MRI) {
  return new Tiny16InstPrinter(MAI, MII, MRI);
}

// Force static initialization.
extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTiny16TargetMC() {
  // Register the MC asm info.
  RegisterMCAsmInfoFn X(getTheTiny16Target(), createTiny16MCAsmInfo);

  // Register the MC codegen info.
//   TargetRegistry::RegisterMCCodeGenInfo(TheTiny16Target, createTiny16MCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheTiny16Target(), createTiny16MCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(getTheTiny16Target(), createTiny16MCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(getTheTiny16Target(),
                                          createTiny16MCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(getTheTiny16Target(), createTiny16MCInstPrinter);

  // Register the ASM Backend.
  TargetRegistry::RegisterMCAsmBackend(getTheTiny16Target(), createTiny16AsmBackend);

  // Register the MCCodeEmitter
  TargetRegistry::RegisterMCCodeEmitter(getTheTiny16Target(), createTiny16MCCodeEmitter);
}
