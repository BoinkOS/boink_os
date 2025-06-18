#include "disk.h"
#include "../llio.h"
#include "../interrupts/pic.h"
#include "../drivers/vga_text/vga_text.h"
#include <stddef.h>

#define ATA_PRIMARY_IO	0x1F0
#define ATA_PRIMARY_CTRL  0x3F6

static disk_t disks[1];

volatile int ata_irq_invoked = 0;

void ata_irq_handler(uint32_t irq_num) {
	(void)irq_num;
	ata_irq_invoked = 1;
}

static void ata_wait_bsy() {
	while (pbin(ATA_PRIMARY_IO + 7) & ATA_SR_BSY);
}

static void ata_wait_drq() {
	while (!(pbin(ATA_PRIMARY_IO + 7) & ATA_SR_DRQ));
}

static int ata_read(uint32_t lba, uint8_t* buffer) {
	ata_irq_invoked = 0;

	ata_wait_bsy();

	// select drive and LBA
	pbout(ATA_PRIMARY_IO + 6, 0xF0 | ((lba >> 24) & 0x0F));
	pbout(ATA_PRIMARY_IO + 2, 1); // sector count
	pbout(ATA_PRIMARY_IO + 3, (uint8_t)(lba & 0xFF));
	pbout(ATA_PRIMARY_IO + 4, (uint8_t)((lba >> 8) & 0xFF));
	pbout(ATA_PRIMARY_IO + 5, (uint8_t)((lba >> 16) & 0xFF));
	pbout(ATA_PRIMARY_IO + 7, ATA_CMD_READ_SECTORS);

	// wait for IRQ handler to fire
	while (ata_irq_invoked == 0);

	ata_wait_bsy();
	ata_wait_drq();

	// read sector
	for (int i = 0; i < SECTOR_SIZE / 2; i++) {
		((uint16_t*)buffer)[i] = pwin(ATA_PRIMARY_IO);
	}

	return 0;
}

static int ata_write(uint32_t lba, const uint8_t* buffer) {
	ata_wait_bsy();

	pbout(ATA_PRIMARY_IO + 6, 0xE0 | ((lba >> 24) & 0x0F));
	pbout(ATA_PRIMARY_IO + 2, 1); // sector count
	pbout(ATA_PRIMARY_IO + 3, (uint8_t)(lba & 0xFF));
	pbout(ATA_PRIMARY_IO + 4, (uint8_t)((lba >> 8) & 0xFF));
	pbout(ATA_PRIMARY_IO + 5, (uint8_t)((lba >> 16) & 0xFF));
	pbout(ATA_PRIMARY_IO + 7, ATA_CMD_WRITE_SECTORS);

	ata_wait_bsy();

	for (int i = 0; i < SECTOR_SIZE / 2; i++) {
		pwout(ATA_PRIMARY_IO, ((uint16_t*)buffer)[i]);
	}

	// send cache flush (optional but nice to have)
	pbout(ATA_PRIMARY_IO + 7, 0xE7); // FLUSH CACHE
	ata_wait_bsy();

	return 0;
}

static disk_ops_t ata_ops = {
	.read = ata_read,
	.write = ata_write
};

void disk_init(void) {
	disks[0].ops = &ata_ops;
	disks[0].driver_data = NULL;
}

disk_t* disk_get(uint32_t index) {
	if (index >= 1) return NULL;
	return &disks[index];
}

int disk_read(uint32_t lba, uint8_t* buffer) {
	return disks[0].ops->read(lba, buffer);
}

int disk_write(uint32_t lba, const uint8_t* buffer) {
	return disks[0].ops->write(lba, buffer);
}