# SCPI Client (PC-side)

This folder contains Python scripts that run on the **PC** and communicate with the SCPI server on the Red Pitaya.

## Files

- **scpi_scope_realtime.py**  
  Realtime viewer using PyQtGraph. It connects to the Red Pitaya SCPI server and displays live ADC waveform.  

- **set_trigger.py**  
  Utility script to set the trigger level on the Red Pitaya via SCPI (`TRIG:LEV <value>`).  

## Usage

1. Start the SCPI server on the Red Pitaya (see `scpi_server/README.md`).  
   You should be able to query the board with:  
   ```bash
   *IDN?
   ```  
   Expected answer: something like `OS 2 Leo Ponsin ver.`.

2. On the PC, run the scripts individually:  
   - Launch realtime scope viewer:  
     ```bash
     python scpi_scope_realtime.py
     ```
   - Or set trigger value (example):  
     ```bash
     python set_trigger.py 0x1000
     ```

## Important Notes

- **Do not run both scripts at the same time.**  
  The SCPI server can only handle one connection. If you try to launch both, the server may crash.

- **Start and stop scripts slowly.**  
  Close one script before opening the other.

- **If the realtime graph does not refresh:**  
  Close the viewer, set a new trigger value with `set_trigger.py <val>`, then restart the viewer.

- **If you do not receive the IDN string quickly (e.g. `OS 2 Leo Ponsin ver.`):**  
  Stop the SCPI server on the Red Pitaya and restart it. The SCPI server is sensitive to command casing and connection state.

