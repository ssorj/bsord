// The libunwind version we're currently using has a bug where
// it will hang in unw_get_proc_name if the heap is messed up.
//
// while (unw_step(&cursor) > 0) {
//     unw_word_t ip = {0};
//     unw_word_t sp = {0};
//     unw_word_t offset = {0};
//
//     unw_get_reg(&cursor, UNW_REG_IP, &ip);
//     unw_get_reg(&cursor, UNW_REG_SP, &sp);
//     print("The next line hangs\n");
//     unw_get_proc_name(&cursor, field, FIELD_SIZE, &offset);
//     print("This line is never printed\n");
//
//     snprintf(line, LINE_SIZE, "  %2d: [0x%016" PRIxPTR "] %s+0x%" PRIxPTR " (0x%016" PRIxPTR ")\n", i, ip, field, offset, sp);
//     print(line);
// }
