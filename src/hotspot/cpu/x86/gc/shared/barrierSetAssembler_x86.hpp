/*
 * Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef CPU_X86_GC_SHARED_BARRIERSETASSEMBLER_X86_HPP
#define CPU_X86_GC_SHARED_BARRIERSETASSEMBLER_X86_HPP

#include "asm/macroAssembler.hpp"
#include "memory/allocation.hpp"
#include "oops/access.hpp"

class InterpreterMacroAssembler;

class BarrierSetAssembler: public CHeapObj<mtGC> {
private:
  void incr_allocated_bytes(MacroAssembler* masm, Register thread,
                            Register var_size_in_bytes,
                            int con_size_in_bytes,
                            Register t1);

public:
  virtual void arraycopy_prologue(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                                  Register src, Register dst, Register count) {}
  virtual void arraycopy_epilogue(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                                  Register src, Register dst, Register count) {}

  virtual void load_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                       Register dst, Address src, Register tmp1, Register tmp_thread);
  virtual void store_at(MacroAssembler* masm, DecoratorSet decorators, BasicType type,
                        Address dst, Register val, Register tmp1, Register tmp2, Register tmp3);

  // The copy_[load/store]_at functions are used by arraycopy stubs. Be careful to only use
  // r10 (aka rscratch1) in a context where restore_arg_regs_using_thread has been used instead
  // of the looser setup_arg_regs. Currently this is done when using type T_OBJECT.
  virtual void copy_load_at(MacroAssembler* masm,
                            DecoratorSet decorators,
                            BasicType type,
                            size_t bytes,
                            Register dst,
                            Address src,
                            Register tmp);

  virtual void copy_store_at(MacroAssembler* masm,
                             DecoratorSet decorators,
                             BasicType type,
                             size_t bytes,
                             Address dst,
                             Register src,
                             Register tmp);

  virtual void copy_load_at(MacroAssembler* masm,
                            DecoratorSet decorators,
                            BasicType type,
                            size_t bytes,
                            XMMRegister dst,
                            Address src,
                            Register tmp,
                            XMMRegister xmm_tmp);

  virtual void copy_store_at(MacroAssembler* masm,
                             DecoratorSet decorators,
                             BasicType type,
                             size_t bytes,
                             Address dst,
                             XMMRegister src,
                             Register tmp1,
                             Register tmp2,
                             XMMRegister xmm_tmp);

  virtual bool supports_avx3_masked_arraycopy() { return true; }

  // Support for jniFastGetField to try resolving a jobject/jweak in native
  virtual void try_resolve_jobject_in_native(MacroAssembler* masm, Register jni_env,
                                             Register obj, Register tmp, Label& slowpath);

  virtual void tlab_allocate(MacroAssembler* masm,
                             Register thread, Register obj,
                             Register var_size_in_bytes,
                             int con_size_in_bytes,
                             Register t1, Register t2,
                             Label& slow_case);

  virtual void eden_allocate(MacroAssembler* masm,
                             Register thread, Register obj,
                             Register var_size_in_bytes,
                             int con_size_in_bytes,
                             Register t1,
                             Label& slow_case) {
    assert(false, "To be overridden by MMTk");
  }

  virtual void barrier_stubs_init() {}

  virtual void nmethod_entry_barrier(MacroAssembler* masm, Label* slow_path, Label* continuation);
  virtual void c2i_entry_barrier(MacroAssembler* masm);

  virtual void check_oop(MacroAssembler* masm, Register obj, Register tmp1, Register tmp2, Label& error);
};

#endif // CPU_X86_GC_SHARED_BARRIERSETASSEMBLER_X86_HPP
