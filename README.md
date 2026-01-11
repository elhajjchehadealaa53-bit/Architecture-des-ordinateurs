# MiniRISC — Rapport court + mode d’emploi

Ce dépôt contient :
- un **émulateur MiniRISC** (dossier `emulator/`)
- deux **programmes embarqués de test** (dossier `embedded_software/`) :
  - `test_hello/` : test **assembleur** 
  - `test_c/` : test **C**  (compile, mais ne s’exécute pas encore correctement)

---

## Structure du dépôt (racine)

- `embedded_software/` : code source des programmes embarqués de test
  - `test_c/` : exemple en C  
    Fichiers principaux : `main.c`, `Makefile`, `minirisc_init.S`, `minirisc.ld`, `syscalls.c`
  - `test_hello/` : exemple en assembleur  
    Fichiers principaux : `Makefile`, `test.S`

- `emulator/` : émulateur MiniRISC (sources C + Makefile)
  - Fichiers d’implémentation : `main.c`, `minirisc.c`, `memory.c`, `platform.c`, etc.
  - `build/` : artefacts de compilation générés par `make` (ex. `build/minirisc`)

> Remarque : les sous-dossiers `build/` contiennent des fichiers générés automatiquement (objets, dépendances, exécutables).

## 1) But du projet

Le but du projet est de fournir une plate‑forme minimale pour :
- **compiler** des programmes pour l’ISA MiniRISC,
- **charger** un binaire brut en mémoire simulée,
- **exécuter** ce programme dans un **émulateur** afin de valider le processeur (fetch/decode/execute) et l’I/O basique.

---

## 2) Fonctionnalités visées

### Côté émulateur
- Exécuter un binaire `.bin` chargé en RAM à partir de `0x80000000`
- Implémenter les instructions MiniRISC nécessaires aux tests (`RV32IM` + quelques extensions selon toolchain)
- Gérer la **mémoire simulée** et un périphérique d’I/O simple :
  - **CharOut** : sortie console via MMIO (écriture de caractères / entiers)

### Côté logiciels embarqués
- Fournir au moins :
  - un test simple en **assembleur**
  - un test en **C** (startup + linker script + syscalls) pour valider une exécution plus réaliste

---

## 3) État actuel : ce qui marche / ne marche pas

### Ce qui marche
- L’émulateur compile et s’exécute.
- Le programme assembleur `test_hello` compile et s’exécute correctement :
  - affichage console OK (Hello World).

### Ce qui ne marche pas (fonctionnalité visée non atteinte)
- Le programme C `test_c` (fichier `esw.bin`) **compile**, mais l’exécution sur l’émulateur échoue avec :
  - `Erreur fetch: lecture invalide à PC=0x7a370d84`

Interprétation : le processeur simulé saute vers une adresse invalide (PC hors zone mémoire), ce qui suggère
qu’il manque (ou qu’il existe une erreur dans) l’implémentation/décodage d’une instruction, d’un immédiat (branch/jump),
ou d’un mécanisme attendu par le runtime C.

---

## 4) Compilation et exécution

### Prérequis
- Linux, `make`, `gcc`
- Toolchain MiniRISC disponible dans le PATH :
  - `riscv32-MINIRISC-elf-gcc`, `riscv32-MINIRISC-elf-objcopy`, etc.

---

### 4.1 Compiler le test assembleur (`test_hello`)

```bash
cd embedded_software/test_hello
make clean
make
```

Exemple de sortie (résumé) :
- compilation `test.S` → `build/test.elf`
- conversion ELF → binaire : `build/test.bin`

---

### 4.2 Compiler l’émulateur

```bash
cd emulator
make clean
make
# produit : build/minirisc
```

---

### 4.3 Exécuter un programme sur l’émulateur

L’émulateur prend un **binaire brut** (`.bin`) en argument.

#### Exécuter `test_hello` (OK)

```bash
cd emulator
./build/minirisc ../embedded_software/test_hello/build/test.bin
```

Sortie observée :
```
Premier mot @0x80000000 = 0x00000502
Hello, World!
What's up?
```

#### Exécuter `test_c` (KO actuellement)

Depuis la racine du projet :

```bash
./emulator/build/minirisc embedded_software/test_c/build/esw.bin
```

Sortie observée :
```
Premier mot @0x80000000 = 0x02000102
Erreur fetch: lecture invalide à PC=0x7a370d84
```

---
