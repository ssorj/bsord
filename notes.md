https://www.hpl.hp.com/hosted/linux/mail-archives/libunwind/2004-March/000245.html
https://github.com/libunwind/libunwind
https://manpages.debian.org/testing/manpages-dev/mprotect.2.en.html
https://www.nongnu.org/libunwind/docs.html

"Offset" is "the byte-offset of the instruction-pointer saved in the
stack frame identified by cp, relative to the start of the
procedure. For example, if procedure foo() starts at address
0x40003000, then invoking unw_get_proc_name() on a stack frame with an
instruction-pointer value of 0x40003080 would return a value of 0x80
in the word pointed to by offp (assuming the procedure is at least
0x80 bytes long)."
