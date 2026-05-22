// ============================================================
// FILE: syscall.c  (xv6 kernel file)
//
// ACTION 1: Add this line in the extern declarations block
//           near the top where other sys_ functions are declared
// ============================================================

extern int sys_getmeminfo(void);

// ============================================================
// ACTION 2: Add this entry in the syscalls[] array
//           Find:  [SYS_close]   sys_close,
//           Add AFTER it:
// ============================================================

[SYS_getmeminfo] sys_getmeminfo,
