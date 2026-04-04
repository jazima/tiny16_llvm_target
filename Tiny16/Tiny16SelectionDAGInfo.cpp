#include "Tiny16SelectionDAGInfo.h"

#define GET_SDNODE_DESC
#include "Tiny16GenSDNodeInfo.inc"

#include <iostream>

using namespace llvm;

Tiny16SelectionDAGInfo::Tiny16SelectionDAGInfo()
        : SelectionDAGGenTargetInfo(Tiny16GenSDNodeInfo) {
    std::cout << "[TINY16] Tiny16SelectionDAGInfo constructor called\n";
}
    
Tiny16SelectionDAGInfo::~Tiny16SelectionDAGInfo() {
    std::cout << "[TINY16] Tiny16SelectionDAGInfo destructor called\n";
}
