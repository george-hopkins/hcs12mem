/*
    hc12mem - HC12 memory reader & writer
    generic_f1b_ram1k_bdm.S: geeneric RAM agent for MC9S12 MCU, with single
    FLASH block and at least 1k of RAM, run via BDM
    $Id$

    Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>

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

#include "../io_mc9s12.h"
#include "../agent.h"

.extern _io
.extern _eeprom
.extern _stack
.global _start

BUFFER_SIZE = 256

.section .text

cmd:
	.space 1
status:
	.space 1
param:
	.space 8
buffer:
	.space BUFFER_SIZE


_start:
	lds #_stack
	ldaa cmd
	cmpa #HC12_AGENT_CMD_INIT
	beq init
	cmpa #HC12_AGENT_CMD_EEPROM_MASS_ERASE
	beq eeprom_mass_erase
	cmpa #HC12_AGENT_CMD_EEPROM_ERASE_VERIFY
	beq eeprom_erase_verify
	cmpa #HC12_AGENT_CMD_EEPROM_READ
	beq eeprom_read
	cmpa #HC12_AGENT_CMD_EEPROM_WRITE
	beq eeprom_write
	cmpa #HC12_AGENT_CMD_FLASH_MASS_ERASE
	beq flash_mass_erase
	cmpa #HC12_AGENT_CMD_FLASH_ERASE_VERIFY
	beq flash_erase_verify
	cmpa #HC12_AGENT_CMD_FLASH_READ
	beq flash_read
	cmpa #HC12_AGENT_CMD_FLASH_WRITE
	beq flash_write
	movb #HC12_AGENT_ERROR_CMD,status
	bgnd


done:
	movb #HC12_AGENT_ERROR_NONE,status
	bgnd


init:
	; ignore osc frequency in (param)
	; because ECLKDIV and FCLKDIV are set by hc12mem

	movw #buffer,param+0
	movw #BUFFER_SIZE,param+2
	bra done


eeprom_cmd:
	movb #ESTAT_CBEIF,_io+ESTAT
	brclr _io+ESTAT,#ESTAT_CCIF,.
	rts


eeprom_mass_erase:
	movb #ESTAT_PVIOL|ESTAT_ACCERR,_io+ESTAT
	movb #0xff,_io+EPROT
	movw #0xffff,_eeprom
	movb #0x41,_io+ECMD
	bsr eeprom_cmd
	bra done


eeprom_erase_verify:
	movb #ESTAT_PVIOL|ESTAT_ACCERR,_io+ESTAT
	movw #0xffff,_eeprom
	movb #0x05,_io+ECMD
	bsr eeprom_cmd
	brclr _io+ESTAT,#ESTAT_BLANK,eeprom_erase_verify_error
	bra done
eeprom_erase_verify_error:
	movb #HC12_AGENT_ERROR_VERIFY,status
	bgnd


eeprom_read:
	ldx #buffer
	ldy param+0 ; address
	ldd param+2 ; length
	lsrd ; d = length in words
eeprom_read_loop:
	movw 2,y+,2,x+
	dbne d,eeprom_read_loop
	bra done


eeprom_write:
	ldx #buffer
	ldy param+0 ; address
	ldd param+2 ; length
	lsrd ; d = length in words
	movb #ESTAT_PVIOL|ESTAT_ACCERR,_io+ESTAT
	movb #0xff,_io+EPROT
eeprom_write_loop:
	movw 2,x+,2,y+
	movb #0x20,_io+ECMD
	bsr eeprom_cmd
	dbne d,eeprom_write_loop
	bra done


flash_cmd:
	movb #FSTAT_CBEIF,_io+FSTAT
	nop
	nop
	nop
	nop
	brclr _io+FSTAT,#FSTAT_CCIF,.
	rts


flash_mass_erase:
	ldaa param+0 ; bank selection
	staa _io+FCNFG
	ldaa param+1 ; page
	staa _io+PPAGE
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movb #0xff,_io+FPROT
	movw #0xffff,0xfffe
	movb #0x41,_io+FCMD
	bsr flash_cmd
	bra done


flash_erase_verify:
	ldaa param+0 ; bank selection
	staa _io+FCNFG
	ldaa param+1 ; page
	staa _io+PPAGE
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movw #0xffff,0xfffe
	movb #0x05,_io+FCMD
	bsr flash_cmd
	brclr _io+FSTAT,#FSTAT_BLANK,flash_erase_verify_error
	bra done
flash_erase_verify_error:
	movb #HC12_AGENT_ERROR_VERIFY,status
	bgnd


flash_read:
	ldaa param+0 ; bank selection
	staa _io+FCNFG
	ldaa param+1 ; page
	staa _io+PPAGE
	ldx #buffer
	ldy param+2  ; address
	ldd param+4  ; length
	lsrd ; d = length in words
flash_read_loop:
	movw 2,y+,2,x+
	dbne d,flash_read_loop
	bra done


flash_write:
	ldaa param+0 ; bank selection
	staa _io+FCNFG
	ldaa param+1 ; page
	staa _io+PPAGE
	ldx #buffer
	ldy param+2  ; address
	ldd param+4  ; length
	lsrd ; d = length in words
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movb #0xff,_io+FPROT
flash_write_loop:
	movw 2,x+,2,y+
	movb #0x20,_io+FCMD
	bsr flash_cmd
	dbne d,flash_write_loop
	bra done


.end
