#include <stdio.h>
#include <stdlib.h>
#include "json_common.h"

JSON_Value * if_error_exit(const JSON_Value *item, const bool always_throw) {
    const JSON_Object *object = json_value_get_object(item);
    double status = json_object_get_number(object, "status");
    if (status == 0) {
        if (always_throw) {
raise:
            fputs(json_serialize_to_string_pretty(item), stderr);
            exit(EXIT_FAILURE);
        }
        return (JSON_Value *) item;
    }

    if (!json_object_has_value(object, "status") || !json_object_dothas_value(object, "status.errors"))
        goto raise;
    {
        const JSON_Object *error = json_object_get_object(object, "status");
        fprintf(stderr, "%f\t%s\t%s\n",
                json_object_get_number(error, "code"),
                /* to_string(error.at("errors")), */
                json_object_get_string(error, "message"),
                json_object_get_string(error, "status"));
        exit(EXIT_FAILURE);
    }
}

JSON_Value * if_bad_status_exit(const struct ServerResponse *response) {
    return if_error_exit(
            json_parse_string(response->body),
            response->status_code > 299
    );
}
