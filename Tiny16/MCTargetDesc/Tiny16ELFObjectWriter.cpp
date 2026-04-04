#include "MCTargetDesc/Tiny16MCTargetDesc.h"
#include "MCTargetDesc/Tiny16FixupKinds.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    class Tiny16ELFObjectWriter : public MCELFObjectTargetWriter {
    public:
        Tiny16ELFObjectWriter(uint8_t OSABI);

        ~Tiny16ELFObjectWriter();

    protected:
        unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                              bool IsPCRel) const override;
    };
}

unsigned Tiny16ELFObjectWriter::getRelocType(const MCFixup &Fixup,
                                             const MCValue &Target,
                                             bool IsPCRel) const {
    if (IsPCRel) {
        llvm_unreachable("Do not support PC-relative fixups for now");
    }

    unsigned Type = 0;
    switch ((unsigned)Fixup.getKind()) {
    default:
        llvm_unreachable("Unimplemented");
    case Tiny16::Fixups::fixup_tiny16_mov_hi8_abs:
        Type = ELF::R_ARM_MOVT_ABS; // R_ARM_MOVT_ABS is used for the high 8 bits of a 16-bit absolute address
        break;
    case Tiny16::Fixups::fixup_tiny16_mov_lo8_abs:
        Type = ELF::R_ARM_MOVW_ABS_NC; // R_ARM_MOVW_ABS_NC is used for the low 8 bits of a 16-bit absolute address
        break;
    }
    return Type;
}

Tiny16ELFObjectWriter::Tiny16ELFObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(/*Is64Bit*/ false, OSABI, /*ELF::EM_Tiny16*/ ELF::EM_TINY16,
                              /*HasRelocationAddend*/ false) {}

Tiny16ELFObjectWriter::~Tiny16ELFObjectWriter() {}

std::unique_ptr<MCObjectTargetWriter>
llvm::createTiny16ELFObjectWriter(uint8_t OSABI) {
    return std::make_unique<Tiny16ELFObjectWriter>(OSABI);
}
