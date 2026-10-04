# Kryppi

Kryppi er et lille mesh-netværksprojekt til Raspberry Pi. Det bruger
`batman-adv` til at forbinde noder over et trådløst ad-hoc-netværk og sender
beskeder mellem noderne med UDP. Beskederne serialiseres som JSON og krypteres
med en delt nøgle, før de sendes gennem mesh-netværket.

Projektet indeholder:

- en UDP-baseret mesh-node i `udp-socket/`
- en terminalbaseret brugerflade (TUI) i `tui/`
- scripts til opsætning og oprydning af `batman-adv` på Raspberry Pi i
  `BATMAN/`

## Forudsætninger

Build-kommandoerne er skrevet til Linux. Til den almindelige UDP-node skal du
bruge:

- g++ med C++11-understøttelse
- GNU Make

Til TUI'en skal du desuden bruge:

- CMake 3.15 eller nyere
- ncurses med udviklingsheaders (`libncurses-dev` på Debian/Ubuntu)

På en Debian- eller Ubuntu-baseret maskine kan afhængighederne installeres
sådan:

```bash
sudo apt update
sudo apt install build-essential cmake libncurses-dev
```

## Konfiguration af secrets

Krypteringsnøglen ligger ikke i kildekoden. Opret en lokal secrets-fil ud fra
eksemplet:

```bash
cd udp-socket
cp secrets.example .secrets
chmod 600 .secrets
```

Åbn derefter `.secrets` og erstat eksempelværdien:

```text
CRYPTO_KEY=indsæt-en-lang-tilfældig-noegle-her
```

`.secrets` er ignoreret af Git og må ikke pushes til et offentligt repository.
Programmet leder som standard efter `.secrets` i den aktuelle mappe. Hvis
filen ligger et andet sted, kan placeringen angives med:

```bash
export KRYPPI_SECRETS_FILE=/sti/til/.secrets
```

Alle noder, der skal kommunikere, skal bruge den samme nøgle.

## Byg UDP-noden

Gå til UDP-projektet og kør Make:

```bash
cd udp-socket
make
```

Det bygger programmet:

```text
udp-socket/build/bin/mesh_test
```

Nyttige Make-kommandoer:

```bash
make info   # Vis hvilke kildefiler der bygges
make clean  # Fjern build-output
```

## Kør UDP-noden

Noden kan startes med en lokal IP-adresse:

```bash
cd udp-socket
./build/bin/mesh_test 10.0.0.1
```

En anden broadcast-adresse kan angives som andet argument:

```bash
./build/bin/mesh_test 10.0.0.1 10.0.255.255
```

Hvis IP-adressen ikke angives som argument, forsøger programmet at læse den
fra `~/mesh/node_ip.txt`, som oprettes af `BATMAN/setup-mesh.sh`.

## Byg TUI'en

TUI'en bygges separat med CMake:

```bash
cd tui
cmake -S . -B build
cmake --build build
```

Det bygger programmet:

```text
tui/build/kryppi
```

Kør TUI'en fra `udp-socket`-mappen, så den automatisk finder
`udp-socket/.secrets`:

```bash
cd ../udp-socket
../tui/build/kryppi
```

TUI'en læser normalt node-IP'en fra `~/mesh/node_ip.txt`. Hvis filen ikke
findes, skal mesh-netværket først sættes op, eller programmet kan startes i
sin lokale UI-testtilstand med et argument:

```bash
../tui/build/kryppi test
```

## Raspberry Pi og batman-adv

På hver Raspberry Pi kan mesh-netværket sættes op med:

```bash
cd BATMAN
chmod +x setup-mesh.sh
./setup-mesh.sh
```

Scriptet installerer `batctl`, opretter `~/mesh/`, konfigurerer `wlan0` i
ad-hoc-tilstand og opretter en systemd-service. Scriptet genstarter Pi'en, når
opsætningen er færdig.

Når mesh-netværket er oppe, kan forbindelsen kontrolleres med:

```bash
ip addr show bat0
sudo batctl if
sudo batctl n
```

Se [BATMAN/README.md](BATMAN/README.md) for flere detaljer og oprydning med
`cleanup-mesh.sh`.

## Projektstruktur

```text
BATMAN/       Opsætning og oprydning af batman-adv
tui/          CMake-projekt for terminalbrugerfladen
udp-socket/   UDP-node, JSON-håndtering og kryptering
```

Build-mapper, runtime-filer og lokale secrets er ignoreret af Git. Kontrollér
altid `git status --ignored` og `git diff --cached`, før projektet pushes til
et offentligt repository.
