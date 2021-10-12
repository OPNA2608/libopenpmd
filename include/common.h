#ifndef COMMON_H
#define COMMON_H

/* boolean for C89 */
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

#define PMD_ERRMAXSIZE 1024

extern const char* pmd_error_malloc;
extern const char* pmd_error_eof;

void PMD_SetError (const char* errorMsg);

const char* PMD_GetError ();

#define MALLOC_CHECK(varname, size) \
	varname = malloc (size); if (varname == NULL)

#endif /* COMMON_H */
