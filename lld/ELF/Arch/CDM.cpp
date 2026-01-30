#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "Thunks.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Endian.h"

#include <iostream>

using namespace llvm;
using namespace llvm::support::endian;
using namespace llvm::support;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;
using namespace llvm::object;

namespace {

class CDM final: public TargetInfo {
public:
    CDM(Ctx &ctx);
    RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
    void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
    int64_t getImplicitAddend(const uint8_t *buf,
                RelType type) const override;
};
}

CDM::CDM(Ctx &ctx): TargetInfo(ctx) {
    defaultImageBase = 0x00000;
}

RelExpr CDM::getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const {
    switch (type) {
    case R_CDM_16:
        return R_ABS;
    default:
        Err(ctx) << getErrorLoc(ctx, loc) << "unknown relocation (" << type.v
                << ") against symbol " << &s;
    return R_NONE;
    }
}

void CDM::relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const {
    switch (rel.type) {
    case R_CDM_16:
        checkUInt(ctx, loc, val, 16, rel);
        write16le(loc, val);
        break;
    default:
        llvm_unreachable("unknown relocation");
    }
}

int64_t CDM::getImplicitAddend(const uint8_t *buf,
                RelType type) const {
    switch (type) {
    case R_CDM_16:
        return SignExtend64<16>(read16le(buf));
    default:
        InternalErr(ctx, buf) << "cannot read addend for relocation " << type;
        return 0;
    }
}

void elf::setCDMTargetInfo(Ctx &ctx) { ctx.target.reset(new CDM(ctx)); }

