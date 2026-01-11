/* minirisc.ld — Linker script bare-metal Mini-RISC */

OUTPUT_ARCH(riscv)
ENTRY(_minirisc_init)

MEMORY
{
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 32M
}

SECTIONS
{
  . = ORIGIN(RAM);

  /* ---- Boot code: first instructions executed ---- */
  .minirisc_init : ALIGN(4)
  {
    KEEP(*(.minirisc_init))
  } > RAM

  /* ---- Code + constants ---- */
  .text : ALIGN(4)
  {
    *(.text .text.*)
    *(.gnu.linkonce.t.*)
    *(.init)
    *(.fini)

    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    *(.srodata .srodata.*)
  } > RAM

  /* ✅ Keep unwind info needed by crtbegin (fix _EH_FRAME_BEGIN_) */
  .eh_frame : ALIGN(4)
  {
    KEEP(*(.eh_frame))
    KEEP(*(.eh_frame.*))
  } > RAM

  /* ---- Constructors / destructors arrays ---- */
  .preinit_array : ALIGN(4)
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array))
    KEEP(*(.preinit_array.*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } > RAM

  .init_array : ALIGN(4)
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN(__init_array_end = .);
  } > RAM

  .fini_array : ALIGN(4)
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } > RAM

  /* ---- Initialized data ---- */
  .data : ALIGN(4)
  {
    __data_start = .;
    *(.data .data.*)
    *(.sdata .sdata.*)
    __data_end = .;
  } > RAM

  /* ✅ Define global pointer for newlib crt0 (_start) */
  __global_pointer$ = __data_start + 0x800;

  /* ---- BSS ---- */
  .bss (NOLOAD) : ALIGN(4)
  {
    __bss_start = .;
    *(.bss .bss.*)
    *(.sbss .sbss.*)
    *(COMMON)
    __bss_end = .;
  } > RAM

  /* ---- End of image (used by _sbrk) ---- */
  . = ALIGN(4);
  _end = .;
  PROVIDE(end = .);

  /* ---- Stack top ---- */
  __stack_top = ORIGIN(RAM) + LENGTH(RAM);

  /* Discard only what is safe to discard */
  /DISCARD/ :
  {
    *(.comment)
    *(.note*)
    *(.debug*)
    *(.gnu*)
    *(.interp)
    *(.dyn*)
    *(.hash)
    *(.rela*)
  }
}