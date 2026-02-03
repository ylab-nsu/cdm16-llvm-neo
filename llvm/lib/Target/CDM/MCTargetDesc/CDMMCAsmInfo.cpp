//
// Created by ilya on 15.10.23.
//

#include "CDMMCAsmInfo.h"

namespace llvm {
CDMMCAsmInfo::CDMMCAsmInfo(const Triple &TheTriple) {
  CodePointerSize = 2;
  MaxInstLength = 4;
  MinInstAlignment = 2;
  PrivateGlobalPrefix = "__L";
  PrivateLabelPrefix = "__L";
}
} // namespace llvm
