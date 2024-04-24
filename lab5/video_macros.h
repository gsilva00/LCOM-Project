#ifndef VIDEO_MACROS_H
#define VIDEO_MACROS_H

#include <lcom/lcf.h>


/** (In AH register) When invoking VBE functions, to distinguish VBE function from VGA BIOS functions. 
 * - Returns VBE_FUNC in AL if function is suported
 * - Returns 0x00 in AH if function is successful
 * - Returns codes below in AH if function is unsuccessful
*/
#define INVOKE_VBE_FUNC 0x4F 

/** Unsuccessful call return codes */
#define AH_CALL_FAIL 0x01 /** VBE call failed */
#define Ah_FUNC_SUP_FAIL 0x02 /** VBE function is not supported in current HW configuration */
#define Ah_FUNC_INV_FAIL 0x03 /** VBE function is invalid in current video mode */

/** VBE modes - screen res, color model, bits per pixel ((A:)R:G:B) */
#define VBE_MODE_105 0x105 /** 1024x768, indexed, 8 per RGB */
#define VBE_MODE_110 0x110 /** 640x480, direct color, 15((1:)5:5:5) */
#define VBE_MODE_115 0x115 /** 800x600, direct color, 24 (8:8:8) */
#define VBE_MODE_11A 0x11A /** 1280x1024, direct color, 16 (5:6:5) */
#define VBE_MODE_14C 0x14C /** 1152x864, direct color, 32((8:)8:8:8) */

/** VBE functions (passed in AL as argument of 0x4F) */
/** Get VBE controller information. Args: 
 * - real-mode address of buffer to be filled with capabilties of controller in registers ES:DI*/
#define VBE_CONTROLLER_INFO 0x00
/** Get VBE mode information. Args: 
 * - Mode in register CX 
 * - real-mode address of buffer to be filled with parameters of specified mode (VRAM's physical address, among others) in registers ES:DI */
#define VBE_MODE_INFO 0x01
#define SET_VBE_MODE 0x02 /** Set VBE mode. Mode passed in BX register, which should have BIT(14) set */
#define SET_LINEAR_FB BIT(14) /** (In BX register) Set bit 14: linear framebuffer */

/** Bios functions */
#define BIOS_VIDEO_MODE 0x00 /** Set video mode. (In AH) Argument (video mode) in AL. Should use standard INT 0x10 BIOS interface */
#define MINIX_TEXT_MODE 0x03 /** Minix text mode */

/** Software interrupt instructions INT xx - Access to BIOS' services. Executed in real-mode  */
#define INT_10 0x10 /** Video card */
#define INT_11 0x11 /** PC configuration */
#define INT_12 0x12 /** Memory configuration */
#define INT_16 0x16 /** Keyboard */

#endif /* VIDEO_MACROS_H */
