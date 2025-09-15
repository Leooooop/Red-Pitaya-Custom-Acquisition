import subprocess
from pathlib import Path
import sys

# Directories (relative to Internship)
project_dir = Path("redpitaya_custom_fpga.xpr") / "redpitaya_custom_fpga" / "redpitaya_custom_fpga.runs" / "impl_1"
bit_path = project_dir / "top_level.bit"
bif_path = project_dir / "top_level.bif"
bin_path = project_dir / "top_level.bit.bin"

# Check if the .bit file exists
if not bit_path.exists():
    print(f"❌ Bitstream not found: {bit_path.resolve()}")
    sys.exit(1)

# 1. Write the BIF file
with open(bif_path, "w") as f:
    f.write(f"all:{{ {bit_path.resolve()} }}")

# 2. bootgen
print("⚙️  Generating the .bin file...")
subprocess.run([
    "bootgen",
    "-image", str(bif_path),
    "-arch", "zynq",
    "-process_bitstream", "bin",
    "-o", str(bin_path),
    "-w"
], check=True)

# 3. scp
print("📤 Uploading the bitstream to the Red Pitaya...")
subprocess.run([
    "scp",
    str(bin_path),
    "root@192.168.1.15:/opt/redpitaya/fpga/"
], check=True)

print("✅ Done.")
