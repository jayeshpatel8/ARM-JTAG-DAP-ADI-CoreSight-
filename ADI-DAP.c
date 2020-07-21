
#define DP_CTRL_STAT   BANK_REG(0x0, 0x4) /* DPv0+: rw */
#define SSTICKYERR   (1UL << 5)
#define CORUNDETECT   (1UL << 0)
#define CDBGPWRUPREQ   (1UL << 28)
#define CDBGPWRUPACK   (1UL << 29)
#define CSYSPWRUPREQ   (1UL << 30)
#define CSYSPWRUPACK   (1UL << 31)
/* MEM-AP register addresses */
#define MEM_AP_REG_CSW      0x00
#define MEM_AP_REG_TAR      0x04
#define MEM_AP_REG_TAR64    0x08        /* RW: Large Physical Address Extension */
#define MEM_AP_REG_DRW      0x0C        /* RW: Data Read/Write register */
#define MEM_AP_REG_BD0      0x10        /* RW: Banked Data register 0-3 */
#define MEM_AP_REG_BD1      0x14
#define MEM_AP_REG_BD2      0x18
#define MEM_AP_REG_BD3      0x1C
#define MEM_AP_REG_MBT      0x20        /* --: Memory Barrier Transfer register */
#define MEM_AP_REG_BASE64   0xF0        /* RO: Debug Base Address (LA) register */
#define MEM_AP_REG_CFG      0xF4        /* RO: Configuration register */
#define MEM_AP_REG_BASE     0xF8        /* RO: Debug Base Address register */
/* Generic AP register address */
#define AP_REG_IDR          0xFC        /* RO: Identification Register */
#define IDR_JEP106  (0x7FFUL << 17)
#define IDR_TYPE    (0xFUL << 0)
#define IDR_JEP106_ARM 0x04760000
#define IDR_CLASS   (0xFUL << 13)
#define DP_SELECT       BANK_REG(0x0, 0x8) /* DPv0+: JTAG: rw; SWD: wo */
/*
* MEM Access Port types
*/
typedef struct mem_ap {
    uint8_t jtag;  /* JTAG-AP - JTAG master for controlling other JTAG devices */
    uint8_t  ahb  ; /* AHB Memory-AP */
    uint8_t  apb  ;  /* APB Memory-AP */
    uint8_t  axi  ;  /* AXI Memory-AP */
} mem_ap_t;

/*
* Access Port classes
*/
enum ap_class {
AP_CLASS_NONE   = 0x00000,  /* No class defined */
AP_CLASS_MEM_AP = 0x10000,  /* MEM-AP */
};
/*
  * Access Port types
  */
  enum ap_type {
    AP_TYPE_JTAG_AP = 0x0,  /* JTAG-AP - JTAG master for controlling other JTAG devices */
    AP_TYPE_AHB_AP  = 0x1,  /* AHB Memory-AP */
    AP_TYPE_APB_AP  = 0x2,  /* APB Memory-AP */
    AP_TYPE_AXI_AP  = 0x4,  /* AXI Memory-AP */
    AP_TYPE_INVALID  = -1,  /* Invalid Memory-AP */
  };




Int main( void ){
     mem_ap_t mem_ap; /* list of mem ap */

    /* Init DAP DP */
     dap_dp_init();
  
    /* Scan and find out the AP offset for MEM-AP */
    dap_mem_ap_scan(&mem_ap);
    
  /* After finding valid mem-ap , select one accordingly */
  If (false == WriteResetReason( ResetReasonAddr ,  ValueToWrite )) {
       LOG(“ Writing Reset Reason Failed ”);
       Return ;
  }

  If (false == WriteMemMappedRegister(ResetRegisterAddr , ValueToWrite)){
              LOG(“Reset Command Execution Failed”);
              Return;
  }
  LOG(“Reset Executed Successfully”)
}
Bool WriteMemMappedRegister(Addr , Val ){
    If (AP_TYPE_INVALID  !=  mem_ap.apb)
    {
           dap_select_ap(mem_ap.apb) // this is implementation specific
           ap_write( MEM_AP_REG_TAR, Value=Addr)
           ap_write( MEM_AP_REG_DRW, Value=Val)
           ap_Capture(&AckDrw)
return is_write_Done() ;
      }
   return false;
}
bool WriteResetReason(ResetReasonAddr, ValueToWrite){
      If (AP_TYPE_INVALID  !=  mem_ap.ahb)
      {
            dap_select_ap(mem_ap.ahb)  // this is implementation specific
            ap_write( MEM_AP_REG_TAR, Value=ResetReasonAddr)
            ap_write( MEM_AP_REG_DRW, Value=ValueToWrite)
return is_write_Done() ;
      }  
     return false;
}
dap_dp_init(){
    
    int dp_ctrl_stat=0x0;
    
    // Enable Power Domain
    dp_ctrl_stat |= CDBGPWRUPREQ;
    dp_ctrl_stat |= CSYSPWRUPREQ;
    
    dp_write(DP_CTRL_STAT, &dp_ctrl_stat)
    
    LOG("DAP: wait CDBGPWRUPACK & CSYSPWRUPACK ");
    dp_poll(DP_CTRL_STAT , (CDBGPWRUPACK|CSYSPWRUPACK))
    
    dp_ctrl_stat=0x0;
 
    // clear sticky error
    dp_ctrl_stat |= SSTICKYERR;
 
    // Enable Overrun Detection
    dp_ctrl_stat |= CORUNDETECT
 
    dp_write(DP_CTRL_STAT, &dp_ctrl_stat)
}
dp_write(reg, val){
      Ir_write(DPACC)
      switch(reg)
      {
              Case: DP_AP_SELECT:                         
                        dr_write( val<<33 | (0x8 >> 2)<< 1)
      }
    
}

dap_select_ap(ap_num, ap_bank_id){
    dp_write(DP_AP_SELECT,( (ap_num<<24)|(ap_bank_id<<4)))
}
void    dap_mem_ap_scan(mem_ap_t * mem_ap)
{
      {
     int val = dap_find_memap(AP_TYPE_AHB_AP);
    if(val == AP_TYPE_INVALID  )  LOG(“Inavlid  AHB AP Found ”);
   mem_ap->ahb=val;
 
    val = dap_find_memap(AP_TYPE_APB_AP);
    if(val == AP_TYPE_INVALID  )  LOG(“Inavlid  APB AP Found ”);
    mem_ap->apb =val;    
 
    val = dap_find_memap(AP_TYPE_AXI_AP); 
    if(val == AP_TYPE_INVALID  ) LOG(“Inavlid  AXI AP Found ”);
    mem_ap->axi =val;        
 
    val = dap_find_memap(AP_TYPE_JTAG_AP);
    if(val == AP_TYPE_INVALID  )  LOG(“Inavlid  JTAG AP Found ”);
    mem_ap->jtag =val; 
       }   
}
int dap_find_memap(type_to_find)
{
     /* Maximum AP number is 255 since the SELECT register is 8 bits */
     for (uint32_t ap_num = 0; ap_num <= 255; ap_num++) {
        uint32_t id_val = 0;
        retval = dap_select_ap(ap_num,0xF )
        retval = ap_read(AP_REG_IDR,&id_val)
        
       /* IDR bits:
        * 31-28 : Revision
        * 27-24 : JEDEC bank (0x4 for ARM)
        * 23-17 : JEDEC code (0x3B for ARM)
        * 16-13 : Class (0b1000=Mem-AP)
        * 12-8  : Reserved
        *  7-4  : AP Variant (non-zero for JTAG-AP)
        *  3-0  : AP Type (0=JTAG-AP 1=AHB-AP 2=APB-AP 4=AXI-AP)
        */
        
        if ((retval == ERROR_OK) &&                  /* Register read success */
        ((id_val & IDR_JEP106) == IDR_JEP106_ARM) && /* Jedec codes match */
        ((id_val & IDR_CLASS) == AP_CLASS_MEM_AP) && /* Class Match */
        ((id_val & IDR_TYPE) == type_to_find)) {      /* type matches*/
    
            LOG("Found %s at AP index: %d (IDR=0x%08"  ")",
                    (type_to_find == AP_TYPE_AHB_AP)  ? "AHB-AP"  :
                    (type_to_find == AP_TYPE_APB_AP)  ? "APB-AP"  :
                    (type_to_find == AP_TYPE_AXI_AP)  ? "AXI-AP"  :
                    (type_to_find == AP_TYPE_JTAG_AP) ? "JTAG-AP" : "Unknown",
                    ap_num, id_val);
                
            return ap_num;
        }
     }
     return AP_TYPE_INVALID  ;
}
