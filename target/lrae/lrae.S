/*
    hc12mem - HC12 memory reader & writer
    generic_f1b_ram1k_lrae.S: generic RAM agent for MC9S12
    with single FLASH block and at least 1k of RAM, run via LRAE
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

#define EEPROM_SIZE 0x0400

.extern _io
.extern _eeprom
.extern _stack
.global _start

.section .text


_start:
	lds #_stack
	bsr sci_rx_word
	cmpd #2000
	bhs setup_xtal_ok
	ldaa #HC12_AGENT_ERROR_XTAL
	bsr sci_tx
	bra _start
setup_xtal_ok:
	clr _io+ECLKDIV
	cmpd #12800
	bls setup_prdiv_set
	lsrd
	lsrd
	lsrd
	movb #ECLKDIV_PRDIV8,_io+ECLKDIV
setup_prdiv_set:
	ldx #200
	idiv
	tfr x,a
	oraa _io+ECLKDIV
	staa _io+ECLKDIV
	staa _io+FCLKDIV

	ldaa #HC12_AGENT_ERROR_NONE
	bsr sci_tx


loop:
	ldx #cmd
	bsr sci_rx
	staa 1,x+
	bsr sci_rx
	staa 1,x+
	suba #3
	beq cmd_loop_done
	tab
cmd_loop:
	bsr sci_rx
	staa 1,x+
	dbne b,cmd_loop
cmd_loop_done:
	ldx #cmd
	ldab 1,x
	decb
	clra
cmd_sum:
	adda 1,x+
	dbne b,cmd_sum
	tab
	bsr sci_rx
	cba
	beq cmd_ok
error_sum:
	ldaa #HC12_AGENT_ERROR_SUM
	bsr sci_tx
	bra loop
cmd_ok:
	ldaa #HC12_AGENT_ERROR_NONE
	bsr sci_tx

	ldaa cmd
;	cmpa #HC12_AGENT_CMD_EEPROM_MASS_ERASE
;	beq eeprom_mass_erase
;	cmpa #HC12_AGENT_CMD_EEPROM_ERASE_VERIFY
;	beq eeprom_erase_verify
;	cmpa #HC12_AGENT_CMD_EEPROM_READ
;	beq eeprom_read
;	cmpa #HC12_AGENT_CMD_EEPROM_WRITE
;	beq eeprom_write
	cmpa #HC12_AGENT_CMD_FLASH_MASS_ERASE
	beq flash_mass_erase
	cmpa #HC12_AGENT_CMD_FLASH_ERASE_VERIFY
	beq flash_erase_verify
	cmpa #HC12_AGENT_CMD_FLASH_ERASE_SECTOR
	beq flash_erase_sector
	cmpa #HC12_AGENT_CMD_FLASH_READ
	beq flash_read
	cmpa #HC12_AGENT_CMD_FLASH_WRITE
	beq flash_write
	ldaa #HC12_AGENT_ERROR_CMD
	bsr sci_tx
	bra loop
done:
	ldaa #HC12_AGENT_ERROR_NONE
	bsr sci_tx
	bra loop


;eeprom_cmd:
;	movb #ESTAT_CBEIF,_io+ESTAT
;	brclr _io+ESTAT,#ESTAT_CCIF,.
;	rts


;eeprom_mass_erase:
;	movb #ESTAT_PVIOL|ESTAT_ACCERR,_io+ESTAT
;	movb #0xff,_io+EPROT
;	movw #_eeprom,_io+EADDR
;	movw #0xffff,_io+EDATA
;	movb #0x41,_io+ECMD
;	bsr eeprom_cmd
;	bra done


;eeprom_erase_verify:
;	movb #ESTAT_PVIOL|ESTAT_ACCERR,_io+ESTAT
;	movw #_eeprom,_io+EADDR
;	movw #0xffff,_io+EDATA
;	movb #0x05,_io+ECMD
;	bsr eeprom_cmd
;	brclr _io+ESTAT,#ESTAT_BLANK,eeprom_erase_verify_error
;	bra done
;eeprom_erase_verify_error:
;	ldaa #HC12_AGENT_ERROR_VERIFY
;	bsr sci_tx
;	bra loop


;eeprom_read:
;	ldd #_eeprom
;	bsr sci_tx_word
;	ldd #EEPROM_SIZE
;	bsr sci_tx_word
;	ldx #_eeprom
;	ldy #EEPROM_SIZE
;eeprom_read_loop:
;	ldaa 0,x
;	bsr sci_tx
;	inx
;	dbne y,eeprom_read_loop
;	bra loop


;eeprom_write:
;	bsr sci_rx_word ; address
;	tfr d,y
;	bsr sci_rx_word ; length
;	bclr _io+ESTAT,#ESTAT_PVIOL|ESTAT_ACCERR
;	movb #0xff,_io+EPROT
;eeprom_write_loop:
;	movw 2,x+,2,y+
;	movb #0x20,_io+ECMD
;	bsr eeprom_cmd
;	dbne d,eeprom_write_loop
;	bra done


flash_cmd:
	movb #FSTAT_CBEIF,_io+FSTAT
	nop
	nop
	nop
	nop
;flash_cmd_wait:
;	ldaa _io+FSTAT
;	bpl flash_cmd_wait
;	asla ; move CCIF to sign bit
;	bpl flash_cmd_wait
	brclr _io+FSTAT,#FSTAT_CCIF,.
	rts


flash_erase_sector:
	ldaa cmd+2 ; bank selection
	staa _io+FCNFG
	ldaa cmd+3 ; page
	staa _io+PPAGE
	ldx cmd+4  ; sector address
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movw #0xffff,0,x
	movb #0x40,_io+FCMD
	bsr flash_cmd
	bra done


flash_mass_erase:
	ldaa cmd+2 ; bank selection
	staa _io+FCNFG
	ldaa cmd+3 ; page
	staa _io+PPAGE
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movw #0xffff,0xfffe
	movb #0x41,_io+FCMD
	bsr flash_cmd
	bra done


flash_erase_verify:
	ldaa cmd+2 ; bank selection
	staa _io+FCNFG
	ldaa cmd+3 ; page
	staa _io+PPAGE
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movw #0xffff,0xfffe
	movb #0x05,_io+FCMD
	bsr flash_cmd
	brclr _io+FSTAT,#FSTAT_BLANK,flash_erase_verify_error
	bra done
flash_erase_verify_error:
	ldaa #HC12_AGENT_ERROR_VERIFY
	bsr sci_tx
	bra loop


flash_read:
	ldaa cmd+2 ; bank selection
	staa _io+FCNFG
	ldaa cmd+3 ; page
	staa _io+PPAGE
	ldx cmd+4 ; address
	ldy cmd+6 ; length
	clrb
flash_read_loop:
	ldaa 0,x
	aba
	tab
	ldaa 0,x
	inx
	bsr sci_tx
	dbne y,flash_read_loop
	tba
	bsr sci_tx ; sum
	bra loop


flash_write:
	ldaa cmd+2 ; bank selection
	staa _io+FCNFG
	ldaa cmd+3 ; page
	staa _io+PPAGE
	ldx cmd+4  ; address
	ldy cmd+6  ; length
	pshx
	pshy
	ldx #buffer
	clrb
flash_write_read_loop:
	bsr sci_rx
	staa 1,x+
	aba
	tab
	dbne y,flash_write_read_loop
	bsr sci_rx
	cba
	beq flash_write_ok
	puly
	pulx
	bra error_sum
flash_write_ok:
	puld
	lsrd ; d = length in words
	puly ; y = address
	movb #FSTAT_PVIOL|FSTAT_ACCERR,_io+FSTAT
	movb #0xff,_io+FPROT
	ldx #buffer
flash_write_loop:
	movw 2,x+,2,y+
	movb #0x20,_io+FCMD
	bsr flash_cmd
	dbne d,flash_write_loop
	bra done


sci_rx:
	brclr _io+SCI0SR1,SCI0SR1_RDRF,sci_rx
	ldaa _io+SCI0DRL
	rts


sci_rx_word:
	bsr sci_rx
	tab
	bsr sci_rx
	exg a,b
	rts


sci_tx:
	brclr _io+SCI0SR1,SCI0SR1_TDRE,sci_tx
	staa _io+SCI0DRL
	rts


sci_tx_word:
	bsr sci_tx
	tba
	bsr sci_tx
	rts


cmd:
	.ds 2
param:
	.ds 8


buffer:
	.ds 256


.end
