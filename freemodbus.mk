## Objects that must be built in order to link
FREEMODBUS_SRC =	FreeModbus/modbus_lld/modbus_lld.c \
					FreeModbus/port/portserial.c \
					FreeModbus/port/portevent.c \
					FreeModbus/port/porttimer.c \
					FreeModbus/modbus/mb.c \
					FreeModbus/modbus/rtu/mbrtu.c \
					FreeModbus/modbus/rtu/mbcrc.c \
					FreeModbus/modbus/ascii/mbascii.c \
					FreeModbus/modbus/functions/mbfunccoils.c \
					FreeModbus/modbus/functions/mbfuncdiag.c \
					FreeModbus/modbus/functions/mbfuncholding.c \
					FreeModbus/modbus/functions/mbfuncinput.c \
					FreeModbus/modbus/functions/mbfuncother.c \
					FreeModbus/modbus/functions/mbfuncdisc.c \
					FreeModbus/modbus/functions/mbutils.c \
					FreeModbus/port/modbus_port.c \

# FreeModbus/modbus/tcp/mbtcp.c \
# FreeModbus/port/port.c \

FREEMODBUS_INC =	FreeModbus/modbus_lld \
					FreeModbus/port \
					FreeModbus/modbus/rtu \
					FreeModbus/modbus/ascii \
					FreeModbus/modbus/include \
					FreeModbus/modbus/tcp \


ALLCSRC += $(FREEMODBUS_SRC)
ALLINC += $(FREEMODBUS_INC)