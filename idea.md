# Reverse Engineering Manuel

Objectif : comprendre efficacement un binaire sans lire tout l'assembleur ligne par ligne et sans utiliser d'outil d'automatisation de resolution.

## Principe

Le reverse efficace se fait en couches :

```text
metadata -> protections -> strings -> symboles/imports -> main -> calls -> conditions -> data flow -> pseudo-code -> verification dynamique
```

Il ne faut pas commencer par lire tout `objdump` du haut vers le bas.

## 1. Identifier le binaire

```sh
file ./binary
```

Questions :

- Architecture : `32-bit` ou `64-bit` ?
- Format : ELF ?
- Dynamique ou statique ?
- Stripped ou not stripped ?
- SUID/SGID visible ?

## 2. Regarder les protections

```sh
checksec --file ./binary
```

Lire :

- `RELRO` : protection de certaines zones de relocation/GOT.
- `STACK CANARY` : detecte certains buffer overflows avant le `ret`.
- `NX` : empeche ou autorise l'execution de code sur la stack.
- `PIE` : randomise ou fixe les adresses du binaire.
- `RPATH/RUNPATH` : chemins de bibliotheques potentiellement dangereux.

Interpretation rapide :

```text
No canary found  -> overflow stack plus simple a exploiter.
NX disabled      -> shellcode sur la stack possible.
NX enabled       -> plutot ret2libc / ROP / fonction existante.
No PIE           -> adresses du binaire fixes.
PIE enabled      -> adresses du binaire randomisees avec ASLR.
No RELRO         -> GOT potentiellement modifiable.
Full RELRO       -> GOT en lecture seule apres chargement.
```

## 3. Chercher les chaines lisibles

```sh
strings ./binary
```

Chercher :

- Messages d'erreur ou de succes.
- Chemins : `/bin/sh`, `/tmp/file`, `/home/...`.
- Mots de passe ou constantes.
- Formats : `%s`, `%x`, `%n`.
- Commandes shell.

## 4. Lister les symboles

```sh
nm ./binary
objdump -t ./binary
readelf -s ./binary
```

But : trouver les noms utiles.

Chercher :

- `main`
- fonctions custom : `run`, `win`, `secret`, `n`, `m`, `p`, etc.
- variables globales
- fonctions externes/importees

Notes :

- `nm` liste les symboles connus dans le binaire.
- `readelf` lit les structures ELF : headers, sections, symboles, relocations.
- `objdump` sert surtout a desassembler et afficher tables/sections.

## 5. Lister les imports

Pour un binaire dynamique :

```sh
objdump -d -M intel ./binary
readelf -r ./binary
```

Dans `.plt`, ne pas lire les instructions. Regarder seulement les noms :

```text
gets@plt
printf@plt
system@plt
strcmp@plt
```

Regle : `.plt` sert a reperer les imports, pas a etre reversee ligne par ligne.

## 6. Aller directement au code utile

Dans `gdb` :

```gdb
set disassembly-flavor intel
break main
run
disas main
info functions
```

Ignorer sauf besoin precis :

```text
.init
.plt instructions internes
_start
__libc_start_main
frame_dummy
constructors/destructors
.fini
```

## 7. Lire les appels avant les details

Dans une fonction, commencer par les `call` :

```asm
call gets
call strcmp
call system
```

Construire un squelette :

```c
gets(buffer);
if (strcmp(input, secret) == 0)
    system("...");
```

Ensuite seulement, lire les `mov`, `lea`, `cmp`, `jxx` pour comprendre les arguments et conditions.

## 8. Comprendre les conventions d'appel

### i386 / 32-bit

Les arguments sont generalement sur la stack :

```asm
mov DWORD PTR [esp], eax
call function
```

Premier argument : `[esp]` juste avant le `call`.

Retour de fonction : `eax`.

Commandes utiles :

```gdb
p/x $eax
p/d $eax
x/wx $esp
x/s *(char **)$esp
```

### x86_64 / 64-bit

Arguments principaux :

```text
rdi, rsi, rdx, rcx, r8, r9
```

Retour de fonction : `rax`.

## 9. Suivre les donnees

Question principale :

```text
D'ou vient cette valeur ?
Ou est-elle stockee ?
Avec quoi est-elle comparee ?
Ou est-elle utilisee ?
```

Ne pas suivre toutes les instructions. Suivre les donnees controlees par l'utilisateur.

## 10. Comprendre les branches

Instructions importantes :

```text
cmp, test
je, jne, jg, jl, ja, jb
call
ret
```

But : reconstruire :

```c
if (condition)
    success();
else
    fail();
```

## 11. Tester dynamiquement

Ne pas deviner. Verifier avec `gdb`.

Commandes utiles :

```gdb
break main
run argument
ni
si
finish
info registers
x/s $eax
x/wx $esp
x/20wx $esp
disas main
```

Quand un registre contient une adresse de string :

```gdb
x/s $eax
```

Quand un registre contient un nombre :

```gdb
p/d $eax
p/x $eax
```
