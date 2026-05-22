// ============================================================
// FILE: kalloc.c  (xv6 kernel file)
// ACTION: Add this function at the END of kalloc.c
//         Used by sys_getmeminfo() to count free pages
// ============================================================

uint
kalloc_free_pages(void)
{
    struct run *r;
    uint        count = 0;

    acquire(&kmem.lock);
    r = kmem.freelist;
    while(r){ count++; r = r->next; }
    release(&kmem.lock);

    return count;
}
