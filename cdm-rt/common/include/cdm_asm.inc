#ifdef __COCAS__
  #define RSECT(name)       rsect .text.name
  #define GLOBAL(name)      name>
  #define EXTERN(name)      name: ext
  #define ZERO              dw 0
  #define END               end.
#else
  #define RSECT(name)       .section .text.name, "ax", @progbits
  #define GLOBAL(name)      name: .global name
  #define EXTERN(name)
  #define ZERO              zero
  #define END
#endif
