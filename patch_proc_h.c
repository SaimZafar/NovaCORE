// ============================================================
// FILE: proc.h  (xv6 kernel file)
// ACTION: Add this struct BEFORE the struct proc definition
// ============================================================

struct meminfo {
    int total_pages;   // total physical pages in system
    int free_pages;    // currently free pages
    int used_pages;    // currently used pages
};
