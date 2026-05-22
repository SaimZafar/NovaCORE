// ============================================================
// FILE: meminfo.c  (place in xv6 root directory)
// USAGE: Type  meminfo  in the xv6 shell
// Shows physical memory usage by calling sys_getmeminfo()
// ============================================================

#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc.h"   // for struct meminfo

int
main(void)
{
    struct meminfo mi;

    if(getmeminfo(&mi) < 0){
        printf(2, "meminfo: getmeminfo() system call failed\n");
        exit();
    }

    int used_kb  = mi.used_pages  * 4;
    int free_kb  = mi.free_pages  * 4;
    int total_kb = mi.total_pages * 4;
    int usage_pct = mi.total_pages > 0
                    ? (mi.used_pages * 100) / mi.total_pages
                    : 0;

    printf(1, "\n");
    printf(1, "=== NovaCORE: Memory Info (xv6 Kernel) ===\n");
    printf(1, "  Page Size   :  4 KB\n");
    printf(1, "  Total Pages : %d  (%d KB)\n", mi.total_pages, total_kb);
    printf(1, "  Used  Pages : %d  (%d KB)\n", mi.used_pages,  used_kb);
    printf(1, "  Free  Pages : %d  (%d KB)\n", mi.free_pages,  free_kb);
    printf(1, "  Usage       : %d%%\n", usage_pct);
    printf(1, "===========================================\n\n");

    exit();
}
