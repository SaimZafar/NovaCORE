// ============================================================
// FILE: sysproc.c  (xv6 kernel file)
// ACTION: Add this function at the END of sysproc.c
// ============================================================

int
sys_getmeminfo(void)
{
    struct meminfo *mi;
    extern uint kalloc_free_pages(void);

    if(argptr(0, (void*)&mi, sizeof(*mi)) < 0)
        return -1;

    mi->total_pages = PHYSTOP / PGSIZE;
    mi->free_pages  = kalloc_free_pages();
    mi->used_pages  = mi->total_pages - mi->free_pages;

    return 0;
}
