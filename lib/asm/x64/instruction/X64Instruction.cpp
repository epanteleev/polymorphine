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

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const Address& addr, const GPReg reg) {
        return os << name << prefix_size(size) << ' ' << addr << ", %" << reg.name(size);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm, const Address& addr) {
        return os << name << prefix_size(size) << " $" << imm << ", " << addr;
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm, const GPReg reg) {
        return os << name << prefix_size(size) << " $" << imm << ", %" << reg.name(size);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg0, const GPReg reg) {
        return os << name << prefix_size(size) << " %" << reg0.name(size) << ", %" << reg.name(size);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg0, const Address& addr) {
        return os << name << prefix_size(size) << " %" << reg0.name(size) << ", " << addr;
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const GPReg reg) {
        return os << name << prefix_size(size) << " %" << reg.name(size);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const Address& addr) {
        return os << name << prefix_size(size) << ' ' << addr;
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const std::size_t size, const std::int64_t imm) {
        return os << name << prefix_size(size) << " $" << imm;
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const XmmReg reg0, const XmmReg reg) {
        return os << name << " %" << reg0.name(16) << ", %" << reg.name(16);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const Address& addr, const XmmReg reg) {
        return os << name << ' ' << addr << ", %" << reg.name(16);
    }

    static std::ostream& print_to(std::ostream& os, const std::string_view name, const XmmReg reg, const Address& addr) {
        return os << name << " %" << reg.name(16) << ", " << addr;
    }

    std::ostream& operator<<(std::ostream &os, const Lea &lea) {
        return print_to(os, "lea", 8, lea.m_src, lea.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const PopR &popr) {
        return print_to(os, "pop", popr.m_size, popr.m_reg);
    }

    std::ostream& operator<<(std::ostream &os, const PopM &popm) {
        return print_to(os, "pop", popm.m_size, popm.m_addr);
    }

    std::ostream& operator<<(std::ostream& os, const NegR& negr) {
        return print_to(os, "neg", negr.m_size, negr.m_src);
    }

    std::ostream& operator<<(std::ostream& os, const NegM &negm) {
        return print_to(os, "neg", negm.m_size, negm.m_src);
    }

    std::ostream& operator<<(std::ostream &os, const UDivR& idiv) {
        return print_to(os, "div", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const UDivM &idiv) {
        return print_to(os, "div", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const IdivR& idiv) {
        return print_to(os, "idiv", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const IdivM &idiv) {
        return print_to(os, "idiv", idiv.m_size, idiv.m_divisor);
    }

    std::ostream& operator<<(std::ostream &os, const PushR &pushr) {
        return print_to(os, "push", pushr.m_size, pushr.m_reg);
    }

    std::ostream& operator<<(std::ostream &os, const PushM &pushm) {
        return print_to(os, "push", pushm.m_size, pushm.m_addr);
    }

    std::ostream& operator<<(std::ostream &os, const PushI &pushi) {
        return print_to(os, "push", pushi.m_size, pushi.m_imm);
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
        return print_to(os, "mov", movrr.m_size, movrr.m_src, movrr.m_dest);
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
        return print_to(os, "mov", movmr.m_size, movmr.m_src, movmr.m_dest);
    }

    std::ostream& operator<<(std::ostream &os, const MovRM &movrm) {
        return print_to(os, "mov", movrm.m_size, movrm.m_src, movrm.m_dest);
    }

    std::ostream & operator<<(std::ostream &os, const MovMI &movrm) {
        return print_to(os, "mov", movrm.m_size, movrm.m_src, movrm.m_dest);
    }

    std::ostream& operator<<(std::ostream &os, const AddRR& add) {
        return print_to(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddMR& add) {
        return print_to(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddRI& add) {
        return print_to(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddMI& add) {
        return print_to(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AddRM& add) {
        return print_to(os, "add", add.m_size, add.m_src, add.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubRR& subrr) {
        return print_to(os, "sub", subrr.m_size, subrr.m_src, subrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubMR &submr) {
        return print_to(os, "sub", submr.m_size, submr.m_src, submr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubRI &subrr) {
        return print_to(os, "sub", subrr.m_size, subrr.m_src, subrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubMI &submi) {
        return print_to(os, "sub", submi.m_size, submi.m_src, submi.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SubRM &subrm) {
        return print_to(os, "sub", subrm.m_size, subrm.m_src, subrm.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRR& cmp) {
        return print_to(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMR& cmp) {
        return print_to(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRI& cmp) {
        return print_to(os, "cmp", cmp.m_size, cmp.m_imm, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpMI& cmp) {
        return print_to(os, "cmp", cmp.m_size, cmp.m_imm, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const CmpRM& cmp) {
        return print_to(os, "cmp", cmp.m_size, cmp.m_src, cmp.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRR& xorrr) {
        return print_to(os, "xor", xorrr.m_size, xorrr.m_src, xorrr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorMR& xormr) {
        return print_to(os, "xor", xormr.m_size, xormr.m_src, xormr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRI& xorri) {
        return print_to(os, "xor", xorri.m_size, xorri.m_src, xorri.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorMI& xormi) {
        return print_to(os, "xor", xormi.m_size, xormi.m_src, xormi.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const XorRM& xorrm) {
        return print_to(os, "xor", xorrm.m_size, xorrm.m_src, xorrm.m_dst);
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
        return print_to(os, "movss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const MovssRM& rr) {
        return print_to(os, "movss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const MovssMR& rr) {
        return print_to(os, "movss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const MovsdRR& rr) {
        return print_to(os, "movsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const MovsdRM& rr) {
        return print_to(os, "movsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const MovsdMR& rr) {
        return print_to(os, "movsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const AddssRR& rr) {
        return print_to(os, "addss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const AddssRM& rr) {
        return print_to(os, "addss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const AddsdRR& rr) {
        return print_to(os, "addsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const AddsdRM& rr) {
        return print_to(os, "addsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubssRR& rr) {
        return print_to(os, "subss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubssRM& rr) {
        return print_to(os, "subss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubsdRR& rr) {
        return print_to(os, "subsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const SubsdRM& rr) {
        return print_to(os, "subsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const UcomisdRR& rr) {
        return print_to(os, "ucomisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const UcomisdRM& rr) {
        return print_to(os, "ucomisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const UcomissRR& rr) {
        return print_to(os, "ucomisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const UcomissRM& rr) {
        return print_to(os, "ucomisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const ComissRR& rr) {
        return print_to(os, "comiss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const ComissRM& rr) {
        return print_to(os, "comiss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const ComisdRR& rr) {
        return print_to(os, "comisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const ComisdRM& rr) {
        return print_to(os, "comisd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const XorpsRR& rr) {
        return print_to(os, "xorps", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const XorpsRM& rr) {
        return print_to(os, "xorps", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const XorpdRR& rr) {
        return print_to(os, "xorpd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const XorpdRM& rr) {
        return print_to(os, "xorpd", rr.m_src, rr.m_dst);
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
        return print_to(os, "cvtsi2ss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2sdRR& rr) {
        return os << "cvtsi2sd %" << rr.m_src.name(rr.m_size) << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream& os, const Cvtsi2sdRM& rr) {
        return os << "cvtsi2sd " << rr.m_src << ", %" << rr.m_dst.name(16);
    }

    std::ostream& operator<<(std::ostream &os, const SalRI& sal) {
        return print_to(os, "sal", sal.m_size, sal.m_src, sal.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SalMI& sal) {
        return print_to(os, "sal", sal.m_size, sal.m_src, sal.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SalRR& sal) {
        const auto rcx_size = sal.m_size > 2 ? 2 : sal.m_size;
        return os << "sal" << prefix_size(sal.m_size) << " %" << rcx.name(rcx_size) << ", %" << sal.m_dst.name(sal.m_size);
    }

    std::ostream& operator<<(std::ostream &os, const SarRI& sar) {
        return print_to(os, "sar", sar.m_size, sar.m_src, sar.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const SarMI& sar) {
        return print_to(os, "sar", sar.m_size, sar.m_src, sar.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const ShrRI& shr) {
        return print_to(os, "shr", shr.m_size, shr.m_src, shr.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const ShrMI& shr) {
        return print_to(os, "shr", shr.m_size, shr.m_src, shr.m_dst);
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
        return print_to(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestMR& test) {
        return print_to(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestRI& test) {
        return print_to(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestMI& test) {
        return print_to(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const TestRM& test) {
        return print_to(os, "test", test.m_size, test.m_src, test.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AndRR& aand) {
        return print_to(os, "and", aand.m_size, aand.m_src, aand.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AndMR& aand) {
        return print_to(os, "and", aand.m_size, aand.m_src, aand.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AndRI& aand) {
        return print_to(os, "and", aand.m_size, aand.m_src, aand.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AndMI& aand) {
        return print_to(os, "and", aand.m_size, aand.m_src, aand.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const AndRM& aand) {
        return print_to(os, "and", aand.m_size, aand.m_src, aand.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const OrRR& oor) {
        return print_to(os, "or", oor.m_size, oor.m_src, oor.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const OrMR& oor) {
        return print_to(os, "or", oor.m_size, oor.m_src, oor.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const OrRI& oor) {
        return print_to(os, "or", oor.m_size, oor.m_src, oor.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const OrMI& oor) {
        return print_to(os, "or", oor.m_size, oor.m_src, oor.m_dst);
    }

    std::ostream& operator<<(std::ostream &os, const OrRM& oor) {
        return print_to(os, "or", oor.m_size, oor.m_src, oor.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const DivssRR& rr) {
        return print_to(os, "divss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const DivssRM& rr) {
        return print_to(os, "divss", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const DivsdRR& rr) {
        return print_to(os, "divsd", rr.m_src, rr.m_dst);
    }

    std::ostream& operator<<(std::ostream& os, const DivsdRM& rr) {
        return print_to(os, "divsd", rr.m_src, rr.m_dst);
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