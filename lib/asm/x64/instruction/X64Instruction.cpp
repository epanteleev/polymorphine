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

    std::ostream& operator<<(std::ostream &os, const Lea &lea) {
        return os << "leaq " << lea.m_src << ", %" << lea.m_dst.name(8);
    }

    std::ostream& operator<<(std::ostream &os, const PopR &popr) {
        return os << "pop" << prefix_size(popr.m_size) << " %" << popr.m_reg.name(popr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const PopM &popm) {
        return os << "pop" << prefix_size(popm.m_size) << ' ' << popm.m_addr;
    }

    std::ostream& operator<<(std::ostream& os, const NegR& negr) {
        return os << "neg" << prefix_size(negr.m_size) << " %" << negr.m_src.name(negr.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const NegM &negm) {
        return os << "neg" << prefix_size(negm.m_size) << ' ' << negm.m_src;
    }

    std::ostream& operator<<(std::ostream &os, const UDivR& idiv) {
        return os << "div" << prefix_size(idiv.m_size) << " %" << idiv.m_divisor.name(idiv.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const UDivM &idiv) {
        return os << "div" << prefix_size(idiv.m_size) << ' ' << idiv.m_divisor;
    }

    std::ostream& operator<<(std::ostream &os, const IdivR& idiv) {
        return os << "idiv" << prefix_size(idiv.m_size) << " %"
               << idiv.m_divisor.name(idiv.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const IdivM &idiv) {
        return os << "idiv" << prefix_size(idiv.m_size) << ' ' << idiv.m_divisor;
    }

    std::ostream& operator<<(std::ostream &os, const PushR &pushr) {
        return os << "push" << prefix_size(pushr.m_size) << " %" << pushr.m_reg.name(pushr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const PushM &pushm) {
        return os << "push" << prefix_size(pushm.m_size) << ' ' << pushm.m_addr;
    }

    std::ostream& operator<<(std::ostream &os, const PushI &pushi) {
        return os << "push" << prefix_size(pushi.m_size) << " $" << pushi.m_imm;
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
        return os << "mov" << prefix_size(movrr.m_size) << " %" << movrr.m_src.name(movrr.m_size) << ", %" << movrr.m_dest.name(movrr.m_size);
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
        return os << "mov" << prefix_size(movmr.m_size) << " %" << movmr.m_src.name(movmr.m_size) << ", " << movmr.m_dest;
    }

    std::ostream& operator<<(std::ostream &os, const MovRM &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " " << movrm.m_src << ", %" << movrm.m_dest.name(movrm.m_size);
    }

    std::ostream & operator<<(std::ostream &os, const MovMI &movrm) {
        return os << "mov" << prefix_size(movrm.m_size) << " $" << movrm.m_src << ", " << movrm.m_dest;
    }

    std::ostream& operator<<(std::ostream &os, const AddRR& add) {
        return os << "add" << prefix_size(add.m_size) << " %"
              << add.m_src.name(add.m_size) << ", %"
              << add.m_dst.name(add.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const AddMR& add) {
        return os << "add" << prefix_size(add.m_size) << " %" << add.m_src.name(add.m_size) << ", " << add.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const AddRI& add) {
        return os << "add" << prefix_size(add.m_size) << " $" << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const AddMI& add) {
        return os << "add" << prefix_size(add.m_size) << " $" << add.m_src << ", " << add.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const AddRM& add) {
        return os << "add" << prefix_size(add.m_size) << " " << add.m_src << ", %" << add.m_dst.name(add.m_size);
    }

    std::ostream& operator<<(std::ostream& os, const SubRR& subrr) {
        return os << "sub" << prefix_size(subrr.m_size) << " %"
               << subrr.m_src.name(subrr.m_size) << ", %"
               << subrr.m_dst.name(subrr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SubMR &submr) {
        return os << "sub" << prefix_size(submr.m_size) << " %"
               << submr.m_src.name(submr.m_size) << ", " << submr.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const SubRI &subrr) {
        return os << "sub" << prefix_size(subrr.m_size) << " $"
               << subrr.m_src << ", %" << subrr.m_dst.name(subrr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SubMI &submi) {
        return os << "sub" << prefix_size(submi.m_size) << " $"
               << submi.m_src << ", " << submi.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const SubRM &subrm) {
        return os << "sub" << prefix_size(subrm.m_size) << " " << subrm.m_src << ", %" << subrm.m_dst.name(subrm.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRR& cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %"
              << cmp.m_src.name(cmp.m_size) << ", %"
              << cmp.m_dst.name(cmp.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMR& cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " %" << cmp.m_src.name(cmp.m_size) << ", " << cmp.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const CmpRI& cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << " $" << cmp.m_imm << ", %" << cmp.m_dst.name(cmp.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMI& cmp) {
        return os << "cmp" << prefix_size(cmp.m_size)
            << " $" << cmp.m_imm << ", " << cmp.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const CmpRM& cmp) {
        return os << "cmp" << prefix_size(cmp.m_size) << ' ' << cmp.m_src << ", %" << cmp.m_dst.name(cmp.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const XorRR& xorrr) {
        return os << "xor" << prefix_size(xorrr.m_size) << " %" << xorrr.m_src.name(xorrr.m_size) << ", %" << xorrr.m_dst.name(xorrr.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const XorMR& xormr) {
        return os << "xor" << prefix_size(xormr.m_size) << " %" << xormr.m_src.name(xormr.m_size) << ", " << xormr.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const XorRI& xorri) {
        return os << "xor" << prefix_size(xorri.m_size) << " $" << xorri.m_src << ", %" << xorri.m_dst.name(xorri.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const XorMI& xormi) {
        return os << "xor" << prefix_size(xormi.m_size) << " $" << xormi.m_src << ", " << xormi.m_dst;
    }

    std::ostream& operator<<(std::ostream &os, const XorRM& xorrm) {
        return os << "xor" << prefix_size(xorrm.m_size) << ' ' << xorrm.m_src << ", %" << xorrm.m_dst.name(xorrm.m_size);
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