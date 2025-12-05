#include "X64Instruction.h"
#include <ostream>

namespace aasm::details {
    std::ostream & operator<<(std::ostream &out, const Cdq &cdq) {
        switch (cdq.m_size) {
            case 2: return out << "cwtd";
            case 4: return out << "cdtq";
            case 8: return out << "cqto";
            default: die("Invalid operand size for cdq instruction: {}", static_cast<unsigned>(cdq.m_size));
        }
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const Address& addr, const GPReg reg) {
        return os << name << prefix_size(size) << ' ' << addr << ", %" << reg.name(size);
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm, const Address& addr) {
        return os << name << prefix_size(size) << " $" << imm << ", " << addr;
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm, const GPReg reg) {
        return os << name << prefix_size(size) << " $" << imm << ", %" << reg.name(size);
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg0, const GPReg reg) {
        return os << name << prefix_size(size) << " %" << reg0.name(size) << ", %" << reg.name(size);
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg0, const Address& addr) {
        return os << name << prefix_size(size) << " %" << reg0.name(size) << ", " << addr;
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg) {
        return os << name << prefix_size(size) << " %" << reg.name(size);
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const Address& addr) {
        return os << name << prefix_size(size) << ' ' << addr;
    }

    static std::ostream& print(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm) {
        return os << name << prefix_size(size) << " $" << imm;
    }

    std::ostream& operator<<(std::ostream &os, const Lea &lea) {
        return print(os, "lea", 8, lea.m_src, lea.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const PopR &popr) {
        return print(os, "pop", popr.m_size, popr.m_reg);
    }

    std::ostream& operator<<(std::ostream &os, const PopM &popm) {
        return print(os, "pop", popm.m_size, popm.m_addr);
    }

    std::ostream& operator<<(std::ostream& os, const NegR& negr) {
        return print(os, "neg", negr.m_size, negr.m_src);
    }

    std::ostream& operator<<(std::ostream& os, const NegM &negm) {
        return print(os, "neg", negm.m_size, negm.m_src);
    }

    std::ostream& operator<<(std::ostream &os, const UDivR& idiv) {
        return print(os, "div", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const UDivM &idiv) {
        return print(os, "div", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const IdivR& idiv) {
        return print(os, "idiv", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const IdivM &idiv) {
        return print(os, "idiv", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const PushR &pushr) {
        return print(os, "push", pushr.m_size, pushr.m_reg);
    }

    std::ostream& operator<<(std::ostream &os, const PushM &pushm) {
        return print(os, "push", pushm.m_size, pushm.m_addr);
    }

    std::ostream& operator<<(std::ostream &os, const PushI &pushi) {
        return print(os, "push", pushi.m_size, pushi.m_imm);
    }

    std::ostream& operator<<(std::ostream &os, const Ret&) {
        return os << "ret";
    }

    std::ostream& operator<<(std::ostream& os, const CMovRR& inst) {
        return os << "cmov" << inst.m_cond << " %" << inst.m_src.name(inst.m_size) << ", %" << inst.m_dst.name(inst.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const CMovRM& inst) {
        return os << "cmov" << inst.m_cond << " " << inst.m_src << ", %" << inst.m_dst.name(inst.m_size);
    }

    std::ostream & operator<<(std::ostream &os, const MovRR &movrr) {
        return print(os, "mov", movrr.m_size, movrr.m_src, movrr.m_dest);
    }

    std::ostream & operator<<(std::ostream &os, const MovRI &movri) {
        if (movri.m_size == 8) {
            os << "movabs";
        } else {
            os << "mov";
        }

        return os << prefix_size(movri.m_size) << " $" << movri.m_src << ", %" << movri.m_dest.name(movri.m_size);
    }

    std::ostream & operator<<(std::ostream &os, const MovMR &movmr) {
        return print(os, "mov", movmr.m_size, movmr.m_src, movmr.m_dest);
    }

    std::ostream& operator<<(std::ostream &os, const MovRM &movrm) {
        return print(os, "mov", movrm.m_size, movrm.m_src, movrm.m_dest);
    }

    std::ostream & operator<<(std::ostream &os, const MovMI &movrm) {
        return print(os, "mov", movrm.m_size, movrm.m_src, movrm.m_dest);
    }

    std::ostream& operator<<(std::ostream &os, const AddRR& add) {
        return print(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddMR& add) {
        return print(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddRI& add) {
        return print(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddMI& add) {
        return print(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddRM& add) {
        return print(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubRR& subrr) {
        return print(os, "sub", subrr.m_size, subrr.m_src, subrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubMR &submr) {
        return print(os, "sub", submr.m_size, submr.m_src, submr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubRI &subrr) {
        return print(os, "sub", subrr.m_size, subrr.m_src, subrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubMI &submi) {
        return print(os, "sub", submi.m_size, submi.m_src, submi.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubRM &subrm) {
        return print(os, "sub", subrm.m_size, subrm.m_src, subrm.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRR& cmp) {
        return print(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMR& cmp) {
        return print(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRI& cmp) {
        return print(os, "cmp", cmp.m_size, cmp.m_imm, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMI& cmp) {
        return print(os, "cmp", cmp.m_size, cmp.m_imm, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRM& cmp) {
        return print(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRR& xorrr) {
        return print(os, "xor", xorrr.m_size, xorrr.m_src, xorrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorMR& xormr) {
        return print(os, "xor", xormr.m_size, xormr.m_src, xormr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRI& xorri) {
        return print(os, "xor", xorri.m_size, xorri.m_src, xorri.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorMI& xormi) {
        return print(os, "xor", xormi.m_size, xormi.m_src, xormi.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRM& xorrm) {
        return print(os, "xor", xorrm.m_size, xorrm.m_src, xorrm.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const MovzxRR& movzxrr) {
        return os << "movz" << prefix_size(movzxrr.m_from_size)
            << prefix_size(movzxrr.m_to_size)
            << " %" << movzxrr.m_src.name(movzxrr.m_from_size)
            << ", %" << movzxrr.m_dest.name(movzxrr.m_to_size);
    }

    std::ostream& operator<<(std::ostream &os, const MovzxRM& movzxrm) {
        return os << "movz" << prefix_size(movzxrm.m_from_size)
            << prefix_size(movzxrm.m_to_size)
            << " " << movzxrm.m_src
            << ", %" << movzxrm.m_dest.name(movzxrm.m_to_size);
    }

    std::ostream& operator<<(std::ostream &os, const MovsxRR& movsxrr) {
        return os << "movs" << prefix_size(movsxrr.m_from_size)
            << prefix_size(movsxrr.m_to_size)
            << " %" << movsxrr.m_src.name(movsxrr.m_from_size)
            << ", %" << movsxrr.m_dest.name(movsxrr.m_to_size);
    }

    std::ostream& operator<<(std::ostream &os, const MovsxRM& movsxrm) {
        return os << "movs" << prefix_size(movsxrm.m_from_size)
            << prefix_size(movsxrm.m_to_size)
            << " " << movsxrm.m_src
            << ", %" << movsxrm.m_dest.name(movsxrm.m_to_size);
    }

    std::ostream& operator<<(std::ostream &os, const MovsxdRR& movsxrr) {
        return os << "movs" << prefix_size(movsxrr.m_from_size)
            << prefix_size(movsxrr.m_to_size)
            << " %" << movsxrr.m_src.name(movsxrr.m_from_size)
            << ", %" << movsxrr.m_dest.name(movsxrr.m_to_size);
    }

    std::ostream& operator<<(std::ostream &os, const MovsxdRM& movsxrm) {
        return os << "movs" << prefix_size(movsxrm.m_from_size)
            << prefix_size(movsxrm.m_to_size)
            << " " << movsxrm.m_src
            << ", %" << movsxrm.m_dest.name(movsxrm.m_to_size);
    }

    std::ostream &operator<<(std::ostream &os, const Jmp &jmp) {
        return os << "jmp " << jmp.m_label;
    }

    std::ostream & operator<<(std::ostream &os, const Jcc &jcc) {
        return os << "j" << jcc.m_type << ' ' << jcc.m_label;
    }

    std::ostream& operator<<(std::ostream& os, const SetCCR& set) {
        return os << "set" << set.m_cond << " %" << set.m_reg.name(1);
    }

    std::ostream &operator<<(std::ostream &os, const Call &call) {
        return os << "call " << call.m_name->name();
    }

    std::ostream & operator<<(std::ostream &os, const CallM &call) {
        return os << "call " << call.m_addr;
    }

    std::ostream &operator<<(std::ostream &os, const Leave &) {
        return os << "leave";
    }

    std::ostream& operator<<(std::ostream& os, const MovssRR& rr) {
        return os << "movss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const MovssRM& rr) {
        return os << "movss " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const MovssMR& rr) {
        return os << "movss %" << rr.m_src.name(16) << ", " << rr.m_dst;
    }

    std::ostream& operator<<(std::ostream& os, const MovsdRR& rr) {
        return os << "movsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const MovsdRM& rr) {
        return os << "movsd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const MovsdMR& rr) {
        return os << "movsd %" << rr.m_src.name(16) << ", " << rr.m_dst;
    }

    std::ostream& operator<<(std::ostream& os, const AddssRR& rr) {
        return os << "addss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const AddssRM& rr) {
        return os << "addss " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const AddsdRR& rr) {
        return os << "addsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const AddsdRM& rr) {
        return os << "addsd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const SubssRR& rr) {
        return os << "subss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const SubssRM& rr) {
        return os << "subss " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const SubsdRR& rr) {
        return os << "subsd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const SubsdRM& rr) {
        return os << "subsd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const UcomisdRR& rr) {
        return os << "ucomisd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const UcomisdRM& rr) {
        return os << "ucomisd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const UcomissRR& rr) {
        return os << "ucomiss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const UcomissRM& rr) {
        return os << "ucomiss " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const ComissRR& rr) {
        return os << "comiss %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const ComissRM& rr) {
        return os << "comiss " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const ComisdRR& rr) {
        return os << "comisd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const ComisdRM& rr) {
        return os << "comisd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const XorpsRR& rr) {
        return os << "xorps %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const XorpsRM& rr) {
        return os << "xorps " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const XorpdRR& rr) {
        return os << "xorpd %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const XorpdRM& rr) {
        return os << "xorpd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtss2siRR& rr) {
        return os << "cvttss2si" << prefix_size(rr.m_size) << " %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(rr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtss2siRM& rr) {
        return os << "cvttss2si" << prefix_size(rr.m_size) << " %" << rr.m_src << ", %" << rr.m_dst.name(rr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsd2siRR& rr) {
        return os << "cvttsd2si" << prefix_size(rr.m_size) << " %" << rr.m_src.name(16) << ", %" << rr.m_dst.name(rr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsd2siRM& rr) {
        return os << "cvttsd2si" << prefix_size(rr.m_size) << " %" << rr.m_src << ", %" << rr.m_dst.name(rr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2ssRR& rr) {
        return os << "cvtsi2ss %" << rr.m_src.name(rr.m_size) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2ssRM& rr) {
        return os << "cvtsi2ss " << rr.m_src << ", %" << rr.m_dst.name(rr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2sdRR& rr) {
        return os << "cvtsi2sd %" << rr.m_src.name(rr.m_size) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2sdRM& rr) {
        return os << "cvtsi2sd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream &os, const SalRI& sal) {
        return os << "sal" << prefix_size(sal.m_size) << " $" << static_cast<unsigned>(sal.m_src) << ", %" << sal.m_dst.name(sal.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SalMI& sal) {
        return os << "sal" << prefix_size(sal.m_size) << " $" << static_cast<unsigned>(sal.m_src) << ", " << sal.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const SalRR& sal) {
        const auto rcx_size = sal.m_size > 2 ? 2 : sal.m_size;
        return os << "sal" << prefix_size(sal.m_size) << " %" << rcx.name(rcx_size) << ", %" << sal.m_dst.name(sal.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SarRI& sar) {
        return os << "sar" << prefix_size(sar.m_size) << " $" << static_cast<unsigned>(sar.m_src) << ", %" << sar.m_dst.name(sar.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SarMI& sar) {
        return os << "sal" << prefix_size(sar.m_size) << " $" << static_cast<unsigned>(sar.m_src) << ", " << sar.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const ShrRI& shr) {
        return os << "shr" << prefix_size(shr.m_size) << " $" << static_cast<unsigned>(shr.m_src) << ", %" << shr.m_dst.name(shr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const ShrMI& shr) {
        return os << "shr" << prefix_size(shr.m_size) << " $" << static_cast<unsigned>(shr.m_src) << ", " << shr.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const SarRR& sar) {
        const auto rcx_size = sar.m_size > 2 ? 2 : sar.m_size;
        return os << "sal" << prefix_size(sar.m_size) << " %" << rcx.name(rcx_size) << ", %" << sar.m_dst.name(sar.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const ShrRR& sar) {
        const auto rcx_size = sar.m_size > 2 ? 2 : sar.m_size;
        return os << "shr" << prefix_size(sar.m_size) << " %" << rcx.name(rcx_size) << ", %" << sar.m_dst.name(sar.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const TestRR& test) {
        return print(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestMR& test) {
        return print(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestRI& test) {
        return print(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestMI& test) {
        return print(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestRM& test) {
        return print(os, "test", test.m_size, test.m_src, test.m_dst);
    }
}

namespace aasm {
    std::ostream& operator<<(std::ostream &os, const X64Instruction &inst) {
        const auto visitor = [&](const auto &var) {
            os << var;
        };

        std::visit(visitor, inst);
        return os;
    }
}