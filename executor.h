#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "types.h"
class Core;

typedef bool (*Executor)(Core*, Instruction);

bool executorAddi (Core* core, Instruction insn);
bool executorAddu (Core* core, Instruction insn);
bool executorAdd (Core* core, Instruction insn);
bool executorSubu (Core* core, Instruction insn);
bool executorSub (Core* core, Instruction insn);
bool executorSlt (Core* core, Instruction insn);
bool executorBne (Core* core, Instruction insn);
bool executorBeq (Core* core, Instruction insn);
bool executorJ (Core* core, Instruction insn);
bool executorLb (Core* core, Instruction insn);
bool executorLh (Core* core, Instruction insn);
bool executorLw (Core* core, Instruction insn);
bool executorSb (Core* core, Instruction insn);
bool executorSh (Core* core, Instruction insn);
bool executorSw (Core* core, Instruction insn);
bool executorOr (Core* core, Instruction insn);
bool executorAnd (Core* core, Instruction insn);


#endif // EXECUTOR_H
