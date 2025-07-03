#include "../../klib/console/console.h"
#include "../../fs/glfs.h"
#include "../../drivers/video/fb.h"
#include "../../drivers/video/text.h"
#include "../../utils.h"
#include "squint.h"

void squint_shell(int argc, char** argv) {
	char* fname = argv[1];
	int findex = glfs_find_file_index(fname);

	if (findex < 0) {
		console_println("File not found.");
		return;
	}

	squint(findex, fname);
}

void render_bmp_image(uint8_t* bmp) {
	BITMAPFILEHEADER* fileHeader = (BITMAPFILEHEADER*)bmp;
	BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(bmp + sizeof(BITMAPFILEHEADER));

	if (fileHeader->bfType != 0x4D42) {
		console_println("not a valid BMP file");
		return;
	}

	int rawHeight = infoHeader->biHeight;
	bool topDown  = rawHeight < 0;
	int height    = rawHeight>0 ? rawHeight : -rawHeight;
	int width = infoHeader->biWidth;
	int bpp = infoHeader->biBitCount;

	if (bpp != 24 && bpp != 32) {
		console_print("unsupported bit depth (");
		console_print_dec(bpp);
		console_println(")");
		return;
	}

	uint8_t* pixelData = bmp + fileHeader->bfOffBits;
	int bytesPerPixel = bpp / 8;
	int rowSize = ((width * bytesPerPixel + 3) & ~3); // pad to nearest 4 bytes

	for (int y = 0; y < height; y++) {
		int row = topDown ? y : height-1-y;

		for (int x = 0; x < width; x++) {
			uint8_t* px = pixelData + row * rowSize + x * bytesPerPixel;

			uint8_t blue = px[0];
			uint8_t green = px[1];
			uint8_t red = px[2];
			//uint8_t alpha = (bpp == 32) ? px[3] : 0xFF;

			uint32_t color = (red << 16) | (green << 8) | blue;

			put_pixel(x, y, color);
		}
	}
}

void draw_status_bar(const char* fname) {
	const uint32_t fg = 0xFFFFFF;
	const uint32_t bg = 0x000000;

	char status[128] = "";
	strcat(status, "S.Q.U.I.N.T / ");
	strcat(status, fname);
	strcat(status, " / q to quit");

	draw_string(25, 25, fg, bg, status);
}

void squint(int findex, const char *fname) {
	console_println("S.Q.U.I.N.T. (Small Quick Unstable Image-Navigating Thing)");

	uint8_t* bmp = (uint8_t*)glfs_load_file_to_address(findex, IMG_BASE);
	if (!bmp) {
		console_println("failed to load image file");
		return;
	}

	BITMAPFILEHEADER* fileHeader = (BITMAPFILEHEADER*)bmp;
	BITMAPINFOHEADER* infoHeader = (BITMAPINFOHEADER*)(bmp + sizeof(BITMAPFILEHEADER));

	if (fileHeader->bfType != 0x4D42) {
		console_println("Not a bitmap image.");
		return;
	}

	int bpp = infoHeader->biBitCount;

	console_print("\nFile OK (");
	console_print_dec(bpp);
	console_print("-bit BMP). Display \"");
	console_print(fname);
	console_print("\"? (y/n) ");

	char conf = read_key();
	console_set_color(0x9019ff);
	console_putc(conf);
	console_set_color(0xFFFFFF);
	console_putc('\n');

	if (conf != 'Y' && conf != 'y') {
		console_println("cancelled.");
		return;
	}

	render_bmp_image(bmp);

	draw_status_bar(fname);

	char inp = ' ';
	while (inp != 'q') {
		inp = read_key();
	}

	console_init();
	console_set_color(0xFFFFFF);
	console_set_background_color(0x000000);
	console_clear();
	console_print("S.Q.U.I.N.T: ");
	console_set_color(0x9019ff);
	console_println("\"How did you do this?\"");
	console_set_color(0xffffff);
	console_print("S.Q.U.I.N.T: ");
	console_set_color(0x9019ff);
	console_println("\"It's very simple -- you read the protocol and write the code.\" - Bill Joy");
	console_set_color(0xffffff);

	console_print("S.Q.U.I.N.T: ");
	console_print(fname);
	console_println(" / closed.");
	console_set_title("Boink Operating System");
}