#include "languages.h"

#include <stdlib.h>

void destroy_languages(Languages languages) { free(languages.buffer); }