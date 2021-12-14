# Sqlite3-pcre2: Regex Extension
This is sqlite3-pcre2, an extension for sqlite3 that uses libpcre to provide the REGEXP() function.

The original source code was written by Alexey Tourbin and can be found at:
[http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git](http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git)

## Build

### Prerequisite

#### Debian

```bash
sudo apt-get install \
    libsqlite3-dev \
    libpcre-dev
```

### Installation
```bash
git clone https://github.com/christian-proust/sqlite3-pcre2
cd sqlite3-pcre2
make
make install
```

## Usage

### REGEXP instruction
```sql
-- Test if asdf starts with the letter A with the case insensitive flag
SELECT 'asdf' REGEXP '(?i)^A'
; -- => 1
-- Test if asdf starts with the letter A
SELECT 'asdf' REGEXP '(?i)^A'
; -- => 0
```


## PCRE library

The documentation of the PCRE library can be found at: [http://pcre.org/](http://pcre.org/).

The regular expression syntax documentation can be found [https://perldoc.perl.org/perlre](here).


## Changelog

### 2021 UPDATE

- Rename the lib from pcre.so to pcre2.so.

### 2020 UPDATE

Updated to work with MacOS. Modified the Makefile to compile and install on MacOS, changed C source code to get rid of "no string" error. (credit to @santiagoyepez for a less hacky fix of explicitly checking for nulls)

### 2006-11-02 Initial version

The original source code was written by Alexey Tourbin and can be found at:
[http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git](http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git)
