#!/usr/bin/env python
# -*- coding: utf-8 -*-

from machine import Pin,SPI,PWM
import time

led = Pin(25, Pin.OUT)
## Configuration Registers */
CANSTAT      = 0x0E
CANCTRL      = 0x0F
BFPCTRL      = 0x0C
TEC          = 0x1C
REC          = 0x1D
CNF3         = 0x28
CNF2         = 0x29
CNF1         = 0x2A
CANINTE      = 0x2B
CANINTF      = 0x2C
EFLG         = 0x2D
TXRTSCTRL    = 0x0D

##  Recieve Filters */
RXF0SgearH     = 0x00
RXF0SgearL     = 0x01
RXF0Egear8     = 0x02
RXF0Egear0     = 0x03
RXF1SgearH     = 0x04
RXF1SgearL     = 0x05
RXF1Egear8     = 0x06
RXF1Egear0     = 0x07
RXF2SgearH     = 0x08
RXF2SgearL     = 0x09
RXF2Egear8     = 0x0A
RXF2Egear0     = 0x0B
RXF3SgearH     = 0x10
RXF3SgearL     = 0x11
RXF3Egear8     = 0x12
RXF3Egear0     = 0x13
RXF4SgearH     = 0x14
RXF4SgearL     = 0x15
RXF4Egear8     = 0x16
RXF4Egear0     = 0x17
RXF5SgearH     = 0x18
RXF5SgearL     = 0x19
RXF5Egear8     = 0x1A
RXF5Egear0     = 0x1B

## Receive Masks */
RXM0SgearH     = 0x20
RXM0SgearL     = 0x21
RXM0Egear8     = 0x22
RXM0Egear0     = 0x23
RXM1SgearH     = 0x24
RXM1SgearL     = 0x25
RXM1Egear8     = 0x26
RXM1Egear0     = 0x27

## Tx Buffer 0 */
TXB0CTRL     = 0x30
TXB0SgearH     = 0x31
TXB0SgearL     = 0x32
TXB0Egear8     = 0x33
TXB0Egear0     = 0x34
TXB0DLC      = 0x35
TXB0D0       = 0x36
TXB0D1       = 0x37
TXB0D2       = 0x38
TXB0D3       = 0x39
TXB0D4       = 0x3A
TXB0D5       = 0x3B
TXB0D6       = 0x3C
TXB0D7       = 0x3D

## Tx Buffer 1 */
TXB1CTRL     = 0x40
TXB1SgearH     = 0x41
TXB1SgearL     = 0x42
TXB1Egear8     = 0x43
TXB1Egear0     = 0x44
TXB1DLC      = 0x45
TXB1D0       = 0x46
TXB1D1       = 0x47
TXB1D2       = 0x48
TXB1D3       = 0x49
TXB1D4       = 0x4A
TXB1D5       = 0x4B
TXB1D6       = 0x4C
TXB1D7       = 0x4D

## Tx Buffer 2 */
TXB2CTRL     = 0x50
TXB2SgearH     = 0x51
TXB2SgearL     = 0x52
TXB2Egear8     = 0x53
TXB2Egear0     = 0x54
TXB2DLC      = 0x55
TXB2D0       = 0x56
TXB2D1       = 0x57
TXB2D2       = 0x58
TXB2D3       = 0x59
TXB2D4       = 0x5A
TXB2D5       = 0x5B
TXB2D6       = 0x5C
TXB2D7       = 0x5D

# ## Rx Buffer 0 */
RXB0CTRL     = 0x60
RXB0SgearH     = 0x61
RXB0SgearL     = 0x62
RXB0Egear8     = 0x63
RXB0Egear0     = 0x64
RXB0DLC      = 0x65
RXB0D0       = 0x66
RXB0D1       = 0x67
RXB0D2       = 0x68
RXB0D3       = 0x69
RXB0D4       = 0x6A
RXB0D5       = 0x6B
RXB0D6       = 0x6C
RXB0D7       = 0x6D

# ## Rx Buffer 1 */
RXB1CTRL     = 0x70
RXB1SgearH     = 0x71
RXB1SgearL     = 0x72
RXB1Egear8     = 0x73
RXB1Egear0     = 0x74
RXB1DLC      = 0x75
RXB1D0       = 0x76
RXB1D1       = 0x77
RXB1D2       = 0x78
RXB1D3       = 0x79
RXB1D4       = 0x7A
RXB1D5       = 0x7B
RXB1D6       = 0x7C
RXB1D7       = 0x7D


# ##******************************************************************
 # *               Bit register masks                                *
 # *******************************************************************/

# ## TXBnCTRL */
TXREQ        = 0x08
TXP          = 0x03

# ## RXBnCTRL */
RXM          = 0x60
BUKT         = 0x04

# ## CANCTRL */
REQOP        = 0xE0
ABAT         = 0x10
#define	OSM          = 0x08
CLKEN        = 0x04
CLKPRE       = 0x03

# ## CANSTAT */
REQOP        = 0xE0
ICOD         = 0x0E

## CANINTE */
RX0IE        = 0x01
RX1IE        = 0x02
TX0IE        = 0x04
TX1IE        = 0x80
TX2IE        = 0x10
ERRIE        = 0x20
WAKIE        = 0x40
MERRE        = 0x80

# ## CANINTF */
RX0IF        = 0x01
RX1IF        = 0x02
TX0IF        = 0x04
TX1IF        = 0x80
TX2IF        = 0x10
ERRIF        = 0x20
WAKIF        = 0x40
MERRF        = 0x80

# ## BFPCTRL */
B1BFS        = 0x20
B0BFS        = 0x10
B1BFE        = 0x08
B0BFE        = 0x04
B1BFM        = 0x02
B0BFM        = 0x01

# ## CNF1 Masks */
SJW          = 0xC0
BRP          = 0x3F

# ## CNF2 Masks */
BTLMODE      = 0x80
SAM          = 0x40
PHSEG1       = 0x38
PRSEG        = 0x07

# ## CNF3 Masks */
WAKFIL       = 0x40
PHSEG2       = 0x07

# ## TXRTSCTRL Masks */
TXB2RTS      = 0x04
TXB1RTS      = 0x02
TXB0RTS      = 0x01


# ##******************************************************************
 # *                    Bit Timing Configuration                     *
 # *******************************************************************/
 
# ## CNF1 */
SJW_1TQ      = 0x40
SJW_2TQ      = 0x80
SJW_3TQ      = 0x90
SJW_4TQ      = 0xC0

# ## CNF2 */
BTLMODE_CNF3 = 0x80
BTLMODE_PH1_IPT  = 0x00

SMPL_3X      = 0x40
SMPL_1X      = 0x00

PHSEG1_8TQ   = 0x38
PHSEG1_7TQ   = 0x30
PHSEG1_6TQ   = 0x28
PHSEG1_5TQ   = 0x20
PHSEG1_4TQ   = 0x18
PHSEG1_3TQ   = 0x10
PHSEG1_2TQ   = 0x08
PHSEG1_1TQ   = 0x00

PRSEG_8TQ    = 0x07
PRSEG_7TQ    = 0x06
PRSEG_6TQ    = 0x05
PRSEG_5TQ    = 0x04
PRSEG_4TQ    = 0x03
PRSEG_3TQ    = 0x02
PRSEG_2TQ    = 0x01
PRSEG_1TQ    = 0x00

# ## CNF3 */
PHSEG2_8TQ   = 0x07
PHSEG2_7TQ   = 0x06
PHSEG2_6TQ   = 0x05
PHSEG2_5TQ   = 0x04
PHSEG2_4TQ   = 0x03
PHSEG2_3TQ   = 0x02
PHSEG2_2TQ   = 0x01
PHSEG2_1TQ   = 0x00

SOF_ENABLED  = 0x80
WAKFIL_ENABLED  =0x40
WAKFIL_DISABLED =0x00


# ##******************************************************************
 # *                  Control/Configuration Registers                *
 # *******************************************************************/

# ## CANINTE */
RX0IE_ENABLED= 0x01
RX0IE_DISABLED  =0x00
RX1IE_ENABLED =0x02
RX1IE_DISABLED  =0x00
G_RXIE_ENABLED  =0x03
G_RXIE_DISABLED =0x00

TX0IE_ENABLED= 0x04
TX0IE_DISABLED  =0x00
TX1IE_ENABLED =0x08
TX2IE_DISABLED  =0x00
TX2IE_ENABLED =0x10
TX2IE_DISABLED  =0x00
G_TXIE_ENABLED  =0x1C
G_TXIE_DISABLED =0x00

ERRIE_ENABLED= 0x20
ERRIE_DISABLED  =0x00
WAKIE_ENABLED= 0x40
WAKIE_DISABLED  =0x00
IVRE_ENABLED = 0x80
IVRE_DISABLED= 0x00

# ## CANINTF */
RX0IF_SET    = 0x01
RX0IF_RESET  = 0x00
RX1IF_SET    = 0x02
RX1IF_RESET  = 0x00
TX0IF_SET    = 0x04
TX0IF_RESET  = 0x00
TX1IF_SET    = 0x08
TX2IF_RESET  = 0x00
TX2IF_SET    = 0x10
TX2IF_RESET  = 0x00
ERRIF_SET    = 0x20
ERRIF_RESET  = 0x00
WAKIF_SET    = 0x40
WAKIF_RESET  = 0x00
IVRF_SET     = 0x80
IVRF_RESET   = 0x00

# ## CANCTRL */ 
REQOP_CONFIG = 0x80
REQOP_LISTEN = 0x60
REQOP_LOOPBACK  =0x40
REQOP_SLEEP  = 0x20
REQOP_NORMAL = 0x00

ABORT        = 0x10

OSM_ENABLED  = 0x08

CLKOUT_ENABLED  =0x04
CLKOUT_DISABLED =0x00
CLKOUT_PRE_8 = 0x03
CLKOUT_PRE_4 = 0x02
CLKOUT_PRE_2 = 0x01
CLKOUT_PRE_1 = 0x00

# ## CANSTAT */
OPMODE_CONFIG= 0x80
OPMODE_LISTEN= 0x60
OPMODE_LOOPBACK =0x40
OPMODE_SLEEP = 0x20
OPMODE_NORMAL= 0x00


# ## RXBnCTRL */
RXM_RCV_ALL  = 0x60
RXM_VALgear_EXT= 0x40
RXM_VALgear_STD= 0x20
RXM_VALgear_ALL= 0x00

RXRTR_REMOTE = 0x08
RXRTR_NO_REMOTE =0x00

BUKT_ROLLOVER = 0x04
BUKT_NO_ROLLOVER =0x00

FILHIT0_FLTR_1  =0x01
FILHIT0_FLTR_0  =0x00

FILHIT1_FLTR_5  =0x05
FILHIT1_FLTR_4  =0x04
FILHIT1_FLTR_3  =0x03
FILHIT1_FLTR_2  =0x02
FILHIT1_FLTR_1  =0x01
FILHIT1_FLTR_0  =0x00


# ## TXBnCTRL */
TXREQ_SET    = 0x08
TXREQ_CLEAR  = 0x00

TXP_HIGHEST  = 0x03
TXP_INTER_HIGH  =0x02
TXP_INTER_LOW= 0x01
TXP_LOWEST   = 0x00
    

# ##******************************************************************
 # *                  Register Bit Masks                             *
 # *******************************************************************/
 
DLC_0       = 0x00
DLC_1       = 0x01
DLC_2       = 0x02
DLC_3       = 0x03
DLC_4       = 0x04
DLC_5       = 0x05
DLC_6       = 0x06
DLC_7       = 0x07    
DLC_8       = 0x08


# ##******************************************************************
 # *                  CAN spi commands                               *
 # *******************************************************************/

CAN_RESET    = 0xC0
CAN_READ     = 0x03
CAN_WRITE    = 0x02
CAN_RTS      = 0x80
CAN_RTS_TXB0 = 0x81
CAN_RTS_TXB1 = 0x82
CAN_RTS_TXB2 = 0x84
CAN_RD_STATUS= 0xA0
CAN_BIT_MODIFY  =0x05  
CAN_RX_STATUS= 0xB0
CAN_RD_RX_BUFF  =0x90
CAN_LOAD_TX  = 0x40  


# ##******************************************************************
 # *                  Miscellaneous                                  *
 # *******************************************************************/

DUMMY_BYTE   = 0x00
TXB0         = 0x31
TXB1         = 0x41
TXB2         = 0x51
RXB0         = 0x61
RXB1         = 0x71
EXgearE_SET    = 0x08
EXgearE_RESET  = 0x00
# #CS   PORTAbits.RA2

CAN_RATE = {
    "5KBPS"   : [0xA7, 0XBF, 0x07],
    "10KBPS"  : [0x31, 0XA4, 0X04],
    "20KBPS"  : [0x18, 0XA4, 0x04],
    "50KBPS"  : [0x09, 0XA4, 0x04],
    "100KBPS" : [0x04, 0x9E, 0x03],
    "125KBPS" : [0x03, 0x9E, 0x03],
    "250KBPS" : [0x01, 0x1E, 0x03],
    "500KBPS" : [0x00, 0x9E, 0x03],
    "800KBPS" : [0x00, 0x92, 0x02],
    "1000KBPS": [0x00, 0x82, 0x02],
}

gRXFlag = 0
sRXFlag = 0
yRXFlag = 0
Com_RecBuff = [0, 0, 0, 0, 0, 0, 0, 0]  # 8

SPI0_CS1 = 1
SPI0_CS0 = 5

debug = True

class MCP2515():
    def __init__(self):
        self.spi = SPI(0)
        self.spi = SPI(0, 10000_000, polarity=0, phase=0, sck=Pin(6), mosi=Pin(7), miso=Pin(4))
        self.cs = Pin(SPI0_CS0, Pin.OUT)

    def ReadByte(self, addr):
        self.cs(0)
        self.spi.write(bytearray([CAN_READ]))
        self.spi.write(bytearray([addr]))
        res = self.spi.read(1)
        self.cs(1)
        return int.from_bytes(res, 'big')

    def WriteByte(self, addr):
        self.cs(0)
        self.spi.write(bytearray([addr]))
        self.cs(1)

    def WriteBytes(self, addr, data):
        self.cs(0)
        self.spi.write(bytearray([CAN_WRITE]))
        self.spi.write(bytearray([addr]))
        self.spi.write(bytearray([data]))
        self.cs(1)

    def Reset(self):
        self.cs(0)
        self.spi.write(bytearray([CAN_RESET]))  # Reset 0xC0
        self.cs(1)

    def Init(self, speed="1000KBPS"):
        print("Reset")
        self.Reset()
        time.sleep(0.1)

        self.WriteBytes(CNF1, CAN_RATE[speed][0])
        self.WriteBytes(CNF2, CAN_RATE[speed][1])
        self.WriteBytes(CNF3, CAN_RATE[speed][2])

        self.WriteBytes(TXB0SgearH, 0xFF)
        self.WriteBytes(TXB0SgearL, 0xE0)
        self.WriteBytes(TXB0DLC, 0x40 | DLC_8)

        self.WriteBytes(RXB0SgearH, 0x00)
        self.WriteBytes(RXB0SgearL, 0x60)
        self.WriteBytes(RXB0CTRL, 0x60)
        self.WriteBytes(RXB0DLC, DLC_8)

        self.WriteBytes(RXF0SgearH, 0xFF)
        self.WriteBytes(RXF0SgearL, 0xE0)
        self.WriteBytes(RXM0SgearH, 0xFF)
        self.WriteBytes(RXM0SgearL, 0xE0)

        self.WriteBytes(CANINTF, 0x00)
        self.WriteBytes(CANINTE, 0x01)

        self.WriteBytes(CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED)

        dummy = self.ReadByte(CANSTAT)
        if (OPMODE_NORMAL != (dummy and 0xE0)):
            self.WriteBytes(CANCTRL, REQOP_NORMAL | CLKOUT_ENABLED)

    def Send(self, CAN_gear, CAN_TX_Buf, length1):
        tempdata = self.ReadByte(CAN_RD_STATUS)
        self.WriteBytes(TXB0SgearH, (CAN_gear >> 3) & 0xFF)
        self.WriteBytes(TXB0SgearL, (CAN_gear & 0x07) << 5)

        self.WriteBytes(TXB0Egear8, 0)
        self.WriteBytes(TXB0Egear0, 0)
        self.WriteBytes(TXB0DLC, length1)

        for j in range(length1):
            self.WriteBytes(TXB0D0 + j, CAN_TX_Buf[j])

        if (tempdata & 0x04):  # TXREQ
            time.sleep(0.01)
            self.WriteBytes(TXB0CTRL, 0)
            while True:
                if (self.ReadByte(CAN_RD_STATUS) & 0x04 != 1):
                    break
        self.WriteByte(CAN_RTS_TXB0)


if __name__ == '__main__':
    print("--------------------------------------------------------")
    can = MCP2515()
    print("init...")
    can.Init()
    print("send data...")

    speed = 0x370
    rpm = 0x360
    voltage = 0x372
    pressure = 0x361
    temperature = 0x3E0
    gear = 0x470
    enginelight = 0x3E4
    enginedata = [1, 2, 3, 4, 5, 6, 7, 0b10000000]
    data = [1, 2, 3, 4, 5, 6, 7, 8]
    dlc = 8
    
    rpmdata = [0, 0, 3, 4, 5, 6, 7, 8]
    rpm_value = 0
    
    
    speeddata = [0, 0, 9, 9, 9, 9, 9, 9]
    temppressdata = [0, 0, 9, 9, 9, 9]
    
    speed_value = 0
    temppress = 150

    can.Send(gear, data, dlc)

    readbuf = []

    led = Pin(25, Pin.OUT)
    led.on()
    time.sleep(1)
    led.off()

    i = 0
    flashing = False
    while True:
        #print("AHHH")
        led.on()
        #can.Send(gear, data, dlc)
        rpm_value = (rpm_value + 1) % 900
        rpmdata[0] = (rpm_value >> 8) & 0xFF  
        rpmdata[1] = rpm_value & 0xFF
        can.Send(rpm, rpmdata, dlc)
        
        speed_value = (speed_value + 5) % 3550
        speeddata[0] = (speed_value >> 8) & 0xFF  
        speeddata[1] = speed_value & 0xFF
        speeddata[2] = (speed_value >> 8) & 0xFF  
        speeddata[3] = speed_value & 0xFF
        speeddata[6] = (speed_value >> 8) & 0xFF  
        speeddata[7] = speed_value & 0xFF
        print(str(speeddata[0]) + "" + str(speeddata[1]))
        can.Send(speed, speeddata, dlc)
        can.Send(voltage, speeddata, dlc)
        #temppress = (temppress + 5) % 1200 + 150
        #temppressdata[0] = (temppress >> 8) & 0xFF  
        #temppressdata[1] = temppress & 0xFF
        #temppressdata[6] = (temppress >> 8) & 0xFF  
        #temppressdata[7] = temppress & 0xFF
        can.Send(pressure, speeddata, dlc)
        can.Send(temperature, speeddata, dlc)
        i += 1
        if i >= 100:
            i = 0
            flashing = not flashing 
        if flashing:
            print("flashing")
            enginedata[7] = 0b10000000
            enginedata[6] = 3 
            can.Send(enginelight, enginedata, dlc)
            can.Send(gear, enginedata, dlc)
        else:
            print("working")
            enginedata[7] = 0b01111111
            enginedata[6] = 5
            can.Send(enginelight, enginedata, dlc)
            can.Send(gear, enginedata, dlc)
        led.off()
        time.sleep(0.01)
        #print("help")

    print("--------------------------------------------------------")
