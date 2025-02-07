#include "cpu_util.h"

Registers::Registers() {}
Registers::~Registers() {}

Instructions::Instructions(Registers &regs_) : regs(regs_) {}
Instructions::~Instructions() {}