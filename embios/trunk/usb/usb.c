//
//
//    Copyright 2010 TheSeven
//
//
//    This file is part of emBIOS.
//
//    emBIOS is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emBIOS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emBIOS.  If not, see <http://www.gnu.org/licenses/>.
//
//


#include "global.h"
#include "panic.h"
#include "usb.h"
#include "usb_ch9.h"
#include "usbdrv.h"
#include "thread.h"
#include "console.h"
#include "util.h"
#include "i2c.h"


static uint8_t ctrlresp[2] CACHEALIGN_ATTR;
static uint32_t dbgrecvbuf[0x80] CACHEALIGN_ATTR;
static uint32_t dbgsendbuf[0x80] CACHEALIGN_ATTR;
static uint32_t dbgasyncsendbuf[0x80] CACHEALIGN_ATTR;
static char dbgendpoints[4] IBSS_ATTR;

enum dbgaction_t
{
    DBGACTION_IDLE = 0,
    DBGACTION_I2CSEND,
    DBGACTION_I2CRECV,
    DBGACTION_POWEROFF
};

static uint32_t dbgstack[0x100] STACK_ATTR;
struct wakeup dbgwakeup IBSS_ATTR;
extern struct scheduler_thread* scheduler_threads;
static enum dbgaction_t dbgaction IBSS_ATTR;
static int dbgi2cbus IBSS_ATTR;
static int dbgi2cslave IBSS_ATTR;
static int dbgi2caddr IBSS_ATTR;
static int dbgi2clen IBSS_ATTR;


static struct usb_device_descriptor CACHEALIGN_ATTR device_descriptor =
{
    .bLength            = sizeof(struct usb_device_descriptor),
    .bDescriptorType    = USB_DT_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_VENDOR_SPEC,
    .bDeviceSubClass    = 0xff,
    .bDeviceProtocol    = 0xff,
    .bMaxPacketSize0    = 64,
    .idVendor           = 0xffff,
    .idProduct          = 0xe000,
    .bcdDevice          = 0x0001,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 0,
    .bNumConfigurations = 1
};

static struct usb_config_bundle
{
    struct usb_config_descriptor config_descriptor;
    struct usb_interface_descriptor interface_descriptor;
    struct usb_endpoint_descriptor endpoint1_descriptor;
    struct usb_endpoint_descriptor endpoint2_descriptor;
    struct usb_endpoint_descriptor endpoint3_descriptor;
    struct usb_endpoint_descriptor endpoint4_descriptor;
} __attribute__((packed)) CACHEALIGN_ATTR config_bundle = 
{
    .config_descriptor =
    {
        .bLength             = sizeof(struct usb_config_descriptor),
        .bDescriptorType     = USB_DT_CONFIG,
        .wTotalLength        = sizeof(struct usb_config_descriptor)
                             + sizeof(struct usb_interface_descriptor)
                             + sizeof(struct usb_endpoint_descriptor) * 4,
        .bNumInterfaces      = 1,
        .bConfigurationValue = 1,
        .iConfiguration      = 0,
        .bmAttributes        = USB_CONFIG_ATT_ONE,
        .bMaxPower           = 250
    },
    .interface_descriptor =
    {
        .bLength             = sizeof(struct usb_interface_descriptor),
        .bDescriptorType     = USB_DT_INTERFACE,
        .bInterfaceNumber    = 0,
        .bAlternateSetting   = 0,
        .bNumEndpoints       = 4,
        .bInterfaceClass     = USB_CLASS_VENDOR_SPEC,
        .bInterfaceSubClass  = 0xff,
        .bInterfaceProtocol  = 0xff,
        .iInterface          = 0
    },
    .endpoint1_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 1
    },
    .endpoint2_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 1
    },
    .endpoint3_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 1
    },
    .endpoint4_descriptor =
    {
        .bLength             = sizeof(struct usb_endpoint_descriptor),
        .bDescriptorType     = USB_DT_ENDPOINT,
        .bEndpointAddress    = 0,
        .bmAttributes        = USB_ENDPOINT_XFER_BULK,
        .wMaxPacketSize      = 0,
        .bInterval           = 1
    }
};

static struct usb_string_descriptor CACHEALIGN_ATTR string_devicename =
{
    32,
    USB_DT_STRING,
    {'e', 'm', 'B', 'I', 'O', 'S', ' ', 'D', 'e', 'b', 'u', 'g', 'g', 'e', 'r'}
};

static const struct usb_string_descriptor CACHEALIGN_ATTR lang_descriptor =
{
    4,
    USB_DT_STRING,
    {0x0409}
};


void usb_setup_dbg_listener()
{
    usb_drv_recv(dbgendpoints[0], dbgrecvbuf, usb_drv_port_speed() ? 512 : 64);
}

void usb_handle_control_request(struct usb_ctrlrequest* req)
{
    const void* addr;
    int size = -1;
    switch (req->bRequest)
    {
    case USB_REQ_GET_STATUS:
        if (req->bRequestType == USB_DIR_IN) ctrlresp[0] = 1;
        else ctrlresp[0] = 0;
        ctrlresp[1] = 0;
        addr = ctrlresp;
        size = 2;
        break;
    case USB_REQ_CLEAR_FEATURE:
        if (req->bRequestType == USB_RECIP_ENDPOINT && req->wValue == USB_ENDPOINT_HALT)
            usb_drv_stall(req->wIndex & 0xf, false, req->wIndex >> 7);
        size = 0;
        break;
    case USB_REQ_SET_FEATURE:
        size = 0;
        break;
    case USB_REQ_SET_ADDRESS:
        size = 0;
        usb_drv_cancel_all_transfers();
        usb_drv_set_address(req->wValue);
        usb_setup_dbg_listener();
        break;
    case USB_REQ_GET_DESCRIPTOR:
        switch (req->wValue >> 8)
        {
        case USB_DT_DEVICE:
            addr = &device_descriptor;
            size = sizeof(device_descriptor);
            break;
        case USB_DT_OTHER_SPEED_CONFIG:
        case USB_DT_CONFIG:
            if ((req->wValue >> 8) == USB_DT_CONFIG)
            {
                int maxpacket = usb_drv_port_speed() ? 512 : 64;
                config_bundle.endpoint1_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint2_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint3_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint4_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.config_descriptor.bDescriptorType = USB_DT_CONFIG;
            }
            else
            {
                int maxpacket = usb_drv_port_speed() ? 64 : 512;
                config_bundle.endpoint1_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint2_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint3_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.endpoint4_descriptor.wMaxPacketSize = maxpacket;
                config_bundle.config_descriptor.bDescriptorType = USB_DT_OTHER_SPEED_CONFIG;
            }
            addr = &config_bundle;
            size = sizeof(config_bundle);
            break;
        case USB_DT_STRING:
            switch (req->wValue & 0xff)
            {
            case 0:
                addr = &lang_descriptor;
                size = sizeof(lang_descriptor);
                break;
            case 1:
                string_devicename.bLength = 14;
                addr = &string_devicename;
                size = 14;
            case 2:
                string_devicename.bLength = sizeof(string_devicename);
                addr = &string_devicename;
                size = sizeof(string_devicename);
                break;
            }
            break;
        }
        break;
    case USB_REQ_GET_CONFIGURATION:
        ctrlresp[0] = 1;
        addr = ctrlresp;
        size = 1;
        break;
    case USB_REQ_SET_CONFIGURATION:
        usb_drv_cancel_all_transfers();
        usb_setup_dbg_listener();
        size = 0;
        break;
    }
    if (!size) usb_drv_send_nonblocking(0, NULL, 0);
    else if (size == -1)
    {
        usb_drv_stall(0, true, true);
        usb_drv_stall(0, true, false);
    }
    else
    {
        usb_drv_recv(0, NULL, 0);
        usb_drv_send_nonblocking(0, addr, size > req->wLength ? req->wLength : size);
    }
}

bool set_dbgaction(enum dbgaction_t action)
{
    if (dbgaction != DBGACTION_IDLE)
    {
        dbgsendbuf[0] = 3;
        usb_drv_send_nonblocking(dbgendpoints[1], dbgsendbuf, 16);
        return true;
    }
    dbgaction = action;
    wakeup_signal(&dbgwakeup);
    return false;
}

void reset() __attribute__((noreturn));

void usb_handle_transfer_complete(int endpoint, int dir, int status, int length)
{
    void* addr = dbgsendbuf;
    int size = 0;
    if (endpoint == dbgendpoints[0])
    {
        switch (dbgrecvbuf[0])
        {
        case 1:  // GET VERSION
            dbgsendbuf[0] = 1;
            dbgsendbuf[1] = 0x01010000;
            dbgsendbuf[2] = PLATFORM_ID;
            dbgsendbuf[3] = usb_drv_port_speed() ? 0x02000200 : 0x00400040;
            size = 16;
            break;
        case 2:  // RESET
            reset();
            break;
        case 3:  // POWER OFF
            set_dbgaction(DBGACTION_POWEROFF);
            break;
        case 4:  // READ MEMORY
            dbgsendbuf[0] = 1;
            memcpy(&dbgsendbuf[4], (const void*)dbgrecvbuf[1], dbgrecvbuf[2]);
            size = dbgrecvbuf[2] + 16;
            break;
        case 5:  // WRITE MEMORY
            dbgsendbuf[0] = 1;
            memcpy((void*)dbgrecvbuf[1], &dbgrecvbuf[4], dbgrecvbuf[2]);
            size = 16;
            break;
        case 6:  // READ DMA
            dbgsendbuf[0] = 1;
            usb_drv_send_nonblocking(dbgendpoints[1], dbgsendbuf, 16);
            usb_drv_send_nonblocking(dbgendpoints[3], (const void*)dbgrecvbuf[1], dbgrecvbuf[2]);
            break;
        case 7:  // WRITE DMA
            dbgsendbuf[0] = 1;
            size = 16;
            usb_drv_recv(dbgendpoints[2], (void*)dbgrecvbuf[1], dbgrecvbuf[2]);
            break;
        case 8:  // READ I2C
            if (set_dbgaction(DBGACTION_I2CRECV)) break;
            dbgi2cbus = dbgrecvbuf[1] & 0xff;
            dbgi2cslave = (dbgrecvbuf[1] >> 8) & 0xff;
            dbgi2caddr = (dbgrecvbuf[1] >> 16) & 0xff;
            dbgi2clen = dbgrecvbuf[1] >> 24;
            break;
        case 9:  // WRITE I2C
            if (set_dbgaction(DBGACTION_I2CSEND)) break;
            dbgi2cbus = dbgrecvbuf[1] & 0xff;
            dbgi2cslave = (dbgrecvbuf[1] >> 8) & 0xff;
            dbgi2caddr = (dbgrecvbuf[1] >> 16) & 0xff;
            dbgi2clen = dbgrecvbuf[1] >> 24;
            memcpy(dbgasyncsendbuf, &dbgsendbuf[4], dbgi2clen);
            break;
        default:
            dbgsendbuf[0] = 2;
            size = 16;
        }
        usb_setup_dbg_listener();
        if (size) usb_drv_send_nonblocking(dbgendpoints[1], addr, size);
    }
}

void usb_handle_bus_reset(void)
{
    dbgendpoints[0] = usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_OUT);
    dbgendpoints[1] = usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_IN);
    dbgendpoints[2] = usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_OUT);
    dbgendpoints[3] = usb_drv_request_endpoint(USB_ENDPOINT_XFER_BULK, USB_DIR_IN);
    config_bundle.endpoint1_descriptor.bEndpointAddress = dbgendpoints[0];
    config_bundle.endpoint2_descriptor.bEndpointAddress = dbgendpoints[1];
    config_bundle.endpoint3_descriptor.bEndpointAddress = dbgendpoints[2];
    config_bundle.endpoint4_descriptor.bEndpointAddress = dbgendpoints[3];
    usb_setup_dbg_listener();
}

void dbgthread(void)
{
    int i;
    while (1)
    {
        wakeup_wait(&dbgwakeup, TIMEOUT_BLOCK);
        for (i = 0; i < MAX_THREADS; i++)
            if (scheduler_threads[i].state == THREAD_DEFUNCT)
            {
                if (scheduler_threads[i].block_type == THREAD_DEFUNCT_STKOV)
                    cprintf(1, "\n*PANIC*\nStack overflow! (%s)\n",
                            scheduler_threads[i].name);
                scheduler_threads[i].state = THREAD_DEFUNCT_ACK;
            }
        if (dbgaction != DBGACTION_IDLE)
        {
            switch (dbgaction)
            {
            case DBGACTION_I2CSEND:
                i2c_send(dbgi2cbus, dbgi2cslave, dbgi2caddr, (uint8_t*)dbgasyncsendbuf, dbgi2clen);
                dbgasyncsendbuf[0] = 1;
                usb_drv_send_nonblocking(dbgendpoints[1], dbgasyncsendbuf, 16);
                break;
            case DBGACTION_I2CRECV:
                i2c_recv(dbgi2cbus, dbgi2cslave, dbgi2caddr,
                         (uint8_t*)(&dbgasyncsendbuf[4]), dbgi2clen);
                dbgasyncsendbuf[0] = 1;
                usb_drv_send_nonblocking(dbgendpoints[1], dbgasyncsendbuf, 16 + dbgi2clen);
                break;
            case DBGACTION_POWEROFF:
                break;
            }
            dbgaction = DBGACTION_IDLE;
        }
    }
}

void usb_init(void)
{
    dbgaction = DBGACTION_IDLE;
    wakeup_init(&dbgwakeup);
    thread_create("Debugger", dbgthread, dbgstack, sizeof(dbgstack), 255, SYSTEM_THREAD, true);
    usb_drv_init();
}
