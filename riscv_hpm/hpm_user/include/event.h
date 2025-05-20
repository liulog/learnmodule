
#ifndef __EVENT_H____
#define __EVENT_H____

#include <stdint.h>
#include <riscv_asm.h>

#define noEvent 0

// Test used events.
#define Frontend_frontendFlush 1
#define CtrlBlock_decoder_waitInstr 2

// kmh's issue width is 6.
#define IssueBW 6

// Top-down related events.
#define rob_commitInstr 51              // Backend 
#define INST_RETIRED rob_commitInstr
#define INST_SPEC 5                     // Backend
#define Front_Bubble 21                 // Frontend 
#define IF_FETCH_BUBBLE Front_Bubble
#define Fetch_Latency_Bound 22          // Frontend
#define IF_FETCH_BUBBLE_EQ_MAX  Fetch_Latency_Bound 
#define RECOVERY_BUBBLE 6               // Backend
#define BR_MIS_PRED 62                  // Backend  
#define TOTAL_FLUSH 63                  // Backend  
#define EXEC_STALL_CYCLE 64             // Backend
#define MEMSTALL_ANYLOAD 38             // Memory
#define MEMSTALL_STORE 65               // Backend
#define MEMSTALL_L1MISS 66              // Backend
#define MEMSTALL_L2MISS 67              // Backend
#define MEMSTALL_L3MISS 68              // Backend

// Hpmevent register ralated fileds.
#define MODE_OFFSET 59
#define MODE_MASK 0x1F
#define MODE_M 0x10
#define MODE_H 0x08
#define MODE_S 0x04
#define MODE_U 0x02
#define MODE_D 0x01

#define OPTYPE2_OFFSET 50
#define OPTYPE2_MASK 0x1F
#define OPTYPE1_OFFSET 45
#define OPTYPE1_MASK 0x1F
#define OPTYPE0_OFFSET 40
#define OPTYPE0_MASK 0x1F
#define OPTYPE_OR 0x0
#define OPTYPE_AND 0x1
#define OPTYPE_XOR 0x2
#define OPTYPE_ADD 0x4
// Operations
// Event0 <Optype0> Event1 = T1
// Event2 <Optype1> Event3 = T2
// T1 <Optype2> T2 = Result

#define EVENT3_OFFSET 30
#define EVENT3_MASK 0x3FF
#define EVENT2_OFFSET 20
#define EVENT2_MASK 0x3FF
#define EVENT1_OFFSET 10
#define EVENT1_MASK 0x3FF
#define EVENT0_OFFSET 0
#define EVENT0_MASK 0x3FF

#define SET(reg, field, value) (reg) = ((reg) & ~((uint64_t)(field##_MASK) << (field##_OFFSET))) | ((uint64_t)(value) << (field##_OFFSET));

static inline uint64_t make_hpmevent_quad(uint64_t mode,
    uint64_t optype2, uint64_t optype1, uint64_t optype0,
    uint64_t event3, uint64_t event2, uint64_t event1, uint64_t event0)
{
    uint64_t value = 0x0;
    SET(value, MODE, mode);
    SET(value, OPTYPE2, optype2);
    SET(value, OPTYPE1, optype1);
    SET(value, OPTYPE0, optype0);
    SET(value, EVENT3, event3);
    SET(value, EVENT2, event2);
    SET(value, EVENT1, event1);
    SET(value, EVENT0, event0);
    return value;
}

static inline uint64_t make_hpmevent_double(uint64_t mode,
    uint64_t optype0,
    uint64_t event1,
    uint64_t event0)
{
    return make_hpmevent_quad(mode, OPTYPE_OR, OPTYPE_OR, optype0, noEvent, noEvent, event1, event0);
}

static inline uint64_t make_hpmevent_single(uint64_t mode,
    uint64_t event)
{
    return make_hpmevent_quad(mode, OPTYPE_OR, OPTYPE_OR, OPTYPE_OR, noEvent, noEvent, noEvent, event);
}

#endif // __EVENT_H____
