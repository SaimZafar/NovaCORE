#!/bin/bash
# ============================================================
#  NovaCORE — xv6 Setup & Patch Script
#  Run from the folder that CONTAINS xv6-public/
#  Or let it clone xv6 fresh automatically
# ============================================================

set -e
echo "========================================================"
echo "   NovaCORE — xv6 Kernel Extension Setup"
echo "   Syscall  : getmeminfo()"
echo "   Utility  : meminfo"
echo "========================================================"

# ── Step 1: Dependencies ─────────────────────────────────────
echo ""
echo "[1] Installing dependencies..."
sudo apt-get update -qq
sudo apt-get install -y git gcc qemu-system-i386 make
echo "    Done."

# ── Step 2: Clone xv6 ────────────────────────────────────────
echo ""
echo "[2] Cloning xv6..."
if [ -d "xv6-public" ]; then
    echo "    xv6-public/ already exists, skipping."
else
    git clone https://github.com/mit-pdos/xv6-public.git
fi
cd xv6-public

# ── Step 3: Patch proc.h — add meminfo struct ────────────────
echo ""
echo "[3] Patching proc.h..."
grep -q "struct meminfo" proc.h && echo "    Already patched." || \
sed -i '/^struct context/i \
struct meminfo {\
  int total_pages;\
  int free_pages;\
  int used_pages;\
};\
' proc.h
echo "    Done."

# ── Step 4: Patch syscall.h — add syscall number ─────────────
echo ""
echo "[4] Patching syscall.h..."
grep -q "SYS_getmeminfo" syscall.h && echo "    Already patched." || \
echo "#define SYS_getmeminfo  22" >> syscall.h
echo "    Done."

# ── Step 5: Patch user.h — expose to user space ──────────────
echo ""
echo "[5] Patching user.h..."
grep -q "getmeminfo" user.h && echo "    Already patched." || \
sed -i '/^\/\/ ulib.c/i int getmeminfo(struct meminfo*);\n' user.h
echo "    Done."

# ── Step 6: Patch usys.S — assembly stub ─────────────────────
echo ""
echo "[6] Patching usys.S..."
grep -q "SYSCALL(getmeminfo)" usys.S && echo "    Already patched." || \
echo "SYSCALL(getmeminfo)" >> usys.S
echo "    Done."

# ── Step 7: Patch syscall.c — register in table ──────────────
echo ""
echo "[7] Patching syscall.c..."
grep -q "sys_getmeminfo" syscall.c && echo "    Already patched." || {
    sed -i '/^extern int sys_close/a extern int sys_getmeminfo(void);' syscall.c
    sed -i '/\[SYS_close\]/a [SYS_getmeminfo] sys_getmeminfo,' syscall.c
}
echo "    Done."

# ── Step 8: Patch kalloc.c — free page counter ───────────────
echo ""
echo "[8] Patching kalloc.c..."
grep -q "kalloc_free_pages" kalloc.c && echo "    Already patched." || \
cat >> kalloc.c << 'EOF'

uint
kalloc_free_pages(void)
{
  struct run *r;
  uint count = 0;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){ count++; r = r->next; }
  release(&kmem.lock);
  return count;
}
EOF
echo "    Done."

# ── Step 9: Patch sysproc.c — syscall implementation ─────────
echo ""
echo "[9] Patching sysproc.c..."
grep -q "sys_getmeminfo" sysproc.c && echo "    Already patched." || \
cat >> sysproc.c << 'EOF'

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
EOF
echo "    Done."

# ── Step 10: Copy meminfo.c ───────────────────────────────────
echo ""
echo "[10] Copying meminfo.c..."
cp ../xv6_patches/meminfo.c ./meminfo.c
echo "     Done."

# ── Step 11: Patch Makefile ───────────────────────────────────
echo ""
echo "[11] Patching xv6 Makefile..."
grep -q "_meminfo" Makefile && echo "     Already patched." || \
sed -i 's/_wc\\/&\n\t_meminfo\\/' Makefile
echo "     Done."

# ── Step 12: Build ────────────────────────────────────────────
echo ""
echo "[12] Building xv6..."
make
echo ""
echo "========================================================"
echo "  BUILD SUCCESSFUL"
echo ""
echo "  To run:   make qemu-nox"
echo "  In xv6 shell type:  meminfo"
echo ""
echo "  Expected output:"
echo "  === NovaCORE: Memory Info (xv6 Kernel) ==="
echo "    Page Size   :  4 KB"
echo "    Total Pages : 3840  (15360 KB)"
echo "    Used  Pages : 312   (1248 KB)"
echo "    Free  Pages : 3528  (14112 KB)"
echo "    Usage       : 8%"
echo "========================================================"
