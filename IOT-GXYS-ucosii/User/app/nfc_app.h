#ifndef _NFC_APP_H_
#define _NFC_APP_H_
#include <stdint.h>


struct NFC_DATA_ST{
	uint8_t work_mode :1;
	uint8_t start_flag :1;
	uint8_t book_back_lock :1;
	uint8_t update_flag :1;
	uint8_t wtn_pick_up_flag :1;
	uint8_t revser :3;
};

void nfc_flag_init(void);

#endif
