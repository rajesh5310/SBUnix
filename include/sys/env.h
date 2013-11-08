/* See COPYRIGHT for copyright information. */

#ifndef JOS_INC_ENV_H
#define JOS_INC_ENV_H

#include <defs.h>
#include <sys/pmap.h>
typedef uint64_t envid_t;

struct PushRegs {
	/* registers as pushed by pusha */
	uint64_t reg_edi;
	uint64_t reg_esi;
	uint64_t reg_ebp;
	uint64_t reg_oesp;		/* Useless */
	uint64_t reg_ebx;
	uint64_t reg_edx;
	uint64_t reg_ecx;
	uint64_t reg_eax;
} __attribute__((packed));

struct Trapframe {
	struct PushRegs tf_regs;
	uint64_t tf_es;
	uint64_t tf_padding1;
	uint64_t tf_ds;
	uint64_t tf_padding2;
	uint64_t tf_trapno;
	/* below here defined by x86 hardware */
	uint64_t tf_err;
	uintptr_t tf_eip;
	uint64_t tf_cs;
	uint64_t tf_padding3;
	uint64_t tf_eflags;
	/* below here only when crossing rings, such as from user to kernel */
	uintptr_t tf_esp;
	uint64_t tf_ss;
	uint64_t tf_padding4;
} __attribute__((packed));


// An environment ID 'envid_t' has three parts:
//
// +1+---------------21-----------------+--------10--------+
// |0|          Uniqueifier             |   Environment    |
// | |                                  |      Index       |
// +------------------------------------+------------------+
//                                       \--- ENVX(eid) --/
//
// The environment index ENVX(eid) equals the environment's offset in the
// 'envs[]' array.  The uniqueifier distinguishes environments that were
// created at different times, but share the same environment index.
//
// All real environments are greater than 0 (so the sign bit is zero).
// envid_ts less than 0 signify errors.  The envid_t == 0 is special, and
// stands for the current environment.

#define LOG2NENV		10
#define NENV			(1 << LOG2NENV)
#define ENVX(envid)		((envid) & (NENV - 1))

// Values of env_status in struct Env
enum {
	ENV_FREE = 0,
	ENV_RUNNABLE,
	ENV_RUNNING,
	ENV_NOT_RUNNABLE
};

// Special environment types
enum EnvType {
	ENV_TYPE_USER = 0,
	ENV_TYPE_IDLE,
};

struct Env {
	struct Trapframe env_tf;	// Saved registers
	struct Env *env_link;		// Next free Env
	envid_t env_id;			// Unique environment identifier
	envid_t env_parent_id;		// env_id of this env's parent
	enum EnvType env_type;		// Indicates special system environments
	unsigned env_status;		// Status of the environment
	uint64_t env_runs;		// Number of times environment has run

	// Address space
	pde_t *env_pgdir;		// Kernel virtual address of page dir
};

#endif // !JOS_INC_ENV_H
