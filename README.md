# Sqlite3-pcre: Regex Extension (MacOS Version)
This is sqlite3-pcre, an extension for sqlite3 that uses libpcre to provide the REGEXP() function.

The original source code was written by Alexey Tourbin and can be found at:

[http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git](http://git.altlinux.org/people/at/packages/?p=sqlite3-pcre.git)  

## 2020 UPDATE:
Updated to work with MacOS. Modified the Makefile to compile and install on MacOS, changed C source code to get rid of "no string" error. (credit to @santiagoyepez for a less hacky fix of explicitly checking for nulls) 

## Usage
```bash
git clone https://github.com/MatthewWolff/sqlite3-pcre
cd sqlite3-pcre
make
make install
```

You can then define a command line function to search whatever database you desire—I wanted to search all my iMessages. 
```bash
search_messages() {
  regex="$@"
  database="$HOME/library/messages/chat.db"
  use_extension=".load /usr/lib/sqlite3/pcre.so"
  query="SELECT text FROM message WHERE text REGEXP '$regex'"
  sqlite3 "$database" -cmd "$use_extension" "$query"
}
```

