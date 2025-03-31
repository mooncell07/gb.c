#include "cpu.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "bus.h"
#include "logger.h"
#include "mmio.h"
#include "registers.h"
#include "types.h"

CPU createCPU(Bus *bus) {
    CPU cpu = {.bus = bus,
               .opcode = 0x00,
               .halted = false,
               .IME = false,
               .IMERising = false};
    return cpu;
}

void getSerialOutput(CPU *self) {
    if (readByte(self->bus, 0xFF02, false, true) == 0x81) {
        printf("%c", (char)readByte(self->bus, 0xFF01, false, true));
        writeByte(self->bus, 0xFF02, 0, false);
    }
}

void reportInvalidOpcode(CPU *self) {
    // Leaving P and Q masks because they are always fully covered by the
    // switch/if statements.
    char msg[80];
    snprintf(msg, sizeof(msg), "Invalid Opcode: $%X @ (X:%d Z:%d Y:%d) ",
             self->opcode, xMask(self->opcode), zMask(self->opcode),
             yMask(self->opcode));
    logState(FATAL, msg);
}

uint8_t fetch(CPU *self) {
    uint8_t res = readByte(self->bus, pc, true, true);
    pc++;
    return res;
}
uint16_t fetchWord(CPU *self) {
    uint8_t lo = fetch(self);
    uint8_t hi = fetch(self);
    return JOIN(hi, lo);
}

uint16_t pop(CPU *self) {
    uint8_t lo = readByte(self->bus, sp, true, true);
    sp++;
    uint8_t hi = readByte(self->bus, sp, true, true);
    sp++;
    return JOIN(hi, lo);
}

void push(CPU *self, uint16_t data) {
    sp--;
    writeByte(self->bus, sp, MSB(data), true);
    sp--;
    writeByte(self->bus, sp, LSB(data), true);
}

void jump(CPU *self, uint16_t address, bool internalCycle) {
    if (internalCycle) {
        internal(self->bus);
    }

    push(self, pc);
    pc = address;
}

void opLDr_r(CPU *self, R8Type src, R8Type dest) {
    setReg8(self->bus, src, getReg8(self->bus, dest));
}

void opLDr_n(CPU *self, R8Type reg) { setReg8(self->bus, reg, fetch(self)); }

void opLDr_nn(CPU *self, R16Type reg) { setReg16(reg, fetchWord(self)); }

void opLDr_addr(CPU *self, R8Type reg, uint16_t address) {
    setReg8(self->bus, reg, readByte(self->bus, address, true, true));
}

void opLDaddr_r(CPU *self, uint16_t address, R8Type reg) {
    writeByte(self->bus, address, getReg8(self->bus, reg), true);
}

void opLDRP_word(CPU *self, R16Type reg, uint16_t data) {
    setReg16(reg, data);
    internal(self->bus);
}

void opLDaddr_SP(CPU *self) {
    uint16_t nn = fetchWord(self);
    writeByte(self->bus, nn, LSB(sp), true);
    writeByte(self->bus, nn + 1, MSB(sp), true);
}

void opPUSHR16(CPU *self, R16Type reg, bool internalCycles) {
    if (internalCycles) {
        internal(self->bus);
    }

    push(self, getReg16(reg));
}

void opPUSHDATA(CPU *self, uint16_t data, bool internalCycles) {
    if (internalCycles) {
        internal(self->bus);
    }
    push(self, data);
}

void opPOP(CPU *self, R16Type reg) { setReg16(reg, pop(self)); }

void opCALL(CPU *self, uint16_t address, bool internalCycles) {
    jump(self, address, internalCycles);
}

void opJP(CPU *self, uint16_t address, bool hl) {
    pc = address;
    if (!hl) {
        internal(self->bus);
    }
}

void opJR(CPU *self) {
    int8_t offset = (int8_t)fetch(self);
    pc += offset;
    internal(self->bus);
}

void opJRcond(CPU *self, CCType cond) {
    int8_t offset = ((int8_t)fetch(self));
    if (getCC(cond)) {
        pc += (uint16_t)offset;
        internal(self->bus);
    }
}

void opRET(CPU *self, uint16_t address) {
    pc = address;
    internal(self->bus);
}

void opRETcond(CPU *self, CCType cond) {
    if (getCC(cond)) {
        pc = pop(self);
        internal(self->bus);
    }
    internal(self->bus);
}

void opRETI(CPU *self) {
    self->IME = true;
    opRET(self, pop(self));
}

void opSTOP(CPU *self) {
    pc++;
    self->halted = true;
}

void alu(CPU *self, AluOp op, uint8_t data) {
    uint8_t acc = getReg8(self->bus, A);
    uint8_t res = 0;

    switch (op) {
    case OR:
        res = acc | data;
        setN(false);
        setH(false);
        setC(false);
        break;

    case CP:
        res = acc - data;
        setN(true);
        setH(checkHalfBorrow(acc, data));
        setC(data > acc);
        break;

    case AND:
        res = acc & data;
        setN(false);
        setH(true);
        setC(false);
        break;

    case XOR:
        res = acc ^ data;
        setN(false);
        setH(false);
        setC(false);
        break;

    case ADD:
        res = acc + data;
        setN(false);
        setH(checkHalfCarry(acc, data));
        setC(acc > res);
        break;

    case SUB:
        res = acc - data;
        setN(true);
        setH(checkHalfBorrow(acc, data));
        setC(data > acc);
        break;

    case ADC:
        res = acc + data + (uint8_t)getFlag(ftC);
        setN(false);
        setH(((acc & 0xF) + (data & 0xF) + (uint8_t)getFlag(ftC)) > 0xF);
        setC(((uint16_t)acc + (uint16_t)data + (uint16_t)getFlag(ftC)) > 0xFF);
        break;

    case SBC:
        res = acc - (uint8_t)getFlag(ftC) - data;
        setN(true);
        setH(((data & 0xF) + (uint8_t)getFlag(ftC)) > (acc & 0xF));
        setC(((uint16_t)data + (uint8_t)getFlag(ftC)) > acc);
        break;
    }

    setZ(res == 0);

    if (op != CP) {
        setReg8(self->bus, A, res);
    }
}

void opINC8(CPU *self, R8Type reg) {
    uint8_t value = getReg8(self->bus, reg);
    uint8_t res = value + 1;
    setZ(res == 0);
    setN(false);
    setH(checkHalfCarry(value, 1));
    setReg8(self->bus, reg, res);
}

void opINC16(CPU *self, R16Type reg) {
    uint16_t value = getReg16(reg);
    uint16_t res = value + 1;
    internal(self->bus);
    setReg16(reg, res);
}

void opDEC8(CPU *self, R8Type reg) {
    uint8_t value = getReg8(self->bus, reg);
    uint8_t res = value - 1;
    setZ(res == 0);
    setN(true);
    setH(checkHalfBorrow(value, 1));
    setReg8(self->bus, reg, res);
}

void opDEC16(CPU *self, R16Type reg) {
    uint16_t value = getReg16(reg);
    uint16_t res = value - 1;
    internal(self->bus);
    setReg16(reg, res);
}

void opADDHL_RP(CPU *self, R16Type reg) {
    uint16_t hl = getReg16(HL);
    uint16_t data = getReg16(reg);

    setReg16(HL, hl + data);
    setN(false);
    setH((hl & 0xFFF) + (data & 0xFFF) > 0xFFF);
    setC(((uint32_t)hl + (uint32_t)data) > 0xFFFF);

    internal(self->bus);
}

void opADDSP_i8(CPU *self, bool internals) {
    uint16_t data = (uint16_t)((int8_t)fetch(self));
    uint16_t res = sp + data;

    setZ(false);
    setN(false);
    setH((sp & 0xF) + (data & 0xF) > 0xF);
    setC((sp & 0xFF) + (data & 0xFF) > 0xFF);

    if (internals) {
        internal(self->bus);
        internal(self->bus);
    }
    sp = res;
}

void prefixHandler(CPU *self) {
    uint8_t op = fetch(self);
    R8Type reg = (R8Type)(zMask(op));

    uint8_t res = 0;

    switch (xMask(op)) {
    case 0x0:
        PrefixOp rot = (PrefixOp)(yMask(op));
        uint8_t u8 = getReg8(self->bus, reg);

        switch (rot) {
        case RL:
            res = (u8 << 1) | getFlag(ftC);
            setC((u8 >> 7) != 0);
            break;

        case RR:
            res = (u8 >> 1) | (getFlag(ftC) << 7);
            setC((u8 & 1) == 1);
            break;

        case RLC:
            setC((u8 >> 7) != 0);
            res = rotateLeftBits(u8, 1) | getFlag(ftC);
            break;
        case RRC:
            setC((u8 & 1) == 1);
            res = rotateRightBits(u8, 1) | (0x80 & getFlag(ftC));
            break;
        case SLA:
            res = u8 << 1;
            setC((u8 >> 7) != 0);
            break;
        case SRL:
            res = u8 >> 1;
            setC((u8 & 1) == 1);
            break;
        case SRA:
            res = (u8 >> 1) | (u8 & 0x80);
            setC((u8 & 1) == 1);
            break;
        case SWAP:
            res = rotateLeftBits(u8, 4);
            setC(false);
            break;
        }
        break;

    case 0x1:
        uint8_t val = getReg8(self->bus, reg);
        setZ(!BT(val, yMask(op)));
        setN(false);
        setH(true);
        break;
    case 0x2:
        res = getReg8(self->bus, reg);
        clearBit(res, yMask(op));
        break;
    case 0x3:
        res = getReg8(self->bus, reg);
        setBit(res, yMask(op));

        break;
    default:
        break;
    }

    if (xMask(op) != 1) {
        setReg8(self->bus, reg, res);
        if (xMask(op) == 0) {
            setZ(res == 0);
            setN(false);
            setH(false);
        }
    }
}

void opDAA(CPU *self) {
    uint8_t value = getReg8(self->bus, A);
    if (!getFlag(ftN)) {
        if (getFlag(ftH) | ((getReg8(self->bus, A) & 0x0F) > 9)) {
            value += 6;
        }

        if (getFlag(ftC) | (getReg8(self->bus, A) > 0x99)) {
            value += 0x60;
            setC(true);
        }
    } else {
        if (getFlag(ftC)) {
            value -= 0x60;
        }

        if (getFlag(ftH)) {
            value -= 6;
        }
    }
    setZ(value == 0);
    setH(false);
    setReg8(self->bus, A, value);
}

void opRLA(CPU *self) {
    uint8_t acc = getReg8(self->bus, A);
    uint8_t data = (acc << 1) | (getFlag(ftC) ? 1 : 0);

    setC((acc >> 7) != 0);
    setZ(false);
    setN(false);
    setH(false);
    setReg8(self->bus, A, data);
}

void opRLCA(CPU *self) {
    uint8_t acc = getReg8(self->bus, A);
    setC((acc >> 7) != 0);
    uint8_t data = rotateLeftBits(acc, 1) | (getFlag(ftC) ? 1 : 0);
    setZ(false);
    setN(false);
    setH(false);
    setReg8(self->bus, A, data);
}

void opRRA(CPU *self) {
    uint8_t u8 = getReg8(self->bus, A);
    uint8_t res = (u8 >> 1) | (getFlag(ftC) << 7);
    setZ(false);
    setN(false);
    setH(false);
    setC((u8 & 1) == 1);
    setReg8(self->bus, A, res);
}

void opRRCA(CPU *self) {
    uint8_t acc = getReg8(self->bus, A);
    setC((acc & 1) == 1);
    uint8_t data = rotateRightBits(acc, 1) | (0x80 & (getFlag(ftC) ? 1 : 0));
    setZ(false);
    setN(false);
    setH(false);
    setReg8(self->bus, A, data);
}

void cpuTick(CPU *self) {
    self->opcode = fetch(self);
    if (self->opcode == 0xCB) {
        prefixHandler(self);
        return;
    }

    switch (xMask(self->opcode)) {
    case 0x0:
        switch (zMask(self->opcode)) {
        case 0x0:

            switch (yMask(self->opcode)) {
            case 0x0:
                break;
            case 0x1:
                opLDaddr_SP(self);
                break;
            case 0x2:
                opSTOP(self);
                break;
            case 0x3:
                opJR(self);
                break;
            case 0x4:
            case 0x5:
            case 0x6:
            case 0x7:
                opJRcond(self, (CCType)(yMask(self->opcode) - 4));
                break;
            default:
                reportInvalidOpcode(self);
                break;
            }

            break;

        case 0x1:
            if (!qMask(self->opcode)) {
                opLDr_nn(self, (R16Type)pMask(self->opcode));
            } else {
                opADDHL_RP(self, (R16Type)pMask(self->opcode));
            }

            break;

        case 0x2:
            if (!qMask(self->opcode)) {
                switch (pMask(self->opcode)) {
                case 0x0:
                    opLDaddr_r(self, getReg16(BC), A);
                    break;
                case 0x1:
                    opLDaddr_r(self, getReg16(DE), A);
                    break;
                case 0x2: {
                    uint16_t data = getReg16(HL);
                    opLDaddr_r(self, data, A);
                    setReg16(HL, data + 1);
                    break;
                }
                case 0x3: {
                    uint16_t data = getReg16(HL);
                    opLDaddr_r(self, data, A);
                    setReg16(HL, data - 1);
                    break;
                }
                }
            } else {
                switch (pMask(self->opcode)) {
                case 0x0:
                    opLDr_addr(self, A, getReg16(BC));
                    break;
                case 0x1:
                    opLDr_addr(self, A, getReg16(DE));
                    break;
                case 0x2: {
                    uint16_t data = getReg16(HL);
                    opLDr_addr(self, A, data);
                    setReg16(HL, data + 1);
                    break;
                }
                case 0x3:
                    uint16_t data = getReg16(HL);
                    opLDr_addr(self, A, data);
                    setReg16(HL, data - 1);
                    break;
                }
            }

            break;

        case 0x3:
            if (!qMask(self->opcode)) {
                opINC16(self, (R16Type)pMask(self->opcode));
            } else {
                opDEC16(self, (R16Type)pMask(self->opcode));
            }

            break;

        case 0x4:
            opINC8(self, (R8Type)(yMask(self->opcode)));
            break;
        case 0x5:
            opDEC8(self, (R8Type)(yMask(self->opcode)));
            break;
        case 0x6:
            opLDr_n(self, (R8Type)(yMask(self->opcode)));
            break;
        case 0x7:
            switch (yMask(self->opcode)) {
            case 0x0:
                opRLCA(self);
                break;
            case 0x1:
                opRRCA(self);
                break;
            case 0x2:
                opRLA(self);
                break;
            case 0x3:
                opRRA(self);
                break;
            case 0x4:
                opDAA(self);
                break;
            case 0x5:
                setReg8(self->bus, A, ~getReg8(self->bus, A));
                setN(true);
                setH(true);
                break;
            case 0x6:
                setC(true);
                setN(false);
                setH(false);
                break;
            case 0x7:
                setC(!getFlag(ftC));
                setN(false);
                setH(false);
                break;
            default:
                reportInvalidOpcode(self);
                break;
            }
            break;
        default:
            reportInvalidOpcode(self);
            break;
        }
        break;

    case 0x1:
        if ((zMask(self->opcode) == 0x6) && (yMask(self->opcode) == 0x6)) {
            self->halted = true;
        } else {
            opLDr_r(self, (R8Type)yMask(self->opcode),
                    (R8Type)zMask(self->opcode));
        }
        break;

    case 0x2:
        alu(self, (AluOp)(yMask(self->opcode)),
            getReg8(self->bus, (R8Type)zMask(self->opcode)));
        break;

    case 0x3:
        switch (zMask(self->opcode)) {
        case 0x0:
            switch (yMask(self->opcode)) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                opRETcond(self, (CCType)yMask(self->opcode));
                break;
            case 0x4:
                opLDaddr_r(self, 0xFF00 + fetch(self), A);
                break;
            case 0x5:
                opADDSP_i8(self, true);
                break;
            case 0x6:
                opLDr_addr(self, A, 0xFF00 + fetch(self));
                break;
            case 0x7: {
                uint16_t old_sp = sp;
                opADDSP_i8(self, false);
                opLDRP_word(self, HL, sp);
                sp = old_sp;
                break;
            }
            }
            break;
        case 0x1:
            if (!qMask(self->opcode)) {
                opPOP(self, group2adjust(pMask(self->opcode)));
            } else {
                switch (pMask(self->opcode)) {
                case 0x0:
                    opRET(self, pop(self));
                    break;
                case 0x1:
                    opRETI(self);
                    break;
                case 0x2:
                    opJP(self, getReg16(HL), true);
                    break;
                case 0x3:
                    opLDRP_word(self, SP, getReg16(HL));
                    break;
                }
            }
            break;

        case 0x2:
            switch (yMask(self->opcode)) {
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3: {
                uint16_t nn = fetchWord(self);
                if (getCC((CCType)yMask(self->opcode))) {
                    opJP(self, nn, false);
                }
                break;
            }
            case 0x4:
                opLDaddr_r(self, 0xFF00 + getReg8(self->bus, C), A);
                break;
            case 0x5:
                opLDaddr_r(self, fetchWord(self), A);
                break;
            case 0x6:
                opLDr_addr(self, A, 0xFF00 + getReg8(self->bus, C));
                break;
            case 0x7:
                opLDr_addr(self, A, fetchWord(self));
                break;
            }
            break;

        case 0x3:
            switch (yMask(self->opcode)) {
            case 0x0:
                opJP(self, fetchWord(self), false);
                break;
            case 0x6:
                self->IME = false;
                break;
            case 0x7:
                self->IMERising = true;
                break;
            default:
                reportInvalidOpcode(self);
                break;
            }
            break;

        case 0x4: {
            uint16_t nn = fetchWord(self);
            if (getCC((CCType)yMask(self->opcode))) {
                opCALL(self, nn, true);
            }
            break;
        }
        case 0x5:
            if (!qMask(self->opcode)) {
                opPUSHR16(self, group2adjust(pMask(self->opcode)), true);
            } else {
                if (pMask(self->opcode) == 0) {
                    opCALL(self, fetchWord(self), true);
                } else {
                    reportInvalidOpcode(self);
                }
            }
            break;
        case 0x6:
            alu(self, (AluOp)yMask(self->opcode), fetch(self));
            break;
        case 0x7:
            opCALL(self, yMask(self->opcode) * 8, true);
            break;
        }
        break;

    default:
        reportInvalidOpcode(self);
        break;
    }
}

const uint16_t IVT[5] = {0x40, 0x48, 0x50, 0x58, 0x60};

void serviceIRQ(CPU *self, IntType intT) {
    self->halted = false;
    self->IME = false;
    self->IMERising = false;
    clearBit(self->bus->mmio->IF, intT);
    jump(self, IVT[intT], false);
}

void handlePendingIRQs(CPU *self) {
    for (int i = 0; i < 5; i++) {
        if (BT(self->bus->mmio->IF, i) && BT(self->bus->mmio->IE, i)) {
            serviceIRQ(self, (IntType)i);
        }
    }
}

void checkPendingIRQs(CPU *self) {
    if (self->bus->mmio->IF == 0) {
        return;
    }
    self->halted = false;
    if (self->IME) {
        handlePendingIRQs(self);
    }
}
