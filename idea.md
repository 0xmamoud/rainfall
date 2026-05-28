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

Exemple :

```text
ELF 32-bit LSB executable, Intel 80386, dynamically linked, not stripped
```

Conclusion :

- `32-bit` : arguments souvent passes sur la stack.
- `64-bit` : premiers arguments souvent dans les registres (`rdi`, `rsi`, `rdx`, etc.).
- `dynamically linked` : imports visibles via PLT/GOT.
- `not stripped` : noms de fonctions souvent disponibles.

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

Si `strings` ne marche pas :

```sh
grep -a '/bin' ./binary
grep -a 'No' ./binary
objdump -s ./binary
hexdump -C ./binary
xxd ./binary
```

Si l'erreur est :

```text
/usr/bin/strings: Input/output error
```

Le probleme peut venir de l'outil `/usr/bin/strings` lui-meme, pas forcement du binaire analyse.

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

Imports suspects :

```text
gets, strcpy, strcat, sprintf, scanf     -> overflow possible
printf, fprintf                         -> format string possible
system, execve                          -> execution de commande possible
atoi, strcmp, strncmp, memcmp           -> comparaison d'entree
open, read, write                       -> interaction fichiers/syscalls
malloc, free                            -> heap possible
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

Si `main` n'est pas nomme, le retrouver via l'appel a `__libc_start_main`.

En i386, on voit souvent :

```asm
push adresse_de_main
call __libc_start_main
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

Exemples :

```text
argv[1] -> atoi -> eax -> cmp 0x1a7
stdin -> gets -> buffer stack -> saved EIP
input -> printf(input) -> format string
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

Conversions utiles :

```gdb
p/d 0x50
p/d 0x1a7
p/x 423
```

## 11. Cartographier la stack

Pour un overflow :

```gdb
info registers
info frame
x/40wx $esp
p/x $esp
p/x $ebp
```

Chercher :

- Debut du buffer.
- Taille approximative du buffer.
- Saved EBP.
- Saved EIP / adresse de retour.
- Distance entre l'entree utilisateur et le retour.

Exemple :

```asm
sub esp,0x50
lea eax,[esp+0x10]
call gets
```

Lecture :

```text
0x50 = 80 octets reserves
buffer commence a esp + 0x10
80 - 16 = 64 octets environ pour le buffer
gets ne verifie pas la taille -> overflow possible
```

## 12. Tester dynamiquement

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

## 13. Tester avec des entrees controlees

Utiliser des entrees simples :

```text
AAAA
AAAAAAAAAAAAAAAA
```

Puis observer :

- Le programme crash ?
- `EIP`/`RIP` change ?
- Une condition change ?
- Une variable est ecrasee ?

Generer une entree longue a la main :

```sh
python -c 'print("A" * 80)'
```

Ce n'est pas une resolution automatique : c'est seulement une entree de test reproductible.

## 14. Reconstruire le pseudo-code

Le fichier `source.c` attendu n'a pas besoin d'etre le vrai code original.

Il doit etre comprehensible et fidele au comportement exploite.

Exemple :

```c
int main(int argc, char **argv)
{
    if (atoi(argv[1]) == 423)
        execv("/bin/sh", args);
    else
        write(1, "No !\n", 5);
}
```

But : expliquer la faille, pas reproduire exactement le compilateur.

## 15. Documenter

Pour chaque niveau, garder :

- Les observations importantes.
- Les commandes utiles.
- Le raisonnement.
- Le pseudo-code.
- L'exploitation finale.
- Pourquoi ca fonctionne.

Eviter :

- Mettre un binaire dans le repo.
- Copier un dump enorme inutile.
- Mettre une solution sans explication.

## Checklist Rapide

```text
1. file ./binary
2. checksec --file ./binary
3. strings ./binary ou alternative si strings casse
4. nm ./binary / readelf -s ./binary / objdump -t ./binary
5. Reperer les imports via .plt
6. gdb -> break main -> disas main
7. Lire les call
8. Identifier les entrees utilisateur
9. Suivre les comparaisons et branches
10. Verifier les valeurs avec gdb
11. Reconstruire le pseudo-code
12. Tester l'exploitation minimale
13. Documenter proprement
```
