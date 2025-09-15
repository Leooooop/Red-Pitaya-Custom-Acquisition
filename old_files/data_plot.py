# data_plot.py
import os, sys, subprocess
import pandas as pd
import plotly.express as px

FS = 125e6  # 125 MHz

csv = sys.argv[1] if len(sys.argv) > 1 else "data.csv"
df = pd.read_csv(csv)
df["t_us"] = df["index"] / FS * 1e6

fig = px.line(df, x="t_us", y="value_raw", title="Acquisition FIFO @125 MHz", render_mode="webgl")
fig.update_traces(line=dict(width=1))
fig.update_layout(xaxis_title="Temps [µs]", yaxis_title="Valeur (signée)")

out_html = os.path.abspath(os.path.splitext(csv)[0] + ".html")
fig.write_html(out_html, auto_open=False)

# Ouvre via explorer.exe (Windows) — pas de PowerShell
try:
    win_path = subprocess.check_output(["wslpath", "-w", out_html]).decode().strip()
    subprocess.run(["explorer.exe", win_path], check=False)
    print("OK →", out_html)
except Exception as e:
    print("Fichier généré :", out_html)
    print("Ouvre-le manuellement avec :")
    print('  explorer.exe "$(wslpath -w %s)"' % out_html)
