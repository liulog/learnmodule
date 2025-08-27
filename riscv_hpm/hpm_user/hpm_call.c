#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <string.h>
#include <event.h>
#include <riscv_asm.h>

#define DEVICE_PATH "/dev/riscv_hpm"

#define IOCTL_MAGIC 'p'
#define IOCTL_ENABLE_COUNTERS _IOW(IOCTL_MAGIC, 0, uint32_t)
#define IOCTL_INHIBIT_COUNTERS _IOW(IOCTL_MAGIC, 1, uint32_t)
#define IOCTL_CONFIGURE_EVENT _IOW(IOCTL_MAGIC, 2, struct hpm_event_args)
#define IOCTL_SET_COUNTER _IOW(IOCTL_MAGIC, 3, struct hpm_counter_args)

struct hpm_event_args
{
    uint64_t idx;         // hpmevent index
    uint64_t event_value; // hpmevent value
};

struct hpm_counter_args
{
    uint64_t idx;         // hpmcounter index
    uint64_t count_value; // hpmcounter value
};

// Used to print the topdown metric.
#define ROWS 15
typedef struct {
    const char* level1;
    const char* level2;
    const char* level3;
    double value;
} TopdownMetric;

// Topdown metric table.
static TopdownMetric table[ROWS] = {
    {"Retiring", NULL, NULL, 0.00},
    {"FrontEnd Bound", NULL, NULL, 0.00},
    {NULL, "Fetch Latency Bound", NULL, 0.00},
    {NULL, "Fetch Bandwidth Bound", NULL, 0.00},
    {"Bad Speculation", NULL, NULL, 0.00},
    {NULL, "Branch Misspredict", NULL, 0.00},
    {NULL, "Machine Clears", NULL, 0.00},
    {"BackEnd Bound", NULL, NULL, 0.00},
    {NULL, "Core Bound", NULL, 0.00},
    {NULL, "Memory Bound", NULL, 0.00},
    {NULL, NULL, "L1 Bound", 0.00},
    {NULL, NULL, "L2 Bound", 0.00},
    {NULL, NULL, "L3 Bound", 0.00},
    {NULL, NULL, "Mem Bound", 0.00},
    {NULL, NULL, "Store Bound", 0.00}
};

uint64_t read_hpmcounter(int idx) {
    switch (idx) {
        case 0:  return csr_read(cycle);
        case 1:  return csr_read(time);
        case 2:  return csr_read(instret);
        case 3:  return csr_read(hpmcounter3);
        case 4:  return csr_read(hpmcounter4);
        case 5:  return csr_read(hpmcounter5);
        case 6:  return csr_read(hpmcounter6);
        case 7:  return csr_read(hpmcounter7);
        case 8:  return csr_read(hpmcounter8);
        case 9:  return csr_read(hpmcounter9);
        case 10: return csr_read(hpmcounter10);
        case 11: return csr_read(hpmcounter11);
        case 12: return csr_read(hpmcounter12);
        case 13: return csr_read(hpmcounter13);
        case 14: return csr_read(hpmcounter14);
        case 15: return csr_read(hpmcounter15);
        case 16: return csr_read(hpmcounter16);
        case 17: return csr_read(hpmcounter17);
        case 18: return csr_read(hpmcounter18);
        case 19: return csr_read(hpmcounter19);
        case 20: return csr_read(hpmcounter20);
        case 21: return csr_read(hpmcounter21);
        case 22: return csr_read(hpmcounter22);
        case 23: return csr_read(hpmcounter23);
        case 24: return csr_read(hpmcounter24);
        case 25: return csr_read(hpmcounter25);
        case 26: return csr_read(hpmcounter26);
        case 27: return csr_read(hpmcounter27);
        case 28: return csr_read(hpmcounter28);
        case 29: return csr_read(hpmcounter29);
        case 30: return csr_read(hpmcounter30);
        case 31: return csr_read(hpmcounter31);
        default:
            printf("Invalid hpmcounter index: %d\n", idx);
            return 0;
    }
}

void print_all_hpmcounters() {
    printf("cycle = 0x%llx\n", csr_read(cycle));
    printf("time = 0x%llx\n", csr_read(time));
    printf("instret = 0x%llx\n", csr_read(instret));
    printf("hpmcounter3 = 0x%llx\n", csr_read(hpmcounter3));
    printf("hpmcounter4 = 0x%llx\n", csr_read(hpmcounter4));
    printf("hpmcounter5 = 0x%llx\n", csr_read(hpmcounter5));
    printf("hpmcounter6 = 0x%llx\n", csr_read(hpmcounter6));
    printf("hpmcounter7 = 0x%llx\n", csr_read(hpmcounter7));
    printf("hpmcounter8 = 0x%llx\n", csr_read(hpmcounter8));
    printf("hpmcounter9 = 0x%llx\n", csr_read(hpmcounter9));
    printf("hpmcounter10 = 0x%llx\n", csr_read(hpmcounter10));
    printf("hpmcounter11 = 0x%llx\n", csr_read(hpmcounter11));
    printf("hpmcounter12 = 0x%llx\n", csr_read(hpmcounter12));
    printf("hpmcounter13 = 0x%llx\n", csr_read(hpmcounter13));
    printf("hpmcounter14 = 0x%llx\n", csr_read(hpmcounter14));
    printf("hpmcounter15 = 0x%llx\n", csr_read(hpmcounter15));
    printf("hpmcounter16 = 0x%llx\n", csr_read(hpmcounter16));
    printf("hpmcounter17 = 0x%llx\n", csr_read(hpmcounter17));
    printf("hpmcounter18 = 0x%llx\n", csr_read(hpmcounter18));
    printf("hpmcounter19 = 0x%llx\n", csr_read(hpmcounter19));
    printf("hpmcounter20 = 0x%llx\n", csr_read(hpmcounter20));
    printf("hpmcounter21 = 0x%llx\n", csr_read(hpmcounter21));
    printf("hpmcounter22 = 0x%llx\n", csr_read(hpmcounter22));
    printf("hpmcounter23 = 0x%llx\n", csr_read(hpmcounter23));
    printf("hpmcounter24 = 0x%llx\n", csr_read(hpmcounter24));
    printf("hpmcounter25 = 0x%llx\n", csr_read(hpmcounter25));
    printf("hpmcounter26 = 0x%llx\n", csr_read(hpmcounter26));
    printf("hpmcounter27 = 0x%llx\n", csr_read(hpmcounter27));
    printf("hpmcounter28 = 0x%llx\n", csr_read(hpmcounter28));
    printf("hpmcounter29 = 0x%llx\n", csr_read(hpmcounter29));
    printf("hpmcounter30 = 0x%llx\n", csr_read(hpmcounter30));
    printf("hpmcounter31 = 0x%llx\n", csr_read(hpmcounter31));
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s {enable|inhibit|setevent|setcounter|test|getcounters|defaultevent|topdown-metric-l12|topdown-metric-l3|topdown-event-l12|topdown-event-l3} [args...]\n", argv[0]);
        return 1;
    }

    const char *cmd = argv[1];
    int fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Failed to open device %s", DEVICE_PATH);
        return -1;
    }

    if (strcmp(cmd, "enable") == 0 || strcmp(cmd, "inhibit") == 0)
    {
        /// This is used to set enable or inhibit register.
        if (argc != 3)
        {
            fprintf(stderr, "Usage: %s value\n", argv[0], cmd);
            close(fd);
            return 1;
        }
        char *endptr;
        uint32_t value = (uint32_t)strtoul(argv[2], &endptr, 0);
        if (ioctl(fd, (strcmp(cmd, "enable") == 0) ? IOCTL_ENABLE_COUNTERS : IOCTL_INHIBIT_COUNTERS, &value) < 0)
        {
            fprintf(stderr, "ioctl error %s\n", argv[0], cmd);
            close(fd);
            return -1;
        }
    }
    else if (strcmp(cmd, "setevent") == 0)
    {
        /// This is used to set the event value according to idx and value.
        if (argc != 4)
        {
            fprintf(stderr, "Usage: %s event_index event_value\n", argv[0]);
            close(fd);
            return 1;
        }
        char *endptr;
        struct hpm_event_args args = {
            .idx = atoll(argv[2]),                                  // idx
            .event_value = (uint64_t)strtoull(argv[3], &endptr, 0), // event_value
        };
        if (ioctl(fd, IOCTL_CONFIGURE_EVENT, &args) < 0)
        {
            fprintf(stderr, "ioctl error %s\n", argv[0], cmd);
            close(fd);
            return -1;
        }
    }
    else if (strcmp(cmd, "setcounter") == 0)
    {
        if (argc != 4)
        {
            fprintf(stderr, "Usage: %s counter_index counter_value\n", argv[0]);
            close(fd);
            return 1;
        }
        char *endptr;
        struct hpm_counter_args args = {
            .idx = atoll(argv[2]),                                  // idx
            .count_value = (uint64_t)strtoull(argv[3], &endptr, 0), // count_value
        };
        if (ioctl(fd, IOCTL_SET_COUNTER, &args) < 0)
        {
            fprintf(stderr, "ioctl error %s\n", argv[0], cmd);
            close(fd);
            return -1;
        }
    }
    else if (strcmp(cmd, "test") == 0)
    {
        /// This is just a simple test.
        // 1. stop related counters
        uint32_t value = 0xFFFFFFFF;
        ioctl(fd, IOCTL_INHIBIT_COUNTERS, &value);

        // 2. configure hpmevent
        struct hpm_event_args args1 = {
            .idx = 3,
            .event_value = make_hpmevent_single(MODE_M, Frontend_frontendFlush),
        };
        ioctl(fd, IOCTL_CONFIGURE_EVENT, &args1);
        struct hpm_event_args args2 = {
            .idx = 11,
            .event_value = make_hpmevent_single(MODE_M, CtrlBlock_decoder_waitInstr),
        };
        ioctl(fd, IOCTL_CONFIGURE_EVENT, &args2);

        // 3. enable related counters
        value = 0xFFFFFFFF;
        ioctl(fd, IOCTL_ENABLE_COUNTERS, &value);

        // 4. begin related counters
        value = 0x0;
        ioctl(fd, IOCTL_INHIBIT_COUNTERS, &value);

        printf("hpmcounter3 = 0x%llx\n", csr_read(hpmcounter3));
        printf("hpmcounter11 = 0x%llx\n", csr_read(hpmcounter11));

        // === tmp workload ===
        volatile uint64_t a = 0;
        for (uint64_t i = 0; i < 1000; i++)
        {
            a += a + i;
        }
        printf("hpmcounter3 = 0x%llx\n", csr_read(hpmcounter3));
        printf("hpmcounter11 = 0x%llx\n", csr_read(hpmcounter11));
    }
    else if (strcmp(cmd, "getcounters") == 0)
    {
        /// This is used to print all hpmcounters.
        print_all_hpmcounters();
    }
    else if (strcmp(cmd, "defaultevent") == 0)
    {
        /// This is used to configure the default event that you need.
        struct hpm_event_args hpm_configs[32];
        memset(hpm_configs, 0, sizeof(hpm_configs));
        
        // 配置事件
        hpm_configs[3]  = (struct hpm_event_args){3,  make_hpmevent_single(MODE_M, 21)};
        hpm_configs[11] = (struct hpm_event_args){11, make_hpmevent_single(MODE_M, 5)};
        hpm_configs[12] = (struct hpm_event_args){12, make_hpmevent_single(MODE_M, 6)};
        hpm_configs[13] = (struct hpm_event_args){13, make_hpmevent_single(MODE_M, 51)};
        
        hpm_configs[19] = (struct hpm_event_args){19, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, noEvent, 4, 11, 18)};
        hpm_configs[20] = (struct hpm_event_args){20, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, noEvent, 5, 12, 19)};
        hpm_configs[21] = (struct hpm_event_args){21, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, noEvent, 6, 13, 20)};
        hpm_configs[22] = (struct hpm_event_args){22, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, noEvent, 7, 14, 21)};
        
        hpm_configs[27] = (struct hpm_event_args){27, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, 1, 13, 25, 37)};
        hpm_configs[28] = (struct hpm_event_args){28, make_hpmevent_quad(MODE_M, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, 5, 17, 29, 41)};
        
        for (int i = 3; i < 32; ++i) {
            if (hpm_configs[i].event_value != 0) {
                ioctl(fd, IOCTL_CONFIGURE_EVENT, &hpm_configs[i]);
            }
        }
    }
    else if (strcmp(cmd, "topdown-event-l12") == 0){
        // * 前端：mhpmevent 3-10
        // * 后端：mhpmevent11-18
        // * 访存：mhpmevent19-26
        // * 缓存：mhpmevent27-31
        // #define rob_commitInstr 51              // Backend 
        // #define INST_RETIRED rob_commitInstr
        // #define INST_SPEC 5                     // Backend
        // #define Front_Bubble 21                 // Frontend 
        // #define IF_FETCH_BUBBLE Front_Bubble
        // #define Fetch_Latency_Bound 22          // Frontend
        // #define IF_FETCH_BUBBLE_EQ_MAX  Fetch_Latency_Bound 
        // #define RECOVERY_BUBBLE 6               // Backend
        // #define BR_MIS_PRED 62                  // Backend  
        // #define TOTAL_FLUSH 63                  // Backend  
        // #define EXEC_STALL_CYCLE 64             // Backend
        // #define MEMSTALL_ANYLOAD 38             // Memory
        // #define MEMSTALL_STORE 65               // Backend
        struct hpm_event_args hpm_configs[32];
        memset(hpm_configs, 0, sizeof(hpm_configs));
        // Frontend
        hpm_configs[3]  = (struct hpm_event_args){3,  make_hpmevent_single(0, IF_FETCH_BUBBLE)};
        hpm_configs[4]  = (struct hpm_event_args){4,  make_hpmevent_single(0, IF_FETCH_BUBBLE_EQ_MAX)};
        // Backend
        hpm_configs[11]  = (struct hpm_event_args){11,  make_hpmevent_single(0, INST_RETIRED)};
        hpm_configs[12]  = (struct hpm_event_args){12,  make_hpmevent_single(0, INST_SPEC)};
        hpm_configs[13]  = (struct hpm_event_args){13,  make_hpmevent_single(0, RECOVERY_BUBBLE)};
        hpm_configs[14]  = (struct hpm_event_args){14,  make_hpmevent_single(0, BR_MIS_PRED)};
        hpm_configs[15]  = (struct hpm_event_args){15,  make_hpmevent_single(0, TOTAL_FLUSH)};
        hpm_configs[16]  = (struct hpm_event_args){16,  make_hpmevent_single(0, EXEC_STALL_CYCLE)};
        hpm_configs[17]  = (struct hpm_event_args){17,  make_hpmevent_single(0, MEMSTALL_STORE)};
        // Memory
        hpm_configs[19]  = (struct hpm_event_args){19,  make_hpmevent_single(0, MEMSTALL_ANYLOAD)};

        for (int i = 3; i < 32; ++i) {
            if (hpm_configs[i].event_value != 0) {
                ioctl(fd, IOCTL_CONFIGURE_EVENT, &hpm_configs[i]);
            }
        }
    }
    else if (strcmp(cmd, "topdown-metric-l12") == 0)
    {
        // 1. calculate the topdown metric.
        // INST_RETIRED / (IssueBW * CPU_CYCLES)
        table[0].value = (double)csr_read(hpmcounter11) / (double)(IssueBW * csr_read(cycle));
        // IF_FETCH_BUBBLE / (IssueBW * CPU_CYCLES)
        table[1].value = (double)csr_read(hpmcounter3) / (double)(IssueBW * csr_read(cycle));
        // IF_FETCH_BUBBLE_EQ_MAX / CPU_CYCLES
        table[2].value = (double)csr_read(hpmcounter4) / (double)csr_read(cycle);
        // FrontEnd Bound - Fetch Latency Bound
        table[3].value = table[1].value - table[2].value;
        // (INST_SPEC - INST_RETIRED + RECOVERY_BUBBLE) / (IssueBW * CPU_CYCLES)
        table[4].value = (double)(csr_read(hpmcounter12)-csr_read(hpmcounter11)+csr_read(hpmcounter13)) / (double)(IssueBW * csr_read(cycle));
        // Bad Speculation * BR_MIS_PRED / TOTAL_FLUSH
        table[5].value = table[4].value * (double)csr_read(hpmcounter14) / (double)csr_read(hpmcounter15);
        // Bad Speculation - Branch Misspredict
        table[6].value = table[4].value - table[5].value;
        // 1 - (FrontEnd Bound + Bad Speculation + Retiring)
        table[7].value = 1 - table[0].value - table[1].value - table[4].value;
        // (EXEC_STALL_CYCLE - MEMSTALL_ANYLOAD - MEMSTALL_STORE) / CPU_CYCLE 
        table[8].value = (double)(csr_read(hpmcounter16)-csr_read(hpmcounter19)-csr_read(hpmcounter17)) / (double)csr_read(cycle);
        // (MEMSTALL_ANYLOAD + MEMSTALL_STORE) / CPU_CYCLES 
        table[9].value = (double)(csr_read(hpmcounter19)+csr_read(hpmcounter17)) / (double)csr_read(cycle);

        unsigned long long cycle = csr_read(cycle);
        unsigned long long instret = csr_read(instret);
        printf("cycle = 0x%llx\n", cycle);
        printf("instret = 0x%llx\n", instret);
        printf("IPC = %.2f\n", (double)instret / (double)cycle);

        // 2. print the topdown metric table.
        printf("+------------------+------------------------+------------------+--------+\n");
        printf("| %-16s | %-22s | %-16s | %-6s |\n", "Level 1", "Level 2", "Level 3", "Result");
        printf("+------------------+------------------------+------------------+--------+\n");
        for (int i = 0; i < 10; ++i) {
            printf("| %-16s | %-22s | %-16s | %6.2f |\n",
                table[i].level1 ? table[i].level1 : "-",
                table[i].level2 ? table[i].level2 : "-",
                table[i].level3 ? table[i].level3 : "-",
                table[i].value);
        }
        printf("+------------------+------------------------+------------------+--------+\n");
    }
    else if (strcmp(cmd, "topdown-event-l3") == 0){
        // #define MEMSTALL_ANYLOAD 38             // Memory
        // #define MEMSTALL_STORE 65               // Backend
        // #define MEMSTALL_L1MISS 66              // Backend
        // #define MEMSTALL_L2MISS 67              // Backend
        // #define MEMSTALL_L3MISS 68              // Backend
        struct hpm_event_args hpm_configs[32];
        memset(hpm_configs, 0, sizeof(hpm_configs));
        // Backend
        hpm_configs[11]  = (struct hpm_event_args){11,  make_hpmevent_single(0, MEMSTALL_ANYLOAD)};
        hpm_configs[12]  = (struct hpm_event_args){12,  make_hpmevent_single(0, MEMSTALL_STORE)};
        hpm_configs[13]  = (struct hpm_event_args){13,  make_hpmevent_single(0, MEMSTALL_L1MISS)};
        hpm_configs[14]  = (struct hpm_event_args){14,  make_hpmevent_single(0, MEMSTALL_L2MISS)};
        hpm_configs[15]  = (struct hpm_event_args){15,  make_hpmevent_single(0, MEMSTALL_L3MISS)};
        
        // TLB 与 Cache 相关的事件
        hpm_configs[19]  = (struct hpm_event_args){19,  make_hpmevent_quad(0, OPTYPE_ADD, OPTYPE_ADD, OPTYPE_ADD, 7, 14, 21, noEvent)};
        hpm_configs[20]  = (struct hpm_event_args){20,  make_hpmevent_quad(0, OPTYPE_ADD, OPTYPE_ADD, OPTYPE_ADD, 6, 13, 20, noEvent)};
        hpm_configs[21]  = (struct hpm_event_args){21,  make_hpmevent_quad(0, OPTYPE_ADD, OPTYPE_ADD, OPTYPE_ADD, 5, 12, 19, noEvent)};
        hpm_configs[22]  = (struct hpm_event_args){22,  make_hpmevent_quad(0, OPTYPE_ADD, OPTYPE_ADD, OPTYPE_ADD, 4, 11, 18, noEvent)};
        hpm_configs[23]  = (struct hpm_event_args){23,  make_hpmevent_single(0, 115)};
        hpm_configs[24]  = (struct hpm_event_args){24,  make_hpmevent_single(0, 110)};

        for (int i = 3; i < 32; ++i) {
            if (hpm_configs[i].event_value != 0) {
                ioctl(fd, IOCTL_CONFIGURE_EVENT, &hpm_configs[i]);
            }
        }
    }
    else if (strcmp(cmd, "topdown-metric-l3") == 0){
        // 1. calculate the topdown metric.
        // (MEMSTALL_ANYLOAD - MEMSTALL_L1MISS) / CPU_CYCLES
        table[10].value = (double)(csr_read(hpmcounter11) - csr_read(hpmcounter13))/ (double)csr_read(cycle);
        // (MEMSTALL_L1MISS - MEMSTALL_L2MISS) / CPU_CYCLES
        table[11].value = (double)(csr_read(hpmcounter13) - csr_read(hpmcounter14)) / (double)csr_read(cycle);
        // (MEMSTALL_L2MISS - MEMSTALL_L3MISS) / CPU_CYCLES
        table[12].value = (double)(csr_read(hpmcounter14) - csr_read(hpmcounter15)) / (double)csr_read(cycle);
        // MEMSTALL_L3MISS / CPU_CYCLES
        table[13].value = (double)csr_read(hpmcounter15) / (double)csr_read(cycle);
        // MEMSTALL_STORE / CPU_CYCLES 
        table[14].value = (double)csr_read(hpmcounter12) / (double)csr_read(cycle);

        // 2. print the topdown metric table.
        printf("+------------------+------------------------+------------------+--------+\n");
        printf("| %-16s | %-22s | %-16s | %-6s |\n", "Level 1", "Level 2", "Level 3", "Result");
        printf("+------------------+------------------------+------------------+--------+\n");
        for (int i = 10; i < ROWS; ++i) {
            printf("| %-16s | %-22s | %-16s | %6.2f |\n",
                table[i].level1 ? table[i].level1 : "-",
                table[i].level2 ? table[i].level2 : "-",
                table[i].level3 ? table[i].level3 : "-",
                table[i].value);
        }
        printf("+------------------+------------------------+------------------+--------+\n");

        printf("MEMSTALL_ANYLOAD = 0x%llx\n", csr_read(hpmcounter11));
        printf("MEMSTALL_STORE = 0x%llx\n", csr_read(hpmcounter12));
        printf("MEMSTALL_L1MISS = 0x%llx\n", csr_read(hpmcounter13));
        printf("MEMSTALL_L2MISS = 0x%llx\n", csr_read(hpmcounter14));
        printf("MEMSTALL_L3MISS = 0x%llx\n", csr_read(hpmcounter15));

        unsigned long long dcache_miss = csr_read(hpmcounter19);
        unsigned long long dcache_access = csr_read(hpmcounter20);
        unsigned long long dtlb_miss = csr_read(hpmcounter21);
        unsigned long long dtlb_access = csr_read(hpmcounter22);
        unsigned long long l2tlb_miss = csr_read(hpmcounter23);
        unsigned long long l2tlb_access = csr_read(hpmcounter24); 
        printf("dcache_miss = 0x%llx\n", dcache_miss);
        printf("dcache_access = 0x%llx\n", dcache_access);
        printf("dtlb_miss = 0x%llx\n", dtlb_miss);
        printf("dtlb_access = 0x%llx\n", dtlb_access);
        printf("l2tlb_miss = 0x%llx\n", l2tlb_miss);
        printf("l2tlb_access = 0x%llx\n", l2tlb_access);
        printf("dcache_miss_rate = %.2f\n", (double)dcache_miss / (double)dcache_access);
        printf("dtlb_miss_rate = %.2f\n", (double)dtlb_miss / (double)dtlb_access);
        printf("l2tlb_miss_rate = %.2f\n", (double)l2tlb_miss / (double)l2tlb_access);
    }
    else
    {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}