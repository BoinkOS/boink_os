#include <stdint.h>

#define SECTOR_SIZE 512
#define ATA_SR_BSY	 0x80 // Busy
#define ATA_SR_DRDY	0x40 // Drive ready
#define ATA_SR_DRQ	 0x08 // Data request (ready to transfer)
#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30

typedef struct {
	int (*read)(uint32_t lba, uint8_t* buffer);
	int (*write)(uint32_t lba, const uint8_t* buffer);
} disk_ops_t;

typedef struct {
	disk_ops_t* ops;
	void* driver_data; // ata-specific data later
} disk_t;

disk_t* disk_get(uint32_t index);
int disk_read(uint32_t lba, uint8_t* buffer);
int disk_write(uint32_t lba, const uint8_t* buffer);
void ata_irq_handler(uint32_t irq_num);
void disk_init(void);