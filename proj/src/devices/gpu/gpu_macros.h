#ifndef GPU_MACROS_H
#define GPU_MACROS_H

#include <lcom/lcf.h>


/** 
 * (In AH register) When invoking VBE functions, to distinguish VBE function from VGA BIOS functions. 
 * - Returns VBE_FUNC in AL if function is suported
 * - Returns 0x00 in AH if function is successful
 * - Returns codes below in AH if function is unsuccessful
*/
#define INVOKE_VBE_FUNC 0x4F 

/** Unsuccessful call return codes */
#define AH_CALL_FAIL 0x01 /** VBE call failed */
#define Ah_FUNC_SUP_FAIL 0x02 /** VBE function is not supported in current HW configuration */
#define Ah_FUNC_INV_FAIL 0x03 /** VBE function is invalid in current video mode */


/** Color modes - from vmi.MemoryModel */
#define INDEXED 0x04 /** Indexed/Palette color mode (packed pixel) */
#define DIRECT_COLOR 0x06 /** Direct color mode*/


/** 
 * VBE modes - arguments of VBE function 0x02 - in register BX:
 * - screen res, color model, bits per pixel ((A:)R:G:B), number of colors
 */
// #define VBE_MODE_100 0x100 /** 640x400, ... - 256 colors */
// #define VBE_MODE_101 0x101 /** 640x480, ... - 256 colors */

// #define VBE_MODE_102 0x102 /** 800x600, ... - 16 colors */
// #define VBE_MODE_103 0x103 /** 800x600, ... - 256 colors */

// #define VBE_MODE_104 0x104 /** 1024x768, ... - 16 colors */
#define VBE_MODE_105 0x105 /** 1024x768, indexed, 8 - so color argument can't be bigger than 256 -> 256 colors */

// #define VBE_MODE_106 0x106 /** 1280x1024, ..., - 16 colors */
// #define VBE_MODE_107 0x107 /** 1280x1024, indexed, 8 - so color argument can't be bigger than 256 -> 256 colors */

// #define VBE_MODE_10D 0x10D /** 320x200, ..., 15 ((1:)5:5:5) - 32k colors */
// #define VBE_MODE_10E 0x10E /** 320x200, ..., 16 (5:6:5) 64k colors */
// #define VBE_MODE_10F 0x10F /** 320x200, ..., 24 (8:8:8) - 16.8M colors */

#define VBE_MODE_110 0x110 /** 640x480, direct color, 15((1:)5:5:5) - 32k colors */
// #define VBE_MODE_111 0x111 /** 640x480, ..., 16 (5:6:5) - 64k colors */
// #define VBE_MODE_112 0x112 /** 640x480, ..., 24 (8:8:8) - 16.8M colors */

// #define VBE_MODE_113 0x113 /** 800x600, ..., 15 ((1:)5:5:5) - 32k colors */
// #define VBE_MODE_114 0x114 /** 800x600, ..., 16 (5:6:5) - 64k colors */
#define VBE_MODE_115 0x115 /** 800x600, direct color, 24 (8:8:8) - 16.8M colors */

// #define VBE_MODE_116 0x116 /** 1024x768, ..., 15 ((1:)5:5:5) - 32k colors */
// #define VBE_MODE_117 0x117 /** 1024x768, ..., 16 (5:6:5) - 64k colors */
// #define VBE_MODE_118 0x118 /** 1024x768, ..., 24 (8:8:8) - 16.8M colors */

// #define VBE_MODE_119 0x119 /** 1280x1024, ..., 15 ((1):5:5:5) - 32k colors */
#define VBE_MODE_11A 0x11A /** 1280x1024, direct color, 16 (5:6:5) - 64k colors */

// #define VBE_MODE_11B 0x11B /** 1152x864, ..., 24 (8:8:8) - 16.8M colors */
#define VBE_MODE_14C 0x14C /** 1152x864, direct color, 32((8:)8:8:8) - 16.8M colors - NOT DEFINED IN VBE 2.0 STANDARD */


/** VBE functions (called using INT 10h interrupt vector, passed in AL as argument of 0x4F) */
/** 
 * Get VBE controller information. Args: 
 * - real-mode address of buffer to be filled with capabilties of controller in registers ES:DI
 */
#define VBE_CONTROLLER_INFO 0x00
/** 
 * Get VBE mode information. Args: 
 * - Mode in register CX 
 * - Real-mode address of buffer to be filled with parameters of specified mode (VRAM's physical address, among others) in registers ES:DI */
#define VBE_MODE_INFO 0x01
#define SET_VBE_MODE 0x02 /** Set VBE mode. Mode passed in BX register, which should have BIT(14) set */
#define SET_LINEAR_FB BIT(14) /** (In BX register) Set bit 14: use linear/flat framebuffer */
#define PRESERVE_DISP_MEM BIT(15) /** (In BX register) Set bit 15: preserve display memory */


/** Bios functions */
#define BIOS_VIDEO_MODE 0x00 /** Set video mode (In AH). Argument (video mode) in AL. Should use standard INT 0x10 BIOS interface */
#define MINIX_TEXT_MODE 0x03 /** Argument of 0x00. Minix text mode: standard CGA */


/** Software interrupt instructions INT xx - Access to BIOS' services. Executed in real-mode  */
#define INT_10 0x10 /** Video card */
#define INT_11 0x11 /** PC configuration */
#define INT_12 0x12 /** Memory configuration */
#define INT_16 0x16 /** Keyboard */


#endif /* GPU_MACROS_H */
