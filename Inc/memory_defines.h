// Memory Definitions: Select only one! Note: 0°C = 273°K
//#define SRAM_ALLIANCE_AS7C34098A 1
//#define SRAM_ALLIANCE_AS7C38098A 1
//#define SRAM_CYPRESS_CY62157EV30 1
//#define SRAM_CYPRESS_CY7C1041CV33 1
//#define SRAM_CYPRESS_CY7C1051DV33 1
//#define SRAM_ISSI_IS61WV51216BLL
//#define MRAM_EVERSPIN_MR4A08BYS35 1
//#define MRAM_EVERSPIN_MR4A08BUYS45 1
//#define FRAM_CYPRESS_FM22L16_55_TG 1
//#define FRAM_ROHM_MR48V256CTAZAARL 1
//#define FRAM_FUJITSU_MB85R1001ANC_GE1 1
//#define RERAM_ADESTO_RM25C512C_LTAI_T 1
#define RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1 1

#define PARALLEL 0
#define SPI 1

#ifdef SRAM_ALLIANCE_AS7C34098A

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)12)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_ALLIANCE_AS7C38098A

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY62157EV30

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)45)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2200)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY7C1041CV33

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)8)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_CYPRESS_CY7C1051DV33

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef SRAM_ISSI_IS61WV51216BLL

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)10)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2400)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef MRAM_EVERSPIN_MR4A08BYS35

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)2097152)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)35)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)273)
#define MEM_OP_TEMP_MIN_K			((uint16_t)343)

#endif

#ifdef MRAM_EVERSPIN_MR4A08BUYS45

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)2097152)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)45)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)398)

#endif

#ifdef FRAM_CYPRESS_FM22L16_55_TG

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)262144)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)16)
#define MEM_ACCESS_TIME_NS			((uint32_t)55)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef FRAM_ROHM_MR48V256CTAZAARL

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)32768)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)70)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)2700)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef FRAM_FUJITSU_MB85R1001ANC_GE1

#define MEM_ACCESS_IF 				PARALLEL
#define MEM_SIZE_ADR 				((uint32_t)131072)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)3000)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef RERAM_ADESTO_RM25C512C_LTAI_T

#define MEM_ACCESS_IF 				SPI
#define MEM_SIZE_ADR 				((uint32_t)512)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)1024)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)1650)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif

#ifdef RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1
struct
{
	uint8_t wp_enable_Pin;
	uint8_t auto_power_down_enable;
	uint8_t low_power_standby_enable;
	uint8_t block_protection_bits;
	uint8_t write_enable_bit;
	uint8_t write_in_progress_bit;

} typedef MemoryStatusRegister;

#define MEM_ACCESS_IF 				SPI
#define MEM_SIZE_ADR 				((uint32_t)524288)
#define MEM_ACCESS_WIDTH_BIT		((uint8_t)8)
#define MEM_ACCESS_TIME_NS			((uint32_t)150)
#define MEM_SUPPLY_VOLTAGE_MIN_MV	((uint16_t)300)
#define MEM_SUPPLY_VOLTAGE_MAX_MV	((uint16_t)3600)
#define MEM_OP_TEMP_MAX_K			((uint16_t)233)
#define MEM_OP_TEMP_MIN_K			((uint16_t)358)

#endif
 
