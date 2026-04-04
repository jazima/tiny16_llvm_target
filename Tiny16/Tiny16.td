//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Descriptions
//===----------------------------------------------------------------------===//

include "Tiny16RegisterInfo.td"
include "Tiny16InstrInfo.td"
include "Tiny16CallingConv.td"

defm : RemapAllTargetPseudoPointerOperands<GPR>;

def Tiny16InstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Tiny16 processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

def Tiny16 : Target {
  let InstructionSet = Tiny16InstrInfo;
}
