/*
 * Initially written by Alexey Tourbin <at@altlinux.org>.
 *
 * The author has dedicated the code to the public domain.  Anyone is free
 * to copy, modify, publish, use, compile, sell, or distribute the original
 * code, either in source code form or as a compiled binary, for any purpose,
 * commercial or non-commercial, and by any means.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pcre.h>
#include <sqlite3ext.h>

SQLITE_EXTENSION_INIT1

typedef struct {
    char *pattern_str;
    pcre *pattern_code;
    pcre_extra *pattern_extra;
} cache_entry;

#ifndef CACHE_SIZE
#define CACHE_SIZE 16
#endif

static
void regexp(sqlite3_context *ctx, int argc, sqlite3_value **argv) {
    const char *pattern_str, *subject_str;
    pcre *pattern_code;
    pcre_extra *pattern_extra;

    assert(argc == 2);

    pattern_str = (const char *) sqlite3_value_text(argv[0]);
    if (!pattern_str) {
        sqlite3_result_error(ctx, "no regexp", -1);
        return;
    }

    /* check null */
    if (sqlite3_value_type(argv[1]) == SQLITE_NULL) {
        return;
    }

    subject_str = (const char *) sqlite3_value_text(argv[1]);
    if (!subject_str) {
        sqlite3_result_error(ctx, "no string", -1);
        return;
    }

    /* simple LRU cache */
    {
        int i;
        int found = 0;
        cache_entry *cache = sqlite3_user_data(ctx);

        assert(cache);

        for (i = 0; i < CACHE_SIZE && cache[i].pattern_str; i++)
            if (strcmp(pattern_str, cache[i].pattern_str) == 0) {
                found = 1;
                break;
            }
        if (found) {
            if (i > 0) {
                cache_entry c = cache[i];
                memmove(cache + 1, cache, i * sizeof(cache_entry));
                cache[0] = c;
            }
        } else {
            cache_entry c;
            const char *err;
            int pos;
            c.pattern_code = pcre_compile(pattern_str, 0, &err, &pos, NULL);
            if (!c.pattern_code) {
                char *e2 = sqlite3_mprintf("%s: %s (offset %d)", pattern_str, err, pos);
                sqlite3_result_error(ctx, e2, -1);
                sqlite3_free(e2);
                return;
            }
            c.pattern_extra = pcre_study(c.pattern_code, 0, &err);
            c.pattern_str = strdup(pattern_str);
            if (!c.pattern_str) {
                sqlite3_result_error(ctx, "strdup: ENOMEM", -1);
                pcre_free(c.pattern_code);
                pcre_free(c.pattern_extra);
                return;
            }
            i = CACHE_SIZE - 1;
            if (cache[i].pattern_str) {
                free(cache[i].pattern_str);
                assert(cache[i].pattern_code);
                pcre_free(cache[i].pattern_code);
                pcre_free(cache[i].pattern_extra);
            }
            memmove(cache + 1, cache, i * sizeof(cache_entry));
            cache[0] = c;
        }
        pattern_code = cache[0].pattern_code;
        pattern_extra = cache[0].pattern_extra;
    }

    {
        int rc;
        assert(pattern_code);
        rc = pcre_exec(pattern_code, pattern_extra, subject_str, strlen(subject_str), 0, 0, NULL, 0);
        sqlite3_result_int(ctx, rc >= 0);
        return;
    }
}

int sqlite3_extension_init(sqlite3 *db, char **err, const sqlite3_api_routines *api) {
    SQLITE_EXTENSION_INIT2(api)
    cache_entry *cache = calloc(CACHE_SIZE, sizeof(cache_entry));
    if (!cache) {
        *err = "calloc: ENOMEM";
        return 1;
    }
    sqlite3_create_function(db, "REGEXP", 2, SQLITE_UTF8, cache, regexp, NULL, NULL);
    return 0;
}
