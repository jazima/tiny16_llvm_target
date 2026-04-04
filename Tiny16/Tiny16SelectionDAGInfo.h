#ifndef TINY16SELECTIONDAGINFO_H
#define TINY16SELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

#define GET_SDNODE_ENUM
#include "Tiny16GenSDNodeInfo.inc"

namespace llvm {

class Tiny16SelectionDAGInfo : public SelectionDAGGenTargetInfo {
public:
  Tiny16SelectionDAGInfo();
  ~Tiny16SelectionDAGInfo() override;
};

}

#endif
