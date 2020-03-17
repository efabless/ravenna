# Startup script for command-line invocation of tclftdi
# Ravenna demonstration/development board
#
# This startup script assumes that the board is physically configured
# for communication with the Ravenna chip on-board SPI.  The Ravenna demo
# board has all four flash SPI signals jumpered between the Ravenna
# processor and the FTDI.  The routines in this script assume the
# jumpers are all set to connect the SPI flash directly to the Ravenna
# processor.  Then the FTDI signals are on the four remaining unjumpered
# pins.  These must be wire-jumpered to the housekeeping SPI pins on the
# long headers.  Then the FTDI can communicate with the SPI flash through
# the Ravenna chip using pass-through mode.

# 1) Define simplified read and write operations for the Ravenna SPI

proc ftdi::ravenna_read {device addr nbytes} {
    if {$nbytes > 7} {
        return [ftdi::ravenna_stream_read $device $addr $nbytes]
    } else {
        set cmdword [expr {(0x40 + ($nbytes << 3)) * 256 + $addr}]
        return [ftdi::spi_read $device $cmdword $nbytes]
    }
}

proc ftdi::ravenna_write {device addr data} {
    set nbytes [llength $data]
    if {$nbytes > 7} {
        ftdi::ravenna_stream_write $device $addr $data
    } else {
        set cmdword [expr {(0x80 + ($nbytes << 3)) * 256 + $addr}]
        ftdi::spi_write $device $cmdword $data
    }
}

proc ftdi::ravenna_stream_read {device addr nbytes} {
    set cmdword [expr {0x40 * 256 + $addr}]
    return [ftdi::spi_read $device $cmdword $nbytes]
}

proc ftdi::ravenna_stream_write {device addr data} {
    set cmdword [expr {0x80 * 256 + $addr}]
    ftdi::spi_write $device $cmdword $data
}

# Put Ravenna into pass-through mode and send an SPI write
# command to the SPI flash.  This works like a stream read,
# and so CSB is held low until the command ends.  When CSB
# is raised, pass-through mode exits.  Arguments command,
# addr, and data are all values of the SPI flash chip.

proc ftdi::ravenna_pass_thru_write {device command data} {
    set cmdword [expr {0xc4 * 256 + $command}]
    ftdi::spi_write $device $cmdword $data
}

# Put Ravenna into pass-through mode and send an SPI read
# command to the SPI flash, along with data (which may include
# an address;  word length of the data determined by the data
# values themselves) and return data received from the device.

proc ftdi::ravenna_pass_thru_read {device command data nbytes} {
    set cmdword [expr {0xc4 * 256 + $command}]
    foreach value $data {
	set cmdword [expr {($cmdword << 8) + $value}]
    }
    set datalen [llength $data]
    ftdi::spi_command $device [expr {8 * (2 + $datalen)}]
    set retval [ftdi::spi_read $device $cmdword $nbytes]
    ftdi::spi_command $device 16
    return $retval
}

# 2. Define procedure to check for vendor/product ID on Ravenna

proc ftdi::ravenna_check {device} {
    set ravenna_id [ftdi::ravenna_read $device 1 3]
    set vendor_id [expr {[lindex $ravenna_id 0] * 256 + [lindex $ravenna_id 1]}]
    set product_id [lindex $ravenna_id 2]

    if {$vendor_id != 1110} {
	puts stderr "Error:  Received vendor ID of $vendor_id, expecting 1110"
    }

    if {$product_id != 3} {
	puts stderr "Error:  Received product ID of $product_id, expecting 3"
    }

    if {$vendor_id == 1110 && $product_id == 3} {
        puts stdout "Confirmed DUT is efabless Ravenna"
	return 0
    }
    return 1
}

# 3. Define additional access commands so that one does not have to memorize
#    register locations

# 3a. Reset Ravenna.  mode can be "pulse", "on", or "off".  Default is "pulse".

proc ftdi::reset {device {mode pulse}} {
    if {$mode == "pulse" || $mode == "on"} {
        ftdi::ravenna_write $device 7 1
    }
    if {$mode == "pulse" || $mode == "off"} {
        ftdi::ravenna_write $device 7 0
    }
}

# 3b. Set clock.  mode is "external" ("ext") or "internal" ("pll").  Default
# mode is "internal" (PLL 100MHz clock).  This routine both switches to the
# external clock and powers down the PLL and crystal oscillator, and vice
# versa.

proc ftdi::set_clock {device {mode internal}} {
    if {$mode == "internal" || $mode == "pll"} {
        ftdi::ravenna_write $device 4 0x07
        ftdi::ravenna_write $device 9 0x03
        ftdi::ravenna_write $device 5 0x00
    }
    if {$mode == "external" || $mode == "ext"} {
        ftdi::ravenna_write $device 5 0x01
        ftdi::ravenna_write $device 9 0x02
        ftdi::ravenna_write $device 4 0x00
    }
}

# 3c. Apply IRQ.  mode can be "pulse", "on", or "off".  Default is "pulse".

proc ftdi::interrupt {device {mode pulse}} {
    if {$mode == "pulse" || $mode == "on"} {
        ftdi::ravenna_write $device 6 1
    }
    if {$mode == "pulse" || $mode == "off"} {
        ftdi::ravenna_write $device 6 0
    }
}

# 3d. Get the CPU trap state.  Returns 1 or 0

proc ftdi::get_trap {device} {
    return [ftdi::ravenna_read $device 8 1]
}

# 3e. Power down the Ravenna chip (disable LDOs and oscillator)

proc ftdi::powerdown {device} {
    ftdi::ravenna_write $device 9 0
}

# 3f. Power up the Ravenna chip (enable LDOs and oscillator)

proc ftdi::powerup {device} {
    ftdi::ravenna_write $device 9 0x03
}

# 4. Define commands to access the SPI flash using the "pass through"
#    mode on Ravenna.

# 4a. Check for flash busy

proc ftdi::flash_busy {device} {
    set status1 [ftdi::ravenna_pass_thru_read $device 5 {} 1]
    return [expr {$status1 & 1}]
}

# 4b. Simple test:  Read the SPI flash JEDEC ID values using pass-through
#     mode.

proc ftdi::flash_check {device} {
    set flash_id [ftdi::ravenna_pass_thru_read $device 159 {} 3]
    set vendor_id [lindex $flash_id 0]
    set product_id [lindex $flash_id 1]
    set feature_id [lindex $flash_id 2]

    if {$vendor_id != 239} {
        puts stderr "Error:  Received vendor ID of $vendor_id, expecting 239"
    }
    if {$product_id != 64} {
        puts stderr "Error:  Received product ID of $product_id, expecting 64"
    }
    if {$feature_id != 22} {
        puts stderr "Error:  Received feature ID of $feature_id, expecting 22"
    }
    if {$vendor_id == 239 && $product_id == 64 && $feature_id == 22} {
        puts stdout "Confirmed SPI flash is a Winbond 32MB device"
        return 0
    } else {
	puts stdout "Received flash_id $flash_id"
    }
    return 1
}

# 4c. Another simple test:  Read the SPI flash status registers using pass-
#     through mode

proc ftdi::flash_registers {device} {

    while {[ftdi::flash_busy $device] > 0} {
	after 100
    }

    set status1 [ftdi::ravenna_pass_thru_read $device 5 {} 1]
    set status2 [ftdi::ravenna_pass_thru_read $device 53 {} 1]
    set status3 [ftdi::ravenna_pass_thru_read $device 21 {} 1]

    puts stdout "Register values:"
    puts stdout "Status 1 = $status1"
    puts stdout "Status 2 = $status2"
    puts stdout "Status 3 = $status3"
}

# 4d. Read contents of the SPI flash

proc ftdi::flash_read {device address length} {

    while {[ftdi::flash_busy $device] > 0} {
	after 100
    }

    set addr2 [expr {($address & 0xff0000) >> 16}]
    set addr1 [expr {($address & 0xff00) >> 8}]
    set addr0 [expr {$address & 0xff}]
    set addr [list $addr2 $addr1 $addr0]
    return [ftdi::ravenna_pass_thru_read $device 3 $addr $length]
}

# 4e. Erase the entire flash memory

proc ftdi::flash_erase {device} {
    # Apply write enable
    ftdi::ravenna_pass_thru_write $device 6 {}

    ftdi::ravenna_pass_thru_write $device 96 {}

    puts stdout "Waiting for erase cycle to finish (takes approximately 10 seconds). . ."
    # Check for program cycle finished
    while {[ftdi::flash_busy $device] > 0} {
        after 100
    }
    puts stdout "Erase cycle done."

    # Apply write disable
    ftdi::ravenna_pass_thru_write $device 4 {}
}


# 4f. Write values to flash

proc ftdi::flash_write {device address values} {

    while {[ftdi::flash_busy $device] > 0} {
	after 100
    }

    # Set write enable
    ftdi::ravenna_pass_thru_write $device 6 {}

    # Write values
    set addr2 [expr {($address & 0xff0000) >> 16}]
    set addr1 [expr {($address & 0xff00) >> 8}]
    set addr0 [expr {$address & 0xff}]
    set data [list $addr2 $addr1 $addr0 {*}$values]
    ftdi::ravenna_pass_thru_write $device 2 $data
    
    # Wait for programming cycle to finish
    while {[ftdi::flash_busy $device] > 0} {
	after 100
    }
}

# 4f. Write a hex file to flash

proc ftdi::flash_write_hex {device hexfile {doerase true} {debug false}} {

    # Check that hexfile is readable.
    if [catch {open $hexfile r} hf] {
        puts stderr "Failure to open/read hex file $hexfile"
	return
    }

    # Check for device ready
    while {[ftdi::flash_busy $device] > 0} {
	after 100
    }

    if {$doerase} {
	# Perform an erase cycle. 
	ftdi::flash_erase $device
    }

    puts stdout "Starting programming cycle. . ."

    # Read hex file.  For each address line, convert data to decimal values,
    # and assemble into an array of byte data.

    # Read in the hex file, convert address blocks, and write them to
    # the SPI flash
    set datavec {}
    set address 0
    while {[gets $hf line] >= 0} {
        if {[string first @ $line] == 0} {

	    # Program the previously read data block (if any)
	    if {[llength $datavec] > 0} {
                if {$debug == true} {
		    puts stdout "Write addr=$address values=$datavec"
		}
		ftdi::flash_write $device $address $datavec
		set datavec {}
	    }

	    # Set the address line (24 bit address)
            # Ignore the first byte (not using 32 bit addressing)
	    set address [format %d 0x[string range $line 3 8]]
        } else {
	    # Append data to datavec
	    # Does not work if too many bytes are written at a time. . .
	    # Not sure why.  Buffer overrun?
	    set nvals [llength $datavec]
            foreach value $line {
		if {$nvals == 255} {
		    if {$debug == true} {
			puts stdout "Write addr=$address values=$datavec"
		    }
		    ftdi::flash_write $device $address $datavec
		    incr address 255 
		    set datavec {}
		    set nvals 0
		}
		lappend datavec [format %d 0x$value]
		incr nvals
	    }
        }
    }

    # Final block
    if {[llength $datavec] > 0} {
        if {$debug == true} {
	    puts stdout "Write addr=$address values=$datavec"
	}
	ftdi::flash_write $device $address $datavec
    }
    puts stdout "Programming cycle done."
    close $hf
}

# Open the device

setid 24596 1027	;# FTDI FT232H
# verbose 3
puts stdout "X"
set ravenna [opendev A]
puts stdout "Y"
# MPSSE mode debugged 2/10/2020.  Bitbang mode also works.
spi_bitbang $ravenna {{CSB 3} {SDO 2} {SDI 1} {SCK 0} {USR0 4} {USR1 5} {USR2 6} {USR3 7}}
bitbang_word $ravenna 8
spi_speed $ravenna 16.0
spi_command $ravenna 16
puts stdout "Z"

# Check that the communications channel to the Ravenna SPI is working

if {[ftdi::ravenna_check $ravenna] != 0} {puts stderr "ERROR"}

# Import commands
namespace import ftdi::powerup ftdi::powerdown ftdi::interrupt ftdi::get_trap
namespace import ftdi::set_clock ftdi::reset ftdi::ravenna_check
namespace import ftdi::flash_check ftdi::flash_registers
namespace import ftdi::flash_read ftdi::flash_write ftdi::flash_write_hex

# Print a list of commands
puts stdout "Ravenna housekeeping SPI commands:"
puts stdout ""
puts stdout "  reset $ravenna \[pulse|on|off\]"
puts stdout "  interrupt $ravenna \[pulse|on|off\]"
puts stdout "  set_clock $ravenna \[external|internal\]"
puts stdout "  powerup $ravenna"
puts stdout "  powerdown $ravenna"
puts stdout "  get_trap $ravenna"
puts stdout "  ravenna_check $ravenna"
puts stdout ""
puts stdout "SPI flash commands:"
puts stdout ""
puts stdout "  flash_check $ravenna"
puts stdout "  flash_registers $ravenna"
puts stdout "  flash_read $ravenna <address> <length>"
puts stdout "  flash_write $ravenna <address> {<values>}"
puts stdout "  flash_write_hex $ravenna <hex_filename>"
puts stdout ""
