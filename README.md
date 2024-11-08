# Franchesckaar – Experiment #1: clipped hair

Clipped hair is a meta-programming language. Kind of. At the root, it only does 
one thing: dynamic search-and-replace. From there, no one knows where it is 
heading.

## Status of the project

- Is the project maintained? *Not really*
- Is it useful? *Likely not*
- Why does it exist? *For fun*

## Syntax of the language

The language is made to allow describing both the structure of a language 
itself, and the content of the program, in a single unified language, in a 
single file. As such, it is very raw, too raw for many practical use cases.

### Evolving from raw bytes

The first thing the compiler can do is understanding bytes. For instance, 
writing `0x90`, ASCII-encoded in a file (four bytes) will result in a single 
byte in the output file, being `0x90`… but that's not much of a feature: as 
such, it would be a poor hexadecimal editor.

### At core: search-and-replace

The main feature is supporting search-and-replace of any sequence of bytes to 
these raw bytes, using a syntax similar to Vim search-and-replace. If an input 
file therefore contains: `/nop/0x90/ nop nop`, this file will be compiled into 
two `0x90` bytes. Syntax is described in the same file as program, as stated.

### Advanced features

The first iteration of the compiler only supported search-and-replace, which 
indeed was quite inconvenient. For instance, when writing an x86 assembly-ish 
language, one might want to make dynamic structures such as `push %dx push %ax` 
giving two output bytes `0x52` and `0x50`. While this could be described as two 
different commands using something like:

```
/push %dx/0x52/
/push %ax/0x50/

push %dx
push %ax
```

It would be a waste of time to have to describe all of the possible 
combinations (think about: you need to cover all registers, then do the same 
for `pop`, and so on). So byte-modifiers were added to the language, such as 
the or-modifier:

```
/push/0x50/
/%ax/|0x00/
/%dx/|0x02/

push %dx
push %ax
```

As their name indicates, byte-modifiers modify the last byte, here or-ing the 
given value with that of the byte.
