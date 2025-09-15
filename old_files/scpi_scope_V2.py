# scpi_test_terminal.py
import socket
import matplotlib.pyplot as plt
import numpy as np

IP   = "192.168.1.15"
PORT = 5010

TIMEOUT_IDN   = 5
TIMEOUT_SCOPE = 30
SHOW_ALL      = False
N_PREVIEW     = 100

SCALE_V_PER_LSB = 0.976 / 8192.0  # ≈ 0.000119 V/LSB

def recv_until_crlf(sock, chunk_size=65536):
    buf = bytearray()
    while True:
        chunk = sock.recv(chunk_size)
        if not chunk:
            break
        buf += chunk
        if len(buf) >= 2 and buf[-2:] == b"\r\n":
            break
    return bytes(buf)

def main():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connexion
            s.settimeout(TIMEOUT_IDN)
            s.connect((IP, PORT))

            # 1) *IDN?
            s.sendall(b"*IDN?\r\n")
            idn = recv_until_crlf(s).decode(errors="replace").strip()
            print("IDN :", idn)

            # 2) scopech1?
            s.settimeout(TIMEOUT_SCOPE)
            s.sendall(b"scopech1?\r\n")
            raw = recv_until_crlf(s).decode(errors="replace").strip()

            if raw.startswith("ERR"):
                print("Erreur SCPI :", raw)
                return

            parts = [p for p in raw.split(",") if p != ""]
            print(f"Reçu {len(parts)} échantillons.")

            try:
                vals = np.array(list(map(int, parts)), dtype=np.int32)
            except Exception:
                print("Impossible de parser les valeurs en entiers.")
                return

            if SHOW_ALL or len(parts) <= N_PREVIEW:
                print("Valeurs (codes ADC) :", ", ".join(parts))
            else:
                print(f"Premiers {N_PREVIEW} :", ", ".join(parts[:N_PREVIEW]))
                print("…")
                print("Derniers 10 :", ", ".join(parts[-10:]))

            # Conversion en volts
            volts = vals * SCALE_V_PER_LSB

            # Stats
            v_min = float(np.min(volts))
            v_max = float(np.max(volts))
            v_avg = float(np.mean(volts))

            print(f"Stats (V): min={v_min:.6f}  max={v_max:.6f}  avg={v_avg:.6f}")

            # ==== Plot ====
            fig, ax = plt.subplots(figsize=(10, 4))
            x = np.arange(len(volts))
            ax.plot(x, volts, marker=".", markersize=2, linestyle="-")
            ax.set_title("Acquisition scopech1 (Volts)")
            ax.set_xlabel("Échantillon")
            ax.set_ylabel("Tension (V)")
            ax.grid(True)

            # Texte sur le côté droit (dans la marge)
            stats_text = (
                f"max = {v_max:.6f} V\n"
                f"min = {v_min:.6f} V\n"
                f"avg = {v_avg:.6f} V"
            )
            # position (x=1.02, y=0.5) -> juste à droite du graphe, centré verticalement
            ax.text(1.02, 0.5, stats_text,
                    transform=ax.transAxes,
                    va="center", ha="left",
                    fontsize=10,
                    bbox=dict(facecolor="white", alpha=0.6, edgecolor="none"))

            plt.tight_layout()
            plt.show()

    except socket.timeout:
        print("Timeout : pas de reponse du serveur")
    except Exception as e:
        print("Erreur reseau :", e)

if __name__ == "__main__":
    main()
