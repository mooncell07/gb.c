import json
from dataclasses import dataclass
from io import StringIO

from jinja2 import Environment, FileSystemLoader

# The keywords `src`, `r1`, `result`, all registers, flags  and C types, keywords
# are implicitly recognised by both templates and this script.

# FUNCTION DEFAULTS
DEFAULT_SEPARATOR = "_"
DEFAULT_MODIFIERS = ["static"]
DEFAULT_DISCRIMINATOR = "op"
DEFAULT_RET_TYPE = "void"
DEFAULT_REGS_VAR = "regs"
DEFAULT_STRUCT_NAME = "State"
DEFAULT_INSTANCE_NAME = "self"
DEFAULT_PARAMS = {DEFAULT_STRUCT_NAME: DEFAULT_INSTANCE_NAME}
DEFAULT_BODY = "UNIMPLEMENTED"

# ARGUMENT GROUPS
ARG_REGISTERS = ("A", "B", "C", "D", "E", "H", "L")
ARG_REGISTER_PAIRS = ("AF", "BC", "DE", "HL")
ARG_REGISTER_PAIRS_DEREFS = ("(HL)", "(BC)", "(DE)")
ARG_DEREF_INTS = "(U16)"
ARG_INTS = ("U16", "U8", "I8")

# ADDRESSING MODES
AM_NONE = "NONE"
AM_REGISTER = "R"
AM_REGISTER_REGISTER = AM_REGISTER + DEFAULT_SEPARATOR + AM_REGISTER
AM_REGISTER_IMMEDIATE = AM_REGISTER + DEFAULT_SEPARATOR + "N"
AM_REGISTERPAIR = 2 * AM_REGISTER
AM_REGISTERPAIR_REGISTERPAIR = AM_REGISTERPAIR + DEFAULT_SEPARATOR + AM_REGISTERPAIR
AM_REGISTERPAIR_IMMEDIATE16 = AM_REGISTERPAIR + DEFAULT_SEPARATOR + "NN"
AM_REGISTER_DEREF_REGISTERPAIR = (
    AM_REGISTER + DEFAULT_SEPARATOR + "DEREF" + DEFAULT_SEPARATOR + AM_REGISTERPAIR
)
AM_DEREF_REGISTERPAIR = "DEREF" + DEFAULT_SEPARATOR + AM_REGISTERPAIR
AM_DEREF_REGISTERPAIR_REGISTER = AM_DEREF_REGISTERPAIR + DEFAULT_SEPARATOR + AM_REGISTER
AM_U3_REGISTER = "U3" + DEFAULT_SEPARATOR + AM_REGISTER
AM_U3_DEREF_REGISTERPAIR = (
    "U3" + DEFAULT_SEPARATOR + "DEREF" + DEFAULT_SEPARATOR + AM_REGISTERPAIR
)
AM_DEREF_REGISTERPAIR_IMMEDIATE = AM_DEREF_REGISTERPAIR + DEFAULT_SEPARATOR + "N"

# INFO KEYS
KEY_LHS = "lhs"
KEY_RHS = "rhs"
KEY_KIND = "kind"
KEY_C_OPR = "c_op"
KEY_PREFORMED = "preformed"
KEY_INTERNAL = "internal"
KEY_Z_FLAG = "z_flag"
KEY_N_FLAG = "n_flag"
KEY_H_FLAG = "h_flag"
KEY_C_FLAG = "c_flag"
KEY_OP = "op"
KEY_WB = "write_back"
KEY_CC = "cc"

# FLAGS
FLAG_C = "ftC"
FLAG_Z = "ftZ"
FLAG_H = "ftH"
FLAG_N = "ftN"

# INSTANCE FIELDS
HALTED = "halted"
IME_RISING = "IMERising"
IME = "IME"

# JINJA TEMPLATES
TEMPLATE_ALU = "alu"
TEMPLATE_LSM = "lsm"
TEMPLATE_RSB = "rsb"
TEMPLATE_STACK = "stack"
TEMPLATE_JUMP = "control_br_jump"
TEMPLATE_CALL = "control_br_call"
TEMPLATE_PREFORMED = "preformed"

# SLOTS
SLOT_HANDLERS = "HANDLERS"
SLOT_PREFIXED = "PREFIXED_LUT"
SLOT_UNPREFIXED = "UNPREFIXED_LUT"

EMPTY = ""


AM_TABLE = {
    (ARG_REGISTERS, ARG_REGISTERS): AM_REGISTER_REGISTER,
    (ARG_REGISTER_PAIRS, ARG_REGISTER_PAIRS): AM_REGISTERPAIR_REGISTERPAIR,
    (ARG_REGISTER_PAIRS, ARG_INTS): AM_REGISTERPAIR_IMMEDIATE16,
    (ARG_REGISTERS, ARG_INTS): AM_REGISTER_IMMEDIATE,
    (ARG_REGISTER_PAIRS_DEREFS, ARG_INTS): AM_DEREF_REGISTERPAIR_IMMEDIATE,
    (ARG_REGISTERS, ARG_REGISTER_PAIRS_DEREFS): AM_REGISTER_DEREF_REGISTERPAIR,
    (ARG_REGISTER_PAIRS_DEREFS, ARG_REGISTERS): AM_DEREF_REGISTERPAIR_REGISTER,
    (ARG_REGISTERS, EMPTY): AM_REGISTER,
    (ARG_REGISTER_PAIRS, EMPTY): AM_REGISTERPAIR,
    (ARG_REGISTER_PAIRS_DEREFS, EMPTY): AM_DEREF_REGISTERPAIR,
}

ALU_OP_MAPPING = {
    "ADD": "+",
    "ADC": "+",
    "SUB": "-",
    "SBC": "-",
    "AND": "&",
    "OR": "|",
    "XOR": "^",
    "CP": "-",
    "INC": "+",
    "DEC": "-",
    "LD": "+",
}

ALU_FLAG_MAPPING = {
    "ADD": {
        "h": "(r1 & 0xF) + (src & 0xF) >= 0x10",
        "c": "r1 > result",
    },
    "SUB": {
        "h": "(r1 & 0xF) < (src & 0xF)",
        "c": "src > r1",
    },
    "ADC": {
        "h": "((r1 & 0xF) + (src & 0xF) + c) > 0xF",
        "c": "((uint16_t)r1 + (uint16_t)src + (uint16_t)c) > 0xFF",
    },
    "SBC": {
        "h": "((src & 0xF) + c) > (r1 & 0xF)",
        "c": "((uint16_t)src + c) > r1",
    },
    "ADD16": {
        "h": "(r1 & 0xFFF) + (src & 0xFFF) > 0xFFF",
        "c": "((uint32_t)r1 + (uint32_t)src) > 0xFFFF",
    },
    "ADDi8": {
        "h": "(r1 & 0xF) + (src & 0xF) >= 0x10",
        "c": "(r1 & 0xFF) + (src & 0xFF) > 0xFF",
    },
}

MISC_PREFORMED_LIST = [
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "NOP",
    "HALT",
    "STOP",
    "EI",
    "DI",
    "RETI",
    "ADD_SP_I8",
    "LD_HL_SP_I8",
    "JP_HL",
]

LOAD_PREFORMED_LIST = [
    "LD_A_deref_FF00_U8",
    "LD_A_deref_FF00_C",
    "LD_deref_FF00_U8_A",
    "LD_deref_FF00_C_A",
    "LD_deref_HLi_A",
    "LD_deref_HLd_A",
    "LD_A_deref_HLi",
    "LD_A_deref_HLd",
    "LD_A_deref_U16",
    "LD_deref_U16_A",
    "LD_deref_U16_SP",
]


@dataclass
class Flags:
    z_flag: str | None = None
    n_flag: str | None = None
    h_flag: str | None = None
    c_flag: str | None = None


def get_x8_ALU_flags(mnemonic: str) -> Flags:
    # We group the flags in two categories based on the nature of their operation.
    flag_group = "SUB" if mnemonic in ("CP", "DEC", "SBC", "SUB") else "ADD"

    # All ALU Ops use same Z Flag operation.
    z = "!result"

    # N Flag only goes up for SUB group of operations.
    n = "true" if flag_group == "SUB" else "false"

    # H is down on (OR, XOR), up on AND and an operation in others.
    h = "true" if mnemonic == "AND" else "false"

    # Get H Flag operation for ADD, SUB, ADC, SBC
    # INC, DEC, CP share same operation so we send a group instead.
    # This saves us from explictly putting their entries in the flag mapping.
    try:
        h = ALU_FLAG_MAPPING[
            mnemonic if mnemonic not in ("INC", "DEC", "CP") else flag_group
        ]["h"]
    except KeyError:
        pass

    # C Flag is down in the case of AND, OR and XOR, None in case of INC, DEC and an operation in others.
    c = "false" if mnemonic in ("AND", "OR", "XOR") else None

    # Get the C Flag operation for ADD, SUB, ADC, SBC
    # Same thing happens here except INC, DEC are left unaffected.
    try:
        c = ALU_FLAG_MAPPING[mnemonic if mnemonic != "CP" else "SUB"]["c"]
    except KeyError:
        pass

    return Flags(z, n, h, c)


def get_x16_ALU_flags(mnemonic: str, signed: bool = False) -> Flags:
    if mnemonic in ("INC", "DEC"):
        return Flags()

    z = "false" if signed else None
    n = "false"
    # LD has same C and H Flag Operations as ADD
    mnemonic = "ADD" if mnemonic == "LD" else mnemonic
    # Postfixes to distinguish between x8 and x16 flags
    mnemonic += "i8" if signed else "16"
    h = ALU_FLAG_MAPPING[mnemonic]["h"]
    c = ALU_FLAG_MAPPING[mnemonic]["c"]

    return Flags(z, n, h, c)


def get_RSB_flags(mnemonic: str) -> Flags:
    # RES and SET dont affect any flag.
    if mnemonic in ("RES", "SET"):
        return Flags()

    z = "!result"

    # These operations unset the Z Flag.
    if mnemonic in ("RLCA", "RRCA", "RLA", "RRA"):
        z = "false"

    # N Flag is always down on every RSB instr.
    n = "false"

    # H Flag is always down except in the case of BIT
    h = "true" if mnemonic == "BIT" else "false"

    c = None

    if mnemonic in ("RL", "RLC", "SLA", "RLA", "RLCA"):
        c = "r1 >> 7"
    elif mnemonic in ("RR", "RRC", "SRL", "SRA", "RRA", "RRCA"):
        c = "r1 & 1"
    elif mnemonic == "SWAP":
        c = "false"

    return Flags(z, n, h, c)


def replace_deref_parens(arg: str) -> str:
    # Replaces (RP) with deref_RP for making the value identifier valid.
    return (
        arg.replace("(", "deref_")
        .replace(")", "")
        .replace("HL+", "HLi")
        .replace("HL-", "HLd")
        .replace("+", "_")
        .replace("-", "_")
    )


def underef_rp(arg: str) -> str:
    # Replaces (RP) with RPp for using it as a valid Register Pair.
    return arg.replace("(", "").replace(")", "p")


def underef_offset(arg: str) -> str:
    return arg.replace("(", "").replace(")", "")


class ExternalFunction:
    def __init__(self, instance: str) -> None:
        self.instance = instance

    def gen_call_getFlag(self, flag_arg: str) -> str:
        return f"getFlag({flag_arg})"

    def gen_call_read8(self, arg: str) -> str:
        return f"read8({self.instance}->bus, {arg}, true, true)"

    def gen_call_write8(self, address_arg: str, value_arg: str) -> str:
        return f"write8({self.instance}->bus, {address_arg}, {value_arg}, true)"

    def gen_call_write16(self, address_arg: str, value_arg: str) -> str:
        return f"write16({self.instance}->bus, {address_arg}, {value_arg})"

    def gen_call_next_imm8(self) -> str:
        return f"next8({self.instance})"

    def gen_call_next_imm16(self) -> str:
        return f"next16({self.instance})"

    def gen_call_internal(self) -> str:
        return f"internal({self.instance}->bus)"

    def gen_call_push(self, value_arg: str) -> str:
        return f"push({self.instance}, {value_arg})"

    def gen_call_pop(self) -> str:
        return f"pop({self.instance})"


class MiscOperation:
    def __init__(self, extern: ExternalFunction, instance: str) -> None:
        self.instance = instance
        self.extern = extern

    def gen_NOP(self) -> list[str]:
        return ["return;"]

    def gen_HALT(self) -> list[str]:
        return [f"{self.instance}->{HALTED} = true;"]

    def gen_STOP(self) -> list[str]:
        return self.gen_HALT()

    def gen_EI(self) -> list[str]:
        return [f"{self.instance}->{IME_RISING} = true;"]

    def gen_DI(self) -> list[str]:
        return [f"{self.instance}->{IME} = false;"]

    def gen_RETI(self) -> list[str]:
        # opRET is auto generated earlier so this should be fine.
        return [f"{self.gen_EI()[0]}", f"opRET({self.instance});"]

    def gen_JP_HL(self) -> list[str]:
        return ["PC = HLp;"]

    def gen_CPL(self) -> list[str]:
        return ["A = ~A;", "Nf(true);", "Hf(true);"]

    def gen_CCF(self) -> list[str]:
        return [
            "Nf(false);",
            "Hf(false);",
            f"Cf(~{self.extern.gen_call_getFlag(FLAG_C)});",
        ]

    def gen_SCF(self) -> list[str]:
        return ["Nf(false);", "Hf(false);", "Cf(true);"]

    def __handle_SP_I8(self, wb_reg: str) -> list[str]:
        body_code = [
            f"uint16_t r1 = SP, src = (uint16_t)(int8_t)({self.extern.gen_call_next_imm8()});",
            "uint16_t result = r1 + src;",
            f"{wb_reg} = result;",
            self.extern.gen_call_internal() + ";",
        ]
        flags = get_x16_ALU_flags("LD", signed=True)
        flag_code = [
            f"Zf({flags.z_flag});",
            f"Nf({flags.n_flag});",
            f"Hf({flags.h_flag});",
            f"Cf({flags.c_flag});",
        ]
        body_code.extend(flag_code)
        return body_code

    def gen_LD_HL_SP_I8(self) -> list[str]:
        return self.__handle_SP_I8("HLp")

    def gen_ADD_SP_I8(self) -> list[str]:
        body = self.__handle_SP_I8("SP")
        body.append(self.extern.gen_call_internal() + ";")
        return body

    def gen_DAA(self) -> list[str]:
        return [
            "uint8_t a = A;",
            f"if (!{self.extern.gen_call_getFlag(FLAG_N)}) {{",
            f"if ({self.extern.gen_call_getFlag(FLAG_H)} | ((A & 0x0F) > 9)) {{ a += 6; }}",
            f"if ({self.extern.gen_call_getFlag(FLAG_C)} | (A > 0x99)) {{",
            "a += 0x60;",
            "Cf(true);",
            "}",
            "} else {",
            f"if ({self.extern.gen_call_getFlag(FLAG_C)}) {{ a -= 0x60; }}",
            f"if ({self.extern.gen_call_getFlag(FLAG_H)}) {{ a -= 6; }}",
            "}",
            "Zf(a == 0);",
            "Hf(false);",
            "A = a;",
        ]


class FunctionInfo:
    def __init__(self, opcode: dict) -> None:
        self.opcode = opcode
        self.extern = ExternalFunction(DEFAULT_INSTANCE_NAME)
        self.misc_op = MiscOperation(self.extern, DEFAULT_INSTANCE_NAME)

    @property
    def _base_dict(self) -> dict[str, str]:
        unused_param = "_"
        return {
            "f_name": self.build_function_name(),
            "f_modifiers": " ".join(DEFAULT_MODIFIERS),
            "f_return_type": DEFAULT_RET_TYPE,
            "f_params": " ".join(
                f"{type} *{instance if self.mnemonic != "NOP" else unused_param}"
                for type, instance in DEFAULT_PARAMS.items()
            ),
        }

    @property
    def mnemonic(self) -> str:
        # "Name" consist of a string in format of either
        # XX
        # XX A
        # XX A1,A2
        # so to get the mnemonic part we just split on the whitespace and get the first value.
        return self.opcode.get("Name").split(" ")[0].upper()

    @property
    def group(self) -> str:
        return self.opcode.get("Group").upper()

    def _get_args(self) -> tuple[str, str]:
        name = self.opcode.get("Name").split(" ")

        if len(name) == 1:
            # No Args (Eg. `RLCA`, `NOP` etc.)
            return (EMPTY, EMPTY)

        args = [i.upper() for i in name[1].split(",")]

        if len(args) == 1:
            # 1 Arg (Eg. `INC R`, `SWAP R` etc.)
            return (args[0], EMPTY)

        # 2 Args (Eg. `LD R,R`, `SBC A,R` etc.)
        return tuple(args)

    def _get_addressing_mode(self, dst: str, src: str) -> str:

        for (dst_group, src_group), mode in AM_TABLE.items():
            if dst in dst_group and src in src_group:
                return mode

        if (dst.isdigit()) and (src in ARG_REGISTERS):
            # OP X,R
            return AM_U3_REGISTER

        if (dst.isdigit()) and (src in ARG_REGISTER_PAIRS_DEREFS):
            # OP X,(RR)
            return AM_U3_DEREF_REGISTERPAIR

        return AM_NONE

    def _get_lhs(self) -> str:
        dst = self._get_args()[0]
        if dst in ARG_REGISTER_PAIRS:
            return dst + "p"
        return dst

    def _get_rhs_expr(self) -> str:
        dst, src = self._get_args()
        mode = self._get_addressing_mode(dst, src)

        if mode in (
            AM_REGISTER_REGISTER,
            AM_U3_REGISTER,
        ):
            return src

        if mode == AM_REGISTERPAIR_REGISTERPAIR:
            return src + "p"

        if mode == AM_REGISTER_IMMEDIATE:
            return self.extern.gen_call_next_imm8()

        if mode in (
            AM_REGISTER_DEREF_REGISTERPAIR,
            AM_U3_DEREF_REGISTERPAIR,
        ):
            return self.extern.gen_call_read8(underef_rp(src or dst))

        if mode == AM_DEREF_REGISTERPAIR:
            return self.extern.gen_call_read8(underef_rp(dst))

        if mode == AM_REGISTERPAIR_IMMEDIATE16:
            return self.extern.gen_call_next_imm16()

        if mode == AM_DEREF_REGISTERPAIR_REGISTER:
            return self.extern.gen_call_write8(underef_rp(dst), src)

        if mode == AM_DEREF_REGISTERPAIR_IMMEDIATE:
            return self.extern.gen_call_write8(
                underef_rp(dst), self.extern.gen_call_next_imm8()
            )

    def _get_cc(self) -> str:
        dst, _ = self._get_args()
        inversed = None
        if dst in ("NC", "NZ"):
            inversed = True
        elif dst in ("C", "Z"):
            inversed = False
        else:
            return
        flag = FLAG_C if dst in ("C", "NC") else FLAG_Z
        return ("!" if inversed else EMPTY) + self.extern.gen_call_getFlag(flag)

    def build_function_name(self) -> str:
        # Building Name.

        # We create a base name first with no arguments.
        base = DEFAULT_DISCRIMINATOR + self.mnemonic

        # Then we fetch the args for this opcode.
        dst, src = [replace_deref_parens(i) for i in self._get_args()]

        # Then we check if this opcode requires a destination argument and add it if it does.
        if dst != EMPTY:
            base += DEFAULT_SEPARATOR + dst

        # Same thing again, check if it needs source argument or no and add if it does.
        if src != EMPTY:
            base += DEFAULT_SEPARATOR + src

        return base

    def build_preformed_info(
        self, preformed: list[str] | None = None
    ) -> dict[str, str]:
        if preformed is None:
            preformed = getattr(
                self.misc_op, "gen_" + self.build_function_name().replace("op", "")
            )()
        instr = {KEY_PREFORMED: preformed}
        instr.update(self._base_dict)
        return instr

    def build_LSM_info(self) -> dict[str, str | None]:
        # Generation Info
        # Includes: LD [R,R | R,(RR) | (RR),R | R,U8 | RR,U16 | RR,RR]
        # Excludes: LD [(RR+-),R | R,(RR+-) | (FF00+U8),A | A,(FF00+U8) | (FF00+C),A | A,(FF00+C) | (U16),SP]
        rhs = self._get_rhs_expr()
        lhs = self._get_lhs()
        if rhs is None and lhs == "SP":
            rhs = (
                self.extern.gen_call_next_imm16()
                if self._get_args()[1] == "U16"
                else "HLp"
            )

        preformed = True if "(" in lhs else False
        instr = dict()
        instr[KEY_PREFORMED] = preformed
        instr[KEY_RHS] = rhs
        instr[KEY_INTERNAL] = None
        if lhs == "SP" and rhs == "HLp":
            instr[KEY_INTERNAL] = self.extern.gen_call_internal()
        if not preformed:
            instr[KEY_LHS] = lhs
            instr[KEY_OP] = "="

        instr.update(self._base_dict)

        return instr

    def build_x16_LSM_stack_info(self) -> dict[str, str]:
        # Generation Info
        # Includes:
        # POP RR | PUSH RR
        # Excludes:
        # None
        mnemonic = self.mnemonic
        lhs = self._get_lhs()

        rhs = (
            self.extern.gen_call_push(lhs)
            if mnemonic == "PUSH"
            else self.extern.gen_call_pop()
        )
        instr = dict()
        instr[KEY_RHS] = rhs
        instr[KEY_LHS] = lhs
        instr[KEY_KIND] = mnemonic
        instr[KEY_INTERNAL] = self.extern.gen_call_internal()
        instr.update(self._base_dict)

        return instr

    def build_LSM_extra(self) -> str:
        # Generation Info
        # Includes: LD [(RR+-),R | R,(RR+-) | (FF00+U8),A | A,(FF00+U8) | (FF00+C),A | A,(FF00+C) | (U16),SP]
        # Excludes: None
        args = list(self._get_args())
        for i, arg in enumerate(args):
            if "(" in arg:
                match arg:
                    case "(FF00+U8)":
                        args[i] = f"(0xFF00 + {self.extern.gen_call_next_imm8()})"
                    case "(FF00+C)":
                        args[i] = "(0xFF00 + C)"
                    case "(HL+)":
                        args[i] = "(HLp++)"
                    case "(HL-)":
                        args[i] = "(HLp--)"
                    case "(U16)":
                        args[i] = f"({self.extern.gen_call_next_imm16()})"

        lhs, rhs = args
        preformed = True if "(" in lhs else False
        if not preformed:
            return self.build_preformed_info(
                [f"{lhs} = " + self.extern.gen_call_read8(rhs) + ";"]
            )

        return self.build_preformed_info(
            [
                (
                    self.extern.gen_call_write8(lhs, rhs)
                    if rhs != "SP"
                    else self.extern.gen_call_write16(lhs, rhs)
                )
                + ";"
            ]
        )

    def build_ALU_info(self) -> dict[str, str | None]:
        # Generation Info
        # Includes (both R,R and R,(HL) variants):
        # ADD, ADC, SUB, SBC, CP, INC, DEC, AND, OR, XOR
        # Excludes:
        # DAA, CPL, CCF, SCF
        mnemonic = self.mnemonic
        # If RHS is None then it is assumed that the instruction is either
        # INC or DEC which have 1 as RHS.
        # We assume so because they are the only instructions in x8/ALU and x16/ALU which have exactly 1 Arg.
        # LHS will never be Empty because the 0 Arg instructions have been blacklisted.
        rhs = self._get_rhs_expr() or 1
        rhs = "SP" if "SP" in self._get_args()[1] else rhs
        lhs = self._get_lhs()

        # Special Case #1: Only for INC (HL) and DEC (HL).
        # Here rhs becomes 1, lhs becomes a read8(HL) call and result is written to the memory.
        write_back = (
            self.extern.gen_call_write8(underef_rp(lhs), "result")
            if "(" in lhs
            else None
        )
        if write_back is not None:
            lhs, rhs = (rhs, 1)

        # Special Case #2: Only for CP.
        # Here write_back is set to False and the template doesnt write back the result.
        # Both the cases are independent of each other and never fire together.
        write_back = False if mnemonic == "CP" else write_back

        instr = dict()
        instr[KEY_RHS] = rhs
        instr[KEY_LHS] = lhs
        instr[KEY_OP] = ALU_OP_MAPPING[mnemonic]
        instr[KEY_WB] = write_back
        instr[KEY_INTERNAL] = (
            self.extern.gen_call_internal() if self.group == "X16/ALU" else None
        )
        instr[KEY_C_OPR] = None
        instr["int_type"] = "uint16_t" if self.group == "X16/ALU" else "uint8_t"
        # ADC and SBC require an additional Carry Flag.
        if mnemonic in ("ADC", "SBC"):
            instr[KEY_C_OPR] = self.extern.gen_call_getFlag(FLAG_C)

        flags = (
            get_x16_ALU_flags(mnemonic)
            if self.group == "X16/ALU"
            else get_x8_ALU_flags(mnemonic)
        )
        instr.update(vars(flags))
        instr.update(self._base_dict)

        return instr

    def build_RSB_info(self) -> dict[str, str | None]:
        # Generation Info
        # Includes (Both the R and (HL) variants):
        # RR, RL, RLC, RRC, SRL, SRA, SLA, SWAP, SET, RES, BIT, RLA, RRA, RLCA, RRCA
        # Excludes:
        # None
        mnemonic = self.mnemonic

        # Instruction is one of RRA, RLA, RLCA, RRCA if theres no arguments

        rhs = self._get_rhs_expr()
        if rhs is None:
            rhs = self._get_args()[0] or "A"

        lhs = self._get_lhs() if mnemonic in ("SET", "RES", "BIT") else None
        # Note
        # write_back controls writing back of operated value.
        # It has 3 states:
        # - A write8 call in case the destination is memory.
        # - A value of None in case the destination is register. (This part is assumed within the template)
        # - A value of False in case the operation does not allow writing back.
        # The keyword "result" is necessary. RSB only writes to HL's deref.
        write_back = (
            self.extern.gen_call_write8("HLp", "result") if "(" in rhs else None
        )

        write_back = f"{rhs} = result" if write_back is None else write_back
        write_back = False if mnemonic == "BIT" else write_back
        instr = dict()
        instr[KEY_RHS] = rhs
        instr[KEY_KIND] = mnemonic
        instr[KEY_LHS] = lhs
        instr[KEY_C_OPR] = self.extern.gen_call_getFlag(FLAG_C)
        instr[KEY_WB] = write_back

        flags = get_RSB_flags(mnemonic)
        instr.update(vars(flags))
        instr.update(self._base_dict)
        return instr

    def build_control_br_jump_info(self) -> dict[str, str | None]:
        # Generation Info
        # Includes:
        # JP U16 | JP CC,U16 | JR i8 | JR CC, i8 | RET | RET CC
        # Excludes:
        # JP HL
        mnemonic = self.mnemonic
        jumps = mnemonic in ("JP", "JR")

        # rhs is a pop operation in case of RET, read imm16 operation in case of JP
        # and signed read imm16 operation in case of JR
        rhs = None
        if mnemonic == "JP":
            rhs = self.extern.gen_call_next_imm16()
        elif mnemonic == "JR":
            rhs = "(uint16_t)(int8_t)" + self.extern.gen_call_next_imm8()
        else:
            rhs = self.extern.gen_call_pop()

        instr = dict()
        instr[KEY_PREFORMED] = False if jumps else True
        instr[KEY_RHS] = rhs
        instr[KEY_LHS] = "PC"
        instr[KEY_OP] = "+=" if mnemonic == "JR" else "="
        instr[KEY_INTERNAL] = self.extern.gen_call_internal()
        instr[KEY_CC] = self._get_cc()

        instr.update(self._base_dict)
        return instr

    def build_control_br_call_info(self) -> dict[str, str]:
        # Generation Info
        # Includes:
        # CALL U16 | CALL CC,U16 | RST XH
        # Excludes:
        # None
        dst, _ = self._get_args()
        lhs = "PC"
        rhs = (
            self.extern.gen_call_next_imm16()
            if not dst.endswith("H")
            else f"0x{dst[:-1]}"
        )
        instr = dict()
        instr[KEY_OP] = self.extern.gen_call_push(lhs)
        instr[KEY_LHS] = lhs
        instr[KEY_RHS] = rhs
        instr[KEY_CC] = self._get_cc()
        instr[KEY_INTERNAL] = self.extern.gen_call_internal()
        instr.update(self._base_dict)
        return instr


def make_instruction_list(
    table: dict[str, dict[str, str]],
) -> dict[str, dict[str, str]]:
    instructions = {
        TEMPLATE_ALU: [],
        TEMPLATE_LSM: [],
        TEMPLATE_RSB: [],
        TEMPLATE_STACK: [],
        TEMPLATE_JUMP: [],
        TEMPLATE_CALL: [],
        TEMPLATE_PREFORMED: [],
    }
    for i, opcode in enumerate(table["Unprefixed"] + table["CBPrefixed"]):
        if opcode.get("Name").upper() == "UNUSED":
            continue

        fn_info = FunctionInfo(opcode)
        opcode_full_name = fn_info.build_function_name().replace("op", "")

        if fn_info.group in ("X8/LSM", "X16/LSM"):
            if opcode_full_name not in LOAD_PREFORMED_LIST:
                if fn_info.mnemonic not in ("PUSH", "POP"):
                    instructions[TEMPLATE_LSM].append(fn_info.build_LSM_info())
                else:
                    instructions[TEMPLATE_STACK].append(
                        fn_info.build_x16_LSM_stack_info()
                    )
            else:
                instructions[TEMPLATE_PREFORMED].append(fn_info.build_LSM_extra())

        elif (
            fn_info.group in ("X8/ALU", "X16/ALU")
            and opcode_full_name not in MISC_PREFORMED_LIST
        ):
            instructions[TEMPLATE_ALU].append(fn_info.build_ALU_info())

        elif fn_info.group == "CONTROL/BR":
            if fn_info.mnemonic in ("JP", "JR", "RET", "RETI"):
                if opcode_full_name in MISC_PREFORMED_LIST:
                    instructions[TEMPLATE_PREFORMED].append(
                        fn_info.build_preformed_info()
                    )
                else:
                    instructions[TEMPLATE_JUMP].append(
                        fn_info.build_control_br_jump_info()
                    )
            elif fn_info.mnemonic in ("CALL", "RST"):
                instructions[TEMPLATE_CALL].append(fn_info.build_control_br_call_info())
        elif opcode_full_name in MISC_PREFORMED_LIST:
            instructions[TEMPLATE_PREFORMED].append(fn_info.build_preformed_info())

        elif fn_info.group == "X8/RSB":
            instructions[TEMPLATE_RSB].append(fn_info.build_RSB_info())

    return instructions


def make_handlers(
    instructions: dict[str, dict[str, str]], env: Environment
) -> dict[str, str]:
    buffer = StringIO()
    for group in instructions:
        template = env.get_template(group + ".jinja")
        output = template.render(instructions=instructions[group])
        buffer.write(output)
    return {SLOT_HANDLERS: buffer.getvalue()}


def make_lookup_tables(table: dict[str, dict[str, str]]) -> dict[str, str]:
    pref_buffer = StringIO()

    for i, op in enumerate(table["CBPrefixed"]):
        fn_info = FunctionInfo(op)
        pref_buffer.write(f"LUT[{hex(i)}] = " + fn_info.build_function_name() + ";\n")

    unpref_buffer = StringIO()

    for i, op in enumerate(table["Unprefixed"]):
        if op.get("Name").upper() not in ("UNUSED", "PREFIX CB"):
            fn_info = FunctionInfo(op)
            unpref_buffer.write(
                f"LUT[{hex(i)}] = " + fn_info.build_function_name() + ";\n"
            )

    return {
        SLOT_PREFIXED: pref_buffer.getvalue(),
        SLOT_UNPREFIXED: unpref_buffer.getvalue(),
    }


def make_backend(
    env: Environment, handlers: dict[str, str], luts: dict[str, str]
) -> str:
    slots = dict()
    slots.update(handlers)
    slots.update(luts)

    backend_template = env.get_template("backend.jinja")
    return backend_template.render(slots=slots)


def main() -> None:
    with open("codegen/dmgops.json") as file:
        table = json.loads(file.read())
        env = Environment(loader=FileSystemLoader("./codegen/templates/"))
        instructions = make_instruction_list(table)
        handlers = make_handlers(instructions, env)
        luts = make_lookup_tables(table)
        output = make_backend(env, handlers, luts)
        print(output)


if __name__ == "__main__":
    main()
