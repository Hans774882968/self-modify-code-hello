#include <stdio.h>   // I/O
#include <windows.h> // Mem

// supposed malicious function
void malicious () {
    // i is always zero
    int i = 0;
    // thus this check is always fail
    // unless SMC changes this value
    if (i == 0x114514)
        // this is the malicious behavior
        printf ("I'm a malware i = %08x\n", i);
    else printf ("not malware i = %d\n", i);
}

// simple anti analysis function
BOOL anti_analysis() {
    return IsDebuggerPresent();
}

// page permission, as in the previous examples
void change_page (void *addr) {
    DWORD old;
    SYSTEM_INFO si;
    UINT64 _addr = (UINT64) addr;
    GetSystemInfo (&si);
    int page_size = si.dwPageSize;
    _addr -= (UINT64) addr % page_size;

    if (!VirtualProtect ( (PVOID) _addr, page_size, PAGE_EXECUTE_READWRITE, &old) )
        printf ("Error: %x\n", GetLastError() );

    return;
}

// Program entry
int main() {
    // try first time
    // will be non-malicious
    malicious();

    //debug print
    printf ("PID %u\n", GetCurrentProcessId() );

    // anti-analysis trick
    if (!anti_analysis() ) {
        const int FUNC_ADJUST_OFFSET = 0x08; // Function entry offset
        const int INSTR_OFFSET = 0x3; // instruction offset within the function
        const int INSTR_DATA = 0x114514; // new instruction bytes
        // SMC, as in the previous case
        void *func_addr = (char *) malicious + FUNC_ADJUST_OFFSET;
        change_page ( (void *) malicious);
        int *instruction = (int *) ( (char *) func_addr + INSTR_OFFSET);
        *instruction = INSTR_DATA;
    }

    // try second time
    // this time will be malicious
    malicious();

    return 0;
}