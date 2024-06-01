# name of the program (Minix service)
PROG=proj

.PATH: ${.CURDIR}/devices/
.PATH: ${.CURDIR}/devices/common
.PATH: ${.CURDIR}/devices/timer
.PATH: ${.CURDIR}/devices/kbc
.PATH: ${.CURDIR}/devices/gpu
.PATH: ${.CURDIR}/devices/rtc
.PATH: ${.CURDIR}/devices/serial_port
.PATH: ${.CURDIR}/objects
.PATH: ${.CURDIR}/objects/object_controllers

# source code files to be compiled
SRCS = proj.c game_state.c timer.c kbc.c keyboard.c mouse.c gpu.c rtc.c uart.c queue.c utils.c ball.c player.c goal.c button.c scoreboard.c timeboard.c cursor.c wall.c ball_controller.c player_controller.c sky_controller.c 

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS += -pedantic -D_LCOM_OPTIMIZED_

# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf

# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>
