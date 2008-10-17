#ifndef UD_DECODE_H
#define UD_DECODE_H

#define MAX_INSN_LENGTH 15

/* register classes */
#define T_NONE  0
#define T_GPR   1
#define T_MMX   2
#define T_CRG   3
#define T_DBG   4
#define T_SEG   5
#define T_XMM   6

/* itab prefix bits */
#define P_none          ( 0 )
#define P_c1            ( 1 << 0 )
#define P_C1(n)         ( ( n >> 0 ) & 1 )
#define P_rexb          ( 1 << 1 )
#define P_REXB(n)       ( ( n >> 1 ) & 1 )
#define P_depM          ( 1 << 2 )
#define P_DEPM(n)       ( ( n >> 2 ) & 1 )
#define P_c3            ( 1 << 3 )
#define P_C3(n)         ( ( n >> 3 ) & 1 )
#define P_inv64         ( 1 << 4 )
#define P_INV64(n)      ( ( n >> 4 ) & 1 )
#define P_rexw          ( 1 << 5 )
#define P_REXW(n)       ( ( n >> 5 ) & 1 )
#define P_c2            ( 1 << 6 )
#define P_C2(n)         ( ( n >> 6 ) & 1 )
#define P_def64         ( 1 << 7 )
#define P_DEF64(n)      ( ( n >> 7 ) & 1 )
#define P_rexr          ( 1 << 8 )
#define P_REXR(n)       ( ( n >> 8 ) & 1 )
#define P_oso           ( 1 << 9 )
#define P_OSO(n)        ( ( n >> 9 ) & 1 )
#define P_aso           ( 1 << 10 )
#define P_ASO(n)        ( ( n >> 10 ) & 1 )
#define P_rexx          ( 1 << 11 )
#define P_REXX(n)       ( ( n >> 11 ) & 1 )
#define P_ImpAddr       ( 1 << 12 )
#define P_IMPADDR(n)    ( ( n >> 12 ) & 1 )

/* rex prefix bits */
#define REX_W(r)        ( ( 0xF & ( r ) )  >> 3 )
#define REX_R(r)        ( ( 0x7 & ( r ) )  >> 2 )
#define REX_X(r)        ( ( 0x3 & ( r ) )  >> 1 )
#define REX_B(r)        ( ( 0x1 & ( r ) )  >> 0 )
#define REX_PFX_MASK(n) ( ( P_REXW(n) << 3 ) | \
                          ( P_REXR(n) << 2 ) | \
                          ( P_REXX(n) << 1 ) | \
                          ( P_REXB(n) << 0 ) )

/* scable-index-base bits */
#define SIB_S(b)        ( ( b ) >> 6 )
#define SIB_I(b)        ( ( ( b ) >> 3 ) & 7 )
#define SIB_B(b)        ( ( b ) & 7 )

/* modrm bits */
#define MODRM_REG(b)    ( ( ( b ) >> 3 ) & 7 )
#define MODRM_NNN(b)    ( ( ( b ) >> 3 ) & 7 )
#define MODRM_MOD(b)    ( ( ( b ) >> 6 ) & 3 )
#define MODRM_RM(b)     ( ( b ) & 7 )

/* operand type constants -- order is important! */

enum __attribute__((packed)) ud_operand_code {
    OP_NONE,

    OP_A,      OP_E,      OP_M,       OP_G,       
    OP_I,

    OP_AL,     OP_CL,     OP_DL,      OP_BL,
    OP_AH,     OP_CH,     OP_DH,      OP_BH,

    OP_ALr8b,  OP_CLr9b,  OP_DLr10b,  OP_BLr11b,
    OP_AHr12b, OP_CHr13b, OP_DHr14b,  OP_BHr15b,

    OP_AX,     OP_CX,     OP_DX,      OP_BX,
    OP_SI,     OP_DI,     OP_SP,      OP_BP,

    OP_rAX,    OP_rCX,    OP_rDX,     OP_rBX,  
    OP_rSP,    OP_rBP,    OP_rSI,     OP_rDI,

    OP_rAXr8,  OP_rCXr9,  OP_rDXr10,  OP_rBXr11,  
    OP_rSPr12, OP_rBPr13, OP_rSIr14,  OP_rDIr15,

    OP_eAX,    OP_eCX,    OP_eDX,     OP_eBX,
    OP_eSP,    OP_eBP,    OP_eSI,     OP_eDI,

    OP_ES,     OP_CS,     OP_SS,      OP_DS,  
    OP_FS,     OP_GS,

    OP_ST0,    OP_ST1,    OP_ST2,     OP_ST3,
    OP_ST4,    OP_ST5,    OP_ST6,     OP_ST7,

    OP_J,      OP_S,      OP_O,          
    OP_I1,     OP_I3, 

    OP_V,      OP_W,      OP_Q,       OP_P, 

    OP_R,      OP_C,  OP_D,       OP_VR,  OP_PR
};


/* operand size constants */

enum __attribute__((packed)) ud_operand_size {
    SZ_NA  = 0,
    SZ_Z   = 1,
    SZ_V   = 2,
    SZ_P   = 3,
    SZ_WP  = 4,
    SZ_DP  = 5,
    SZ_MDQ = 6,
    SZ_RDQ = 7,

    /* the following values are used as is,
     * and thus hard-coded. changing them 
     * will break internals 
     */
    SZ_B   = 8,
    SZ_W   = 16,
    SZ_D   = 32,
    SZ_Q   = 64,
    SZ_T   = 80,
    SZ_O   = 128,
};


/* A single operand of an entry in the instruction table. 
 * (internal use only)
 */
struct ud_itab_entry_operand 
{
  enum ud_operand_code type;
  enum ud_operand_size size;
};


/* A single entry in an instruction table. 
 *(internal use only)
 */
struct ud_itab_entry 
{
  enum ud_mnemonic_code         mnemonic;
  struct ud_itab_entry_operand  operand1;
  struct ud_itab_entry_operand  operand2;
  struct ud_itab_entry_operand  operand3;
  uint32_t                      prefix;
};

extern const char * ud_lookup_mnemonic( enum ud_mnemonic_code c );

#endif /* UD_DECODE_H */

/* vim:cindent
 * vim:expandtab
 * vim:ts=4
 * vim:sw=4
 */
