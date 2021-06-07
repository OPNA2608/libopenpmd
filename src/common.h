#ifndef COMMON_H
#define COMMON_H

/* boolean for C90 */
typedef enum {false, true} boolean;

/*
 * Wrap unused function arguments. Only works in debug builds.
 * Should not be usable in release code because it defeats the whole point of the warning flags
 * but it's useful when testing WIP code.
 */
#ifdef DEBUG
	#define UNUSED(x) (void)(x);
#else
	#define UNUSED(x)
#endif

#endif /* COMMON_H */
