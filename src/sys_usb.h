/*
    hc12mem - HC12 memory reader & writer
    Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>

    sys_usb.h: USB devices access via libusb

    $Id$

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __SYS_USB_H
#define __SYS_USB_H

#include "sys.h"

#if SYS_TYPE_UNIX
# include "libusb/libusbunix.h"
# define LIBUSB_NAME "libusb.so"
#endif
#if SYS_TYPE_WIN32
# include "libusb/libusbwin32.h"
# define LIBUSB_NAME "libusb0.dll"
# define SYS_USB_WIN32_ETIMEDOUT 116 /* magic value within libusb */
#endif

typedef void (*libusb_init_t)(void);
typedef char *(*libusb_strerror_t)(void);
#if SYS_TYPE_WIN32
typedef struct usb_version *(*libusb_get_version_t)(void);
#endif
typedef void (*libusb_set_debug_t)(int level);
typedef int (*libusb_find_busses_t)(void);
typedef int (*libusb_find_devices_t)(void);
typedef struct usb_bus *(*libusb_get_busses_t)(void);
typedef struct usb_device *(*libusb_device_t)(usb_dev_handle *dev);
typedef usb_dev_handle *(*libusb_open_t)(struct usb_device *dev);
typedef int (*libusb_close_t)(usb_dev_handle *dev);
typedef int (*libusb_set_configuration_t)(usb_dev_handle *dev, int configuration);
typedef int (*libusb_claim_interface_t)(usb_dev_handle *dev, int interface);
typedef int (*libusb_release_interface_t)(usb_dev_handle *dev, int interface);
typedef int (*libusb_control_msg_t)(usb_dev_handle *dev,
	int requesttype, int request, int value, int index, char *bytes, int size, int timeout);
typedef int (*libusb_bulk_read_t)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
typedef int (*libusb_bulk_write_t)(usb_dev_handle *dev, int ep, char *bytes, int size, int timeout);
typedef int (*libusb_get_string_simple_t)(usb_dev_handle *dev, int index, char *buf, size_t buflen);

typedef usb_dev_handle *sys_usb_dev_t;

extern int sys_usb_open(void);
extern int sys_usb_close(void);
extern int sys_usb_device_open(sys_usb_dev_t *d, uint16_t vid, uint16_t pid);
extern int sys_usb_device_close(sys_usb_dev_t *d);
extern int sys_usb_control_msg(sys_usb_dev_t *d,
	uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
	void *buf, size_t *len, unsigned int timeout);
extern int sys_usb_bulk_read(sys_usb_dev_t *d, uint8_t ep,
	void *buf, size_t *len, unsigned int timeout);
extern int sys_usb_bulk_write(sys_usb_dev_t *d, uint8_t ep,
	const void *buf, size_t *len, unsigned int timeout);

#endif /* __SYS_USB_H */
