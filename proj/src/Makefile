# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/devices/
.PATH: ${.CURDIR}/devices/common
.PATH: ${.CURDIR}/devices/timer
.PATH: ${.CURDIR}/devices/kbc
.PATH: ${.CURDIR}/devices/gpu
.PATH: ${.CURDIR}/controllers/rtc
.PATH: ${.CURDIR}/devices/serial_port

# source code files to be compiled
SRCS = proj.c timer.c keyboard.c mouse.c mouse_sm.c gpu.c colors_utils.c utils.c kbc.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
