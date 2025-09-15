# scpiV1.py
import socket
import matplotlib.pyplot as plt

IP   = "192.168.1.15"
PORT = 5010

TIMEOUT_IDN   = 5      # s
TIMEOUT_SCOPE = 30     # s (garde large)
SHOW_ALL      = False  # True => imprime toutes les valeurs
N_PREVIEW     = 100    # nombre de valeurs à afficher en aperçu

def recv_until_crlf(sock, chunk_size=65536):
    """Lit jusqu'à ce que la réponse se termine par \\r\\n."""
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

            # conversion en entiers
            try:
                vals = list(map(int, parts))
                print(f"min={min(vals)}  max={max(vals)}")
            except Exception:
                vals = []
                print("Impossible de parser les valeurs en entiers.")

            # aperçu
            if SHOW_ALL or len(parts) <= N_PREVIEW:
                print("Valeurs :", ", ".join(parts))
            else:
                print(f"Premiers {N_PREVIEW} :", ", ".join(parts[:N_PREVIEW]))
                print("…")
                print("Derniers 10 :", ", ".join(parts[-10:]))

            # === Visualisation matplotlib ===
            if vals:
                plt.figure(figsize=(10,4))
                plt.plot(vals, marker=".", markersize=2, linestyle="-")
                plt.title("Acquisition scopech1")
                plt.xlabel("Échantillon")
                plt.ylabel("Valeur")
                plt.grid(True)
                plt.show()

    except socket.timeout:
        print("Timeout : pas de reponse du serveur")
    except Exception as e:
        print("Erreur reseau :", e)

if __name__ == "__main__":
    main()
