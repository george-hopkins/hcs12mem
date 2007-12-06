/*
    hcs12mem - HC12/S12 memory reader & writer
    Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>

    sys_usb.c: USB devices access via libusb

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

#include "hcs12mem.h"
#include "sys_usb.h"

static int libusb_ref = 0;
static sys_dl_t libusb_dl;

static libusb_init_t libusb_init_f;
static libusb_strerror_t libusb_strerror_f;
#if SYS_TYPE_WIN32
static libusb_get_version_t libusb_get_version_f;
#endif
static libusb_find_busses_t libusb_find_busses_f;
static libusb_find_devices_t libusb_find_devices_f;
static libusb_get_busses_t libusb_get_busses_f;
static libusb_device_t libusb_device_f;
static libusb_open_t libusb_open_f;
static libusb_close_t libusb_close_f;
static libusb_set_configuration_t libusb_set_configuration_f;
static libusb_claim_interface_t libusb_claim_interface_f;
static libusb_release_interface_t libusb_release_interface_f;
static libusb_control_msg_t libusb_control_msg_f;
static libusb_bulk_read_t libusb_bulk_read_f;
static libusb_bulk_write_t libusb_bulk_write_f;
static libusb_get_string_simple_t libusb_get_string_simple_f;


/*
 *  open usb library
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

int sys_usb_open(void)
{
	int ret;

	if (libusb_ref++ != 0)
		return 0;

	ret = sys_dl_open(&libusb_dl, LIBUSB_NAME);
	if (ret != 0)
	{
		error("unable to open usb library (%s): %s\n",
		      (const char *)LIBUSB_NAME,
		      (const char *)strerror(ret));
		return ret;
	}

	ret = sys_dl_func(&libusb_dl, "usb_init",
		(sys_dl_func_t *)(void *)&libusb_init_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_strerror",
		(sys_dl_func_t *)(void *)&libusb_strerror_f);
	if (ret != 0)
		goto nofunc;

#if SYS_TYPE_WIN32
	ret = sys_dl_func(&libusb_dl, "usb_get_version",
		(sys_dl_func_t *)(void *)&libusb_get_version_f);
	if (ret != 0)
		goto nofunc;
#endif

	ret = sys_dl_func(&libusb_dl, "usb_find_busses",
		(sys_dl_func_t *)(void *)&libusb_find_busses_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_find_devices",
		(sys_dl_func_t *)(void *)&libusb_find_devices_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_get_busses",
		(sys_dl_func_t *)(void *)&libusb_get_busses_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_device",
		(sys_dl_func_t *)(void *)&libusb_device_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_open",
		(sys_dl_func_t *)(void *)&libusb_open_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_close",
		(sys_dl_func_t *)(void *)&libusb_close_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_set_configuration",
		(sys_dl_func_t *)(void *)&libusb_set_configuration_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_claim_interface",
		(sys_dl_func_t *)(void *)&libusb_claim_interface_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_release_interface",
		(sys_dl_func_t *)(void *)&libusb_release_interface_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_control_msg",
		(sys_dl_func_t *)(void *)&libusb_control_msg_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_bulk_read",
		(sys_dl_func_t *)(void *)&libusb_bulk_read_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_bulk_write",
		(sys_dl_func_t *)(void *)&libusb_bulk_write_f);
	if (ret != 0)
		goto nofunc;

	ret = sys_dl_func(&libusb_dl, "usb_get_string_simple",
		(sys_dl_func_t *)(void *)&libusb_get_string_simple_f);
	if (ret != 0)
		goto nofunc;

	(*libusb_init_f)();

#if SYS_TYPE_WIN32
	{

		struct usb_version *v;

		v = (*libusb_get_version_f)();
		if (v == NULL)
		{
#			if !SYS_TYPE_WIN32 /* bug in libusb-win32 */
			sys_dl_close(&libusb_dl);
#			endif
			libusb_ref = 0;
			error("unable to get libusb version\n");
			return ENOENT;
		}

		if (options.debug)
		{
			printf("libusb DLL version <%u.%u.%u.%u>\n",
			       (unsigned int)v->dll.major,
			       (unsigned int)v->dll.minor,
			       (unsigned int)v->dll.micro,
			       (unsigned int)v->dll.nano);
		}

		if (v->driver.major == -1)
		{
#			if !SYS_TYPE_WIN32 /* bug in libusb-win32 */
			sys_dl_close(&libusb_dl);
#			endif
			libusb_ref = 0;
			error("device not connected or driver not installed\n");
			return ENOENT;
		}

		if (options.debug)
		{
			printf("libusb driver version <%u.%u.%u.%u>\n",
			       (unsigned int)v->driver.major,
			       (unsigned int)v->driver.minor,
			       (unsigned int)v->driver.micro,
			       (unsigned int)v->driver.nano);
		}
	}
#endif

	ret = (*libusb_find_busses_f)();
	if (options.debug)
		printf("USB busses: %d\n", ret);

	ret = (*libusb_find_devices_f)();
	if (options.debug)
		printf("USB devices: %d\n", ret);

	return 0;

nofunc:
#	if !SYS_TYPE_WIN32 /* bug in libusb-win32 */
	sys_dl_close(&libusb_dl);
#	endif
	libusb_ref = 0;
	error("invalid usb library (%s): missing symbols\n",
	      (const char *)LIBUSB_NAME);
	return ret;
}


/*
 *  close usb library
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

int sys_usb_close(void)
{
	int ret;

	if (libusb_ref == 0)
		return 0;
	if (--libusb_ref != 0)
		return 0;

	ret = sys_dl_close(&libusb_dl);
	if (ret != 0)
	{
		error("unable to close usb library (%s): %s\n",
		      (const char *)LIBUSB_NAME,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  helper function - formatting usb version number
 *
 *  in:
 *    buf - buffer for string
 *    len - buffer size
 *    v - version to format
 *  out:
 *    void
 */

static void sys_usb_format_bcd_version(char *buf, size_t len, uint16_t v)
{
	if ((v & 0x0f) == 0)
	{
		snprintf(buf, len, "%u.%u",
			 (unsigned int)(v >> 8),
			 (unsigned int)((v >> 4) & 0x000f));
	}
	else
	{
		snprintf(buf, len, "%u.%u.%u",
			 (unsigned int)(v >> 8),
			 (unsigned int)((v >> 4) & 0x000f),
			 (unsigned int)(v & 0x000f));
	}
}


/*
 *  open device connection via usb
 *
 *  in:
 *    d - device handle (on return)
 *    vid, pid - USB VID&PID for device
 *  out:
 *    status code (errno-like)
 */

int sys_usb_device_open(sys_usb_dev_t *d, uint16_t vid, uint16_t pid)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *h;
	char bufm[256];
	char bufp[256];
	char bufsn[256];
	int i;
	int ret;

	dev = NULL;
	for (bus = (*libusb_get_busses_f)(); bus != NULL; bus = bus->next)
	{
		for (dev = bus->devices; dev != NULL; dev = dev->next)
		{
			if (dev->descriptor.idVendor == vid &&
			    dev->descriptor.idProduct == pid)
				break;
		}
		if (dev != NULL)
			break;
	}
	if (dev == NULL)
	{
		error("USB device not found\n");
		return ENOENT;
	}

	if (options.debug)
	{
		printf("USB device <%s> bus <%s>\n",
		       (const char *)dev->filename,
		       (const char *)bus->dirname);
	}

	if (options.verbose)
	{
		char buf_usbv[80];
		char buf_devr[80];

		sys_usb_format_bcd_version(buf_usbv, sizeof(buf_usbv),
			(uint16_t)dev->descriptor.bcdUSB);
		sys_usb_format_bcd_version(buf_devr, sizeof(buf_devr),
			(uint16_t)dev->descriptor.bcdDevice);
		printf("USB device USB version <%s> VID <0x%04x> PID <0x%04x> device release <%s>\n",
		       (const char *)buf_usbv,
		       (unsigned int)vid,
		       (unsigned int)pid,
		       (const char *)buf_devr);
	}

	h = (*libusb_open_f)(dev);
	if (h == NULL)
	{
		error("unable to open USB device\n");
		return EIO;
	}

	ret = (*libusb_get_string_simple_f)(h,
		dev->descriptor.iManufacturer, bufm, sizeof(bufm));
	if (ret < 0)
	{
		ret = -ret;
		error("unable to read USB device manufacturer descriptor: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}

	ret = (*libusb_get_string_simple_f)(h,
		dev->descriptor.iProduct, bufp, sizeof(bufp));
	if (ret < 0)
	{
		ret = -ret;
		error("unable to read USB device product descriptor: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}

	ret = (*libusb_get_string_simple_f)(h,
		dev->descriptor.iSerialNumber, bufsn, sizeof(bufsn));
	if (ret < 0)
	{
		ret = -ret;
		error("unable to read USB device serial number descriptor: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}

	if (options.verbose)
	{
		if (strcmp(bufp, bufsn) == 0)
		{
			printf("USB device manufacturer <%s> product <%s>\n",
			       (const char *)bufm,
			       (const char *)bufp);
		}
		else
		{
			printf("USB device manufacturer <%s> product <%s> serial number <%s>\n",
			       (const char *)bufm,
			       (const char *)bufp,
			       (const char *)bufsn);
		}
	}

	if (options.verbose)
	{
		for (i = 0; i < (int)dev->descriptor.bNumConfigurations; ++ i)
		{
			printf("USB device config <#%u> power <%s> max current <%umA>\n",
			       (unsigned int)(i + 1),
			       (const char *)(dev->config[i].bmAttributes & 0x40 ? "self" : "bus"),
			       (unsigned int)dev->config[i].MaxPower * 2);
		}
	}

	ret = -(*libusb_set_configuration_f)(h, 1);
	if (ret != 0)
	{
		error("unable to set USB device configuration: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}

	ret = -(*libusb_claim_interface_f)(h, 0);
	if (ret != 0)
	{
		error("unable to claim USB device interface: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}

	*d = (sys_usb_dev_t)h;
	return 0;
}


/*
 *  close usb device connection
 *
 *  in:
 *    d - device handle
 *  out:
 *    status code (errno-like)
 */

int sys_usb_device_close(sys_usb_dev_t *d)
{
	usb_dev_handle *h;
	int ret;

	if (*d == NULL)
		return 0;

	h = (usb_dev_handle *)(*d);

	ret = -(*libusb_release_interface_f)(h, 0);
	if (ret != 0)
	{
		error("unable to release USB device interface: %s\n",
		      (*libusb_strerror_f)());
	}

	ret = -(*libusb_close_f)(h);
	if (ret != 0)
	{
		error("unable to close USB device: %s\n",
		      (*libusb_strerror_f)());
	}

	*d = NULL;
	return 0;
}


/*
 *  send control message to USB device
 *
 *  in:
 *    d - device handle
 *    request_type, request, value, index - control message header fields
 *    bytes - data buffer (in & out)
 *    len - data count to send (on entry), received data count (on return)
 *    timeout - operation timeout, in ms
 *  out:
 *    status code (errno-like)
 */

int sys_usb_control_msg(sys_usb_dev_t *d,
	uint8_t request_type, uint8_t request, uint16_t value, uint16_t index,
	void *buf, size_t *len, unsigned int timeout)
{
	int ret;

	ret = (*libusb_control_msg_f)((usb_dev_handle *)(*d),
		(int)request_type, (int)request, (int)value, (int)index,
		(char *)buf, (int)(*len), (int)timeout);
	if (ret < 0)
	{
		ret = -ret;
#if SYS_TYPE_WIN32
		if (ret == SYS_USB_WIN32_ETIMEDOUT)
			ret = ETIMEDOUT;
#endif
		error("unable to send control message to USB device: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}
	*len = (size_t)ret;
	return 0;
}


/*
 *  bulk read data transfer from USB device
 *
 *  in:
 *    d - device handle
 *    ep - endpoint number
 *    bytes - data buffer
 *    len - data count to read (on entry), read data counter (on return)
 *    timeout - operation timeout, in ms
 *  out:
 *    status code (errno-like)
 */

int sys_usb_bulk_read(sys_usb_dev_t *d, uint8_t ep,
	void *buf, size_t *len, unsigned int timeout)
{
	int ret;

	ret = (*libusb_bulk_read_f)((usb_dev_handle *)(*d),
		USB_ENDPOINT_IN | (int)ep,
		(char *)buf, (int)(*len), (int)timeout);
	if (ret < 0)
	{
		ret = -ret;
#if SYS_TYPE_WIN32
		if (ret == SYS_USB_WIN32_ETIMEDOUT)
			ret = ETIMEDOUT;
#endif
		error("unable to read data from USB device: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}
	*len = (size_t)ret;
	return 0;
}


/*
 *  bulk write data transfer to USB device
 *
 *  in:
 *    d - device handle
 *    ep - endpoint number
 *    bytes - data buffer
 *    len - data count to write (on entry), written data counter (on return)
 *    timeout - operation timeout, in ms
 *  out:
 *    status code (errno-like)
 */

int sys_usb_bulk_write(sys_usb_dev_t *d, uint8_t ep,
	const void *buf, size_t *len, unsigned int timeout)
{
	int ret;

	ret = (*libusb_bulk_write_f)((usb_dev_handle *)(*d),
		USB_ENDPOINT_OUT | (int)ep,
		(char *)buf, (int)(*len), (int)timeout);
	if (ret < 0)
	{
		ret = -ret;
#if SYS_TYPE_WIN32
		if (ret == SYS_USB_WIN32_ETIMEDOUT)
			ret = ETIMEDOUT;
#endif
		error("unable to write data to USB device: %s\n",
		      (*libusb_strerror_f)());
		return ret;
	}
	*len = (size_t)ret;
	return 0;
}
