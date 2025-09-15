# SCPI Server (Red Pitaya)

This folder contains the SCPI server sources that must run **on the Red Pitaya board**.

## Files

- **Makefile**  
  Build instructions for the SCPI server.

- **rp.c / rp.h**  
  Low-level access helpers to read/write FPGA registers.

- **scpi_commands.c / scpi_commands.h**  
  Implementation of the SCPI commands (`*IDN?`, `scopech1?`, `TRIG:LEV`, ...).

- **scpi_main.c**  
  Entry point for the SCPI server application.

## Usage

1. Copy all files from this folder to the Red Pitaya.  
   For example, you can place them inside `/opt/redpitaya/`:

   ⚠️ Note: If you copy into `/opt/redpitaya/`, don’t forget that this folder is mounted **read-only** after a reboot.  
   Before copying, remount it with:
   ```bash
   mount -o remount,rw /opt/redpitaya
   ```

2. On the Red Pitaya, compile the sources:
   ```bash
   make all / make clean
   ```

   This generates the SCPI server executable.

3. Run the server on the Red Pitaya:
   ```bash
   ./scpi_server
   ```

   It will start listening for SCPI commands on the configured port.
