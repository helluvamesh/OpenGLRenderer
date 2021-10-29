
---- What's this? ----

-- Utility --
- Unicode string (using ICU)
- Dynamic array
- Hash map
- Hash set
- Doubly liked list
- Single cast delegate
- Multi cast delegate
- Random number generator
- System clock
- Date time
- Timer
- Number formatting
- BitSet stored in integer types

-- 3D Math --
- Vectors
- Quaternion
- Matrices
- Various shape classes
- Float math wrapper to avoid NaNs
- Double math wrapper to avoid NaNs

-- Serialization --
- Data reflection system
- Binary strict serializer (fast, small file size)
- Binary lenient serializer (skips unexpected or mistyped data, slower, larger file size)
- JSON serializer
- hibrid CSV - JSON serializer

-- Binary --
- Binary converters
- Base64 converter

---- dependencies ----

- icu4c-69_1


---- debugging ----

define LANG_DEBUG_CONTAINERS to get meaningful error messages

e.g.:
- Properties -> C/C++ -> Preprocessor -> Preprocessor Definitions: add LANG_DEBUG_CONTAINERS
or
- put in your precompiled header: #define LANG_DEBUG_CONTAINERS


---- reflection system ----

- read ReflectionSystem.txt


---- type prefixes ----

D (Data), F: can be stack allocated
H (Heap): must be heap allocated
S (Static): static class or namespace
I: interface
E: enum
with math classes: F - float, D - double
