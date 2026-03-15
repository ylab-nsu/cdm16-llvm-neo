#ifdef __COCAS__
  #define RSECT(name)       rsect name
  #define GLOBAL(name)      name>
  #define EXTERN(name)      name: ext
  #define END               end.
#else
  #define RSECT(name)       .section .text.name
  #define GLOBAL(name)      name: .global name
  #define EXTERN(name)
  #define END
#endif
